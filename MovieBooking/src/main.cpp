// MovieBooking.cpp : Defines the entry point for the application.
//

#include "src/zeromq_async.hpp"
#include "src/cmd.hpp"

#include <nlohmann/json.hpp>

#include <iostream>


using namespace std;

int main()
{
    std::cerr << "Starting..." << std::endl;

    movie_booking::create_service(zeromq_async_reply);

    zeromq_async_main([](std::string_view request_id, std::string_view received_msg) {
        nlohmann::json json = nlohmann::json::parse(received_msg);
        if (json.contains("pid"))
        {
            std::cerr << "[" << json["pid"] << "] " << json["cmd"] << std::endl;

            nlohmann::json args;

            if (json.contains("args")) {
                args = json["args"];
            }
            movie_booking::execute_command(request_id, zeromq_async_reply, json["cmd"], args);;
        }
    });

	return 0;
}
