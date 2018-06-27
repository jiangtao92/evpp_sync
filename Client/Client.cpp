// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Client.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{
    ::google::InitGoogleLogging(argv[0]);

    jpp::Client client("127.0.0.1", 8801);
    //client.setConnectionCallback([&client](const evpp::TCPConnPtr& conn) {
    //    if (conn->IsConnected()) {
    //        client.setRequestCallback([](jpp::message* msg) -> jpp::response {
    //            jpp::response resp(std::move(*msg)); // echo
    //            return resp;
    //        });
    //    }
    //    else {
    //        client.setRequestCallback(jpp::Client::RequestCallback());
    //    }
    //});

    client.Start();

    evpp::Buffer req, resp;
    std::string str = "Hello World";
    req.Append(str.c_str(), str.size());
    cout << req.ToString().c_str() << endl;
    auto ret = client.Send(&req, &resp, 2 * 1000);
    if (jpp::Status::sSucceed == ret) {
        cout << "resp: " << resp.ToString().c_str() << endl;
    }
    else {
        cout << "Send failed, error =  " << static_cast<int>(ret) << endl;
    }
    return 0;
}

#include <windows.h>
#include "winmain-inl.h"

