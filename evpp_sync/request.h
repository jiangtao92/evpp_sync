#ifndef __REQUEST_H_
#define __REQUEST_H_

#include "message.h"

namespace jpp {

    class request : public message {

    public:
        request(int32_t reqidx, bool needResponse, int timeout)
            : message()
            , needResponse_(needResponse)
            , timeout_(timeout)
        {
            type_ = kRequest;
            reqidx_ = reqidx;
        }

        int32_t getTimeout() const { return timeout_; };
        bool getNeedResponse() const { return needResponse_; };
    private:
        bool        needResponse_;  // 是否需要回复
        int32_t     timeout_;        // 超时时间
    };

    typedef std::shared_ptr<request> requestPtr;
}

#endif
