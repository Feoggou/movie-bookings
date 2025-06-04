// MovieBooking.cpp : Defines the entry point for the application.
//

#include "zeromq_async.hpp"
#include "src/cmd.hpp"

#include <nlohmann/json.hpp>

#include <iostream>
#include <format>

import mbooking;


int main()
{
    std::cerr << "Starting..." << std::endl;

    mbooking::create_service(zeromq_async_reply);

    zeromq_async_main([](std::string_view client_id, std::string_view received_msg) {
        nlohmann::json json = nlohmann::json::parse(received_msg);
        if (json.contains("request_id"))
        {
            std::cerr << std::format("(main) client_id='{}'; request_id='{}'; command={}", client_id, std::string(json["request_id"]), json["cmd"].dump()) << std::endl;

            nlohmann::json args;

            if (json.contains("args")) {
                args = json["args"];
            }

            movie_booking::ID id = { .client = std::string(client_id), .request = std::string(json["request_id"]) };
            movie_booking::execute_command(id, json["cmd"], args);;
        }
    });

	return 0;
}
