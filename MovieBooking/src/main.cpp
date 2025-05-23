// MovieBooking.cpp : Defines the entry point for the application.
//

#include "src/mb_service.hpp"
#include "src/workers.hpp"
#include <mbooking/movie_booking.h>

#include <nlohmann/json.hpp>
#include <zmq.hpp>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>

#include <iostream>
#include <chrono>
#include <thread>


using namespace std;

void json_foo()
{
    std::string json_str = R"(["apple", "banana", "cherry"])";

    // Parse JSON string
    nlohmann::json j = nlohmann::json::parse(json_str);

    // Convert to vector
    std::vector<std::string> vec = j.get<std::vector<std::string>>();

    for (const auto& s : vec) {
        std::cout << s << std::endl;
    }

    // **************

    j = vec;

    // Convert JSON to string
    json_str = j.dump(4); // Optional: j.dump(4) for pretty-print

    std::cout << "JSON string: " << json_str << std::endl;
}

std::vector<std::shared_ptr<movie_booking::IFutureWrapper>> g_futures;

void execute_command(movie_booking::API &api, const string &command_name, const nlohmann::json &args)
{
    if (command_name == "getPlayingMovies") {
        g_futures.push_back(api.getPlayingMovies());
    }
    else if (command_name == "getTheaterNamesForMovie") {
        //std::cout << "args --------- " << args << std::endl;
        std::vector<std::string> vec = args;
        if (vec.size() == 1) {
            std::string movie = vec[0];
            g_futures.push_back(api.getTheaterNamesForMovie(movie));
        }
    }
    else if (command_name == "getAvailableSeats") {
        //std::cout << "args --------- " << args << std::endl;
        std::vector<std::string> vec = args;
        if (vec.size() == 2) {
            std::string movie = vec[0];
            std::string theater = vec[1];

            g_futures.push_back(api.getAvailableSeats(movie, theater));
        }
    }
    else if (command_name == "bookSeats") {
        //std::cout << "args --------- " << args << std::endl;
        //std::cout << "args size: " << args.size() << std::endl;
        
        std::string client = args[0];
        std::string movie = args[1];
        std::string theater = args[2];

        //std::cout << "client: " << client << std::endl;
        //std::cout << "movie: " << movie << std::endl;
        //std::cout << "theater: " << theater << std::endl;
        std::vector<size_t> seats = args[3];

        //std::cout << "seats: " << seats.size() << " in total " << std::endl;

        g_futures.push_back(api.bookSeats(client, movie, theater, seats));
    }
}

void reply_thread_callback(std::stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        if (g_futures.empty()) {
            // sleep
            continue;
        }

        // go through all items and see if either is ready.
        //IFutureWrapper
    }
}

int main()
{
    start_workers();

    movie_booking::SyncedService service;
    movie_booking::API api(service);

	std::cout << "Hello World!" << std::endl;

    // 1. Create ZeroMQ context with a single IO thread
    zmq::context_t context(1);

    // 2. Create REP (reply) socket
    zmq::socket_t socket(context, zmq::socket_type::rep);

    // 3. Bind to TCP port
    socket.bind("tcp://*:52345");

    std::cout << "Server listening on port 52345...\n";

    std::jthread reply_thread(reply_thread_callback);

    while (true) {
        zmq::message_t request;

        // 4. Wait for next client request
        auto sock_reply = socket.recv(request, zmq::recv_flags::none);
        std::string received_msg(static_cast<char*>(request.data()), request.size());
        //std::cerr << "Received: " << received_msg << std::endl;

        nlohmann::json json = nlohmann::json::parse(received_msg);
        if (json.contains("pid")) {
            std::cerr << "[" << json["pid"] << "] " << json["cmd"] << std::endl;

            nlohmann::json args;

            if (json.contains("args")) {
                args = json["args"];
            }
            /*json = */execute_command(api, json["cmd"], args);
            //std::cerr << "JSON result is: " << json << std::endl;
        }

        // 5. Send reply back to client
        std::string reply_msg = json.dump();
        zmq::message_t reply(reply_msg.size());
        memcpy(reply.data(), reply_msg.data(), reply_msg.size());
        socket.send(reply, zmq::send_flags::none);
    }

	return 0;
}
