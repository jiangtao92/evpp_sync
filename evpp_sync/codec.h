#pragma once

#include <evpp/tcp_conn.h>
#include <evpp/buffer.h>

#include "message.h"

class LengthHeaderCodec {
public:
    typedef std::function<void(const evpp::TCPConnPtr&, jpp::message* message)> MessageCallback;

    explicit LengthHeaderCodec(const MessageCallback& cb)
        : messageCallback_(cb) {}

    void OnMessage(const evpp::TCPConnPtr& conn, evpp::Buffer* buf) {
        while (buf->size() >= kHeaderLen) {
            const int32_t len = buf->PeekInt32();
            if (len > INT_MAX || len < 0) {
                conn->Close();
                break;
            }

            if (buf->size() >= len + kHeaderLen) {
                buf->Skip(kHeaderLen);              // 跳过头部
                jpp::message msg;
                if (msg.read(buf->Next(len))) {
                    messageCallback_(conn, &msg);
                }
            }
            else {
                break;
            }
        }
    }

    void Send(evpp::TCPConnPtr conn, const jpp::message *msg) {
        evpp::Buffer buf;
        msg->write(buf);                // fix me: 内存拷贝
        buf.PrependInt32(buf.size());   // 头部数据
        return conn->Send(&buf);
    }

private:
    MessageCallback messageCallback_;
    const static size_t kHeaderLen = sizeof(int32_t);
};
