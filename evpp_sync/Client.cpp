#include "Client.h"

#include <cassert>

using namespace std;
using namespace evpp;

namespace jpp {
    Client::Client(std::string const &addr, uint16_t port)
        : loop_()
        , client_(loop_.loop(), addr + ":" + std::to_string(port), "")
        , codec_(std::bind(&Client::onMessage, this, placeholders::_1, placeholders::_2))
        , reqidx_(0)
        , requestcallback_([](jpp::message*) { return jpp::response(); })
        , connectioncallback_([](const evpp::TCPConnPtr&) {})
    {
        client_.SetConnectionCallback([this](const TCPConnPtr &conn) {
            if (connectioncallback_) {
                connectioncallback_(conn);
            }
        });

        client_.SetMessageCallback(std::bind(&LengthHeaderCodec::OnMessage, &codec_, placeholders::_1, placeholders::_2));
    }

    Client::~Client() {

        Stop();
    }

    void Client::Start() {

        if (!loop_.IsRunning()) {
            client_.Connect();

            loop_.Start(true);
        }
    }

    void Client::Stop() {

        if (loop_.IsRunning()) {
            // disconnect
            client_.Disconnect();

            // stop the thread
            loop_.Stop(true);
        }
    }

    Status Client::Send(evpp::Buffer *bufReq, evpp::Buffer *bufResp, uint32_t timeout /* = 0 */)
    {
        requestPtr req = std::make_shared<request>(get_next_idx(), true, timeout);
        req->appendBuffer(*bufReq);
        response resp;
        auto ret = SendSync(req, resp);
        if (Status::sSucceed == ret)
        {
            bufResp->Append(resp.getBuffer()); // memcpy
        }
        return ret;
    }

    //////////////////////////////////////////////////////////////////////////
    // private
    void Client::onMessage(const evpp::TCPConnPtr &conn, jpp::message *message)
    {
        switch (message->getType())
        {
        case jpp::kRequest:
        {
            auto result = requestcallback_(message);

            codec_.Send(conn, &result);

            break;
        }
        case jpp::kResponse:
        {
            auto pos = outgoing_.find(message->getReqidx());
            if (pos != end(outgoing_))
            {
                pos->second.set_value(*message);
                outgoing_.erase(pos);
                return;
            }
            break;
        }
        default:
            break;
        }
    }

    Status Client::SendSync(jpp::requestPtr req, jpp::response &resp)
    {
        assert(!loop_.loop()->IsInLoopThread());

        std::shared_ptr<ResponsePromise> p = std::make_shared<ResponsePromise>();
        auto ret = p->get_future();

        loop_.loop()->RunInLoop([=] {
            // 连接OK
            if (client_.conn() && client_.conn()->IsConnected()) {
                codec_.Send(client_.conn(), req.get());
                outgoing_.insert(make_pair(req->getReqidx(), std::move(*p)));
            }
            else
            {
                p->set_exception(std::make_exception_ptr(send_error("tcp disconnected!")));
            }
        });

        int timeout = 0;
        if ((timeout = req->getTimeout())) {
            auto wait_result = ret.wait_for(std::chrono::milliseconds(timeout));
            if (std::future_status::timeout == wait_result) {
                int idx = req->getReqidx();
                auto f = [idx, this] {
                    auto pos = outgoing_.find(idx);
                    if (pos != end(outgoing_))
                    {
                        outgoing_.erase(pos);
                    }
                };

                loop_.loop()->RunInLoop(std::move(f));

                return Status::sTimeout;
            }
        }

        try
        {
            resp = ret.get();
            return Status::sSucceed;
        }
        catch (const send_error &)
        {
            return Status::sFaild;
        }
        catch (...)
        {
            throw; // 处理不了的错误继续向上抛
        }
    }
}
