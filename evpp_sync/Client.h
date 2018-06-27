#pragma once
#ifndef __CLIENT_H_
#define __CLIENT_H_

#include "disable_copy.h"
#include "codec.h"
#include "request.h"
#include "response.h"
#include "structDef.h"

#include <evpp/tcp_client.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>
#include <evpp/event_loop_thread.h>

#include <unordered_map>
#include <future>
#include <atomic>

namespace jpp {

    class Client : private disable_copy {

    public:
        Client(std::string const &addr, uint16_t port);

        ~Client();

        void Start();

        void Stop();

        Status Send(evpp::Buffer *req, evpp::Buffer *resp, uint32_t timeout = 0);

		Status Send(const evpp::Slice &s, evpp::Buffer *resp, uint32_t timeout = 0);

        void setRequestCallback(const RequestCallback &cb) { requestcallback_ = cb; };

        void setConnectionCallback(const ConnectionCallback &cb) { connectioncallback_ = cb; }

    private:
        void onMessage(const evpp::TCPConnPtr &conn, jpp::message *message);

        uint32_t get_next_idx() { return ++reqidx_; };

        Status SendSync(jpp::requestPtr req, jpp::response &resp);

    private:
        typedef std::promise<jpp::message> ResponsePromise;
        evpp::EventLoopThread loop_;
        evpp::TCPClient client_;
        LengthHeaderCodec codec_;
        RequestCallback requestcallback_;
        ConnectionCallback connectioncallback_;

        std::atomic<uint32_t> reqidx_;                              // 请求计数
        std::unordered_map<uint32_t, ResponsePromise> outgoing_;    // 同步请求列表
    };
}

#endif