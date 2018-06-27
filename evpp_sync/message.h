#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <evpp/buffer.h>

namespace jpp {

	enum MessageType : unsigned char
	{
		kRequest = 0,
		kResponse,
		kMessageMax,
	};

	class message {

	public:
        message() : type_(kRequest), session_(0), reqidx_(0), buffer_() {};

        message(const message &msg)
        {
            *this = msg;
        }

        message(message &&msg)
        {
            *this = std::move(msg);
        }

        message& operator=(const message &msg)
        {
            type_ = msg.type_;
            session_ = msg.session_;
            reqidx_ = msg.reqidx_;
            setBuffer(msg.buffer_);
            return *this;
        }

        message& operator=(message &&msg)
        {
            type_ = msg.type_;
            session_ = msg.session_;
            reqidx_ = msg.reqidx_;
            setBuffer(std::move(msg.buffer_));

            return *this;
        }

        // 反序列化
        bool read(const evpp::slice &s)
        {
            evpp::Buffer buf;				// fix me: 临时对象
            buf.Append(s.data(), s.size());

            return read(buf);
        }

        // 序列化
        void write(evpp::Buffer &buf) const
        {
            buf.AppendInt32(tag_);
            buf.AppendInt8(static_cast<int8_t>(type_));
            buf.AppendInt32(session_);
            buf.AppendInt32(reqidx_);
            buf.Append(buffer_.data(), buffer_.size()); // 一次memcopy
        }

        MessageType getType() const { return type_; };
        void setType(MessageType type) { type_ = type; };
        int32_t getSession() const { return session_; };
        void setSession(int32_t session) { session_ = session; };
        int32_t getReqidx() const { return reqidx_; };
        void setReqidx(int32_t reqidx) { reqidx_ = reqidx; };

        const evpp::slice getBuffer() const { return evpp::slice(buffer_.data(), buffer_.size()); };
        void setBuffer(const evpp::Buffer &buffer) { buffer_.Reset(); buffer_.Append(buffer.data(), buffer.size()); };
        void setBuffer(const evpp::Slice &s) { buffer_.Reset(); buffer_.Append(s.data(), s.size()); }
        void setBuffer(evpp::Buffer &&buffer) { buffer_.Swap(buffer); }
        void appendBuffer(const evpp::Buffer &buffer) { buffer_.Append(buffer.data(), buffer.size()); };

    private:
        bool read(evpp::Buffer &buf)
        {
            if (buf.size() < headerSize)
            {
                return false;
            }

            if (buf.PeekInt32() != tag_)
            {
                return false;
            }
            buf.Skip(sizeof(tag_));

            type_ = static_cast<MessageType>(buf.PeekInt8());
            buf.Skip(sizeof(type_));
            session_ = buf.PeekInt32();
            buf.Skip(sizeof(session_));
            reqidx_ = buf.PeekInt32();
            buf.Skip(sizeof(reqidx_));
            buffer_.Swap(buf);  // 直接使用,减少内存拷贝

            return true;
        }
	
	protected:
		MessageType     type_;      // 消息类型
        int32_t         session_;   // 会话编号
        int32_t         reqidx_;    // 消息编号
        evpp::Buffer    buffer_;    // 消息传递的内容 // fix me : 修改成指针表示,存在拷贝风险

        const static int tag_ = 0x6a707470;
        const static int headerSize = sizeof(tag_) + sizeof(MessageType) + sizeof(int32_t) * 2;
	};
}

#endif
