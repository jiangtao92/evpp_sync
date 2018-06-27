#include "Server.h"

using namespace std;
using namespace evpp;

namespace jpp {

    Server::Server(const std::string & listen_addr, const std::string &name, uint32_t thread_num)
        : loop_() // accept loop
        , server_(loop_.loop(), listen_addr, name, thread_num)
        , codec_(std::bind(&Server::onMessage, this, placeholders::_1, placeholders::_2))
        , messageCallback_([](jpp::message *) { return jpp::response(); })
        , connectionCallback_([](const evpp::TCPConnPtr & conn) {})
    {
        server_.SetMessageCallback(std::bind(&LengthHeaderCodec::OnMessage, &codec_, placeholders::_1, placeholders::_2));

        server_.SetConnectionCallback([this](const evpp::TCPConnPtr & conn) {
            if (conn->IsConnected()) {
                if (client_ && client_ != conn) {
                    conn->Close();
                }

                client_ = conn;
            }
            else
            {
                if (client_ == conn) {
                    client_.reset();
                }
            }

            connectionCallback_(conn);
        });
    }

    Server::~Server()
    {
		Stop();
    }

    bool Server::Start()
    {
        if (!loop_.IsRunning()) {
            if (server_.Init() && server_.Start()) {
                return loop_.Start(true);
            }
        }
        return false;
    }

	bool Server::Stop()
	{
		// stop server
		if (server_.IsRunning()) {
			server_.Stop([this]() {loop_.Stop(false); });
		}

		// stop thread
		while (!loop_.IsStopped()) {
			usleep(1);
		}
		return true;
	}

    //////////////////////////////////////////////////////////////////////////
    // private:
    void Server::onMessage(const evpp::TCPConnPtr & conn, jpp::message * message)
    {
        switch (message->getType())
        {
        case jpp::kRequest:
        {
			// 加入线程池,减少对io线程的阻塞
            auto result = messageCallback_(message);

            codec_.Send(conn, &result);

            break;
        }
        case jpp::kResponse:
        {
			//// todo: 多线程下存在问题
   //         auto pos = outgoing_.find(message->getReqidx());
   //         if (pos != end(outgoing_))
   //         {
   //             pos->second.set_value(*message);
   //             outgoing_.erase(pos);
   //             return;
   //         }
        }
        default:
            break;
        }
    }

    //Status Server::Send(evpp::Buffer *bufReq, evpp::Buffer *bufResp, uint32_t timeout /* = 0 */) 
    //{
    //    requestPtr req(new request(get_next_idx(), true, timeout));
    //    req->appendBuffer(*bufReq);
    //    response resp;
    //    auto ret = SendSync(req, resp);
    //    if (Status::sSucceed == ret)
    //    {
    //        bufResp->Append(resp.getBuffer());
    //    }
    //    return ret;
    //}

    //Status Server::SendSync(jpp::requestPtr req, jpp::response &resp)
    //{
    //    assert(!loop_.loop()->IsInLoopThread());

    //    std::shared_ptr<ResponsePromise> p = std::make_shared<ResponsePromise>();
    //    auto ret = p->get_future();

    //    loop_.loop()->RunInLoop([=] {
    //        // 连接OK
    //        if (client_ && client_->IsConnected()) {
    //            codec_.Send(client_, req.get());
    //            outgoing_.insert(make_pair(req->getReqidx(), std::move(*p)));
    //        }
    //        else
    //        {
    //            p->set_exception(std::make_exception_ptr(send_error("tcp disconnected!")));
    //        }
    //    });

    //    int timeout = 0;
    //    if ((timeout = req->getTimeout())) {
    //        auto wait_result = ret.wait_for(std::chrono::milliseconds(timeout));
    //        if (std::future_status::timeout == wait_result) {
    //            int idx = req->getReqidx();
    //            auto f = [idx, this] {
    //                auto pos = outgoing_.find(idx);
    //                if (pos != end(outgoing_))
    //                {
    //                    outgoing_.erase(pos);
    //                }
    //            };

    //            loop_.loop()->RunInLoop(std::move(f));

    //            return Status::sTimeout;
    //        }
    //    }

    //    try
    //    {
    //        resp = ret.get();
    //    }
    //    catch (const send_error &)
    //    {
    //        return Status::sFaild;
    //    }
    //    catch (...)
    //    {
    //        throw; // 处理不了的错误继续向上抛
    //    }
    //}
}