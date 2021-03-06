// Server.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Server.h"

#include <thread>
#include <iostream>
using namespace std;

int main(int argc, char * argv[])
{
    ::google::InitGoogleLogging(argv[0]);

    jpp::Server server("0.0.0.0:8801", "", std::thread::hardware_concurrency());
    // echo
    server.SetMessageCallback([](jpp::message* msg) -> jpp::response {
		//cout <<"tid: "<<std::this_thread::get_id() << endl;
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

	cout << "Server will stop now" << endl;
	// stop the server
	server.Stop();

    return 0;
}

#include <windows.h>
#include "winmain-inl.h"

