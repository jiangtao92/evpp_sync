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
        bool        needResponse_;  // �Ƿ���Ҫ�ظ�
        int32_t     timeout_;        // ��ʱʱ��
    };

    typedef std::shared_ptr<request> requestPtr;
}

#endif
