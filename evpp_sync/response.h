#ifndef __RESPONSE_H_
#define __RESPONSE_H_

#include "message.h"

namespace jpp {
    
    class response : public message {

    public:
        response()
            : message()
        {
            type_ = kResponse;
        }

        response(message && msg)
            : message(std::move(msg))
        {
            type_ = kResponse;
        }
    };
}

#endif
