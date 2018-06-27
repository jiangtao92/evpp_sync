// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Server.h"

#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
    ::google::InitGoogleLogging(argv[0]);

    jpp::Server server("0.0.0.0:8801", "", 0);
    // echo
    server.SetMessageCallback([](jpp::message* msg) -> jpp::response {
        cout << msg->getBuffer().ToString().c_str() << endl;
        jpp::response resp(std::move(*msg));
        return resp;
    });

    server.Start();

    string line;
    while (std::getline(cin, line)) {
        if (line == "Quit") {
            break; // quit !
        }
    }

    return 0;
}

#include <windows.h>
#include "winmain-inl.h"

