#pragma once
#ifndef __STRUCTDEF_H_
#define __STRUCTDEF_H_

#include <string>

#include "message.h"
#include "response.h"

namespace jpp {

    enum class Status {
        sSucceed,
        sFaild,
        sTimeout,
    };

    class send_error : public std::runtime_error {
    public:
        explicit send_error(const std::string &msg) : runtime_error(msg.c_str()) {};
        explicit send_error(const char *msg) : runtime_error(msg) {};
    };

	typedef std::function<jpp::response(jpp::message*)> RequestCallback;
	typedef std::function<void(const evpp::TCPConnPtr&)> ConnectionCallback;

}
#endif
