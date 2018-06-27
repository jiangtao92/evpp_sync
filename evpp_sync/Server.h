#ifndef __SERVER_H_
#define __SERVER_H_

#include <evpp/tcp_server.h>
#include <evpp/event_loop_thread.h>

#include "structDef.h"
#include "request.h"
#include "response.h"
#include "codec.h"

#include <unordered_map>
#include <future>

namespace jpp {
    class Server {
		
	public:
        Server(const std::string & listen_addr /*ip:port*/, const std::string &name, uint32_t thread_num);

        ~Server();

        bool Start();

		bool Stop();

        void SetMessageCallback(const RequestCallback &cb) { messageCallback_ = cb; };

        void SetConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; };

        //Status Send(evpp::Buffer *req, evpp::Buffer *resp, uint32_t timeout = 0);

    private:
        void onMessage(const evpp::TCPConnPtr&, jpp::message * message);

        Status SendSync(jpp::requestPtr req, jpp::response &resp);

        uint32_t get_next_idx() { return ++reqindex_; };

    private:
        evpp::EventLoopThread loop_;
        evpp::TCPServer server_;
        LengthHeaderCodec codec_;
		RequestCallback messageCallback_;
		ConnectionCallback connectionCallback_;

        // 只支持单连接的情况
        evpp::TCPConnPtr client_;
        typedef std::promise<jpp::message> ResponsePromise;
        std::atomic<uint32_t> reqindex_;                                    // 请求计数
        std::unordered_map<uint32_t, ResponsePromise> outgoing_;            // 同步请求列表
    };
}


#endif
