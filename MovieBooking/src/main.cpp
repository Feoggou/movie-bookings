// MovieBooking.cpp : Defines the entry point for the application.
//

#include "src/workers.hpp"
#include "src/zeromq_async.hpp"
#include "src/cmd.hpp"

#include <nlohmann/json.hpp>

#include <iostream>


using namespace std;

int main()
{
    start_workers();

    movie_booking::create_service();

	std::cerr << "Starting..." << std::endl;

    zeromq_async_main([](std::string_view received_msg) -> std::string {
        nlohmann::json json = nlohmann::json::parse(received_msg);
        if (json.contains("pid"))
        {
            std::cerr << "[" << json["pid"] << "] " << json["cmd"] << std::endl;

            nlohmann::json args;

            if (json.contains("args")) {
                args = json["args"];
            }
            /*json = */movie_booking::execute_command(json["cmd"], args);
            //std::cerr << "JSON result is: " << json << std::endl;

            json = { };
            return json.dump();
        }
        return {};
        });

	return 0;
}
