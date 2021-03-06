// Client.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "Client.h"
#include "timer.hpp"
#include <iostream>

#include <future>

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

	client.setConnectionCallback([](const evpp::TCPConnPtr& conn) {
		cout << "...." << endl;
	});

    client.Start();

	bool exit = false;

	auto ret = std::async(std::launch::async, [&client, &exit]() {

		jpp::Timer timer;
		int count = 0;
		unique_ptr<char[]> bufs = std::make_unique<char[]>(4 * 1024);
		while (!exit)
		{
			evpp::Buffer req, resp;
			//std::string str = "Hello World";
			//req.Append(str.c_str(), str.size());
			req.Append(bufs.get(), 4 * 1024);

			auto ret = client.Send(&req, &resp, 2 * 1000);
			if (jpp::Status::sSucceed == ret) {
				
				if (count >= 1000) {

					cout << "total time cost " << timer.elapse() << " avg: " << timer.elapse() / (double)count << "ms" << endl;
					timer.reset();
					count = 0;
				}

				count++;
			}
			else {
				cout << "Send failed, error =  " << static_cast<int>(ret) << endl;
			}
		}
	});

	std::string line;

	while (std::getline(cin, line)) {
		if (line == "Quit") {
			exit = true;
			break;
		}
	}

	// wait async stop and exit the main thread;
	ret.get();

    return 0;
}

#include <windows.h>
#include "winmain-inl.h"

