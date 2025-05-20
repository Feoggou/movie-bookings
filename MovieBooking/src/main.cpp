// MovieBooking.cpp : Defines the entry point for the application.
//

#include "src/mb_service.hpp"
#include "src/workers.hpp"
#include <mbooking/movie_booking.h>

#include <nlohmann/json.hpp>
#include <zmq.hpp>

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

nlohmann::json execute_command(movie_booking::Service &s, string_view command_name, nlohmann::json args)
{
    if (command_name == "getPlayingMovies") {
        return s.getPlayingMovies();
    }
    else if (command_name == "getTheaterNamesForMovie") {
        std::cout << "args --------- " << args << std::endl;
        std::vector<std::string> vec = args;
        if (vec.size() == 1) {
            std::string movie = vec[0];
            return s.getTheaterNamesForMovie(movie);
        }
    }
    else if (command_name == "getAvailableSeats") {
        std::cout << "args --------- " << args << std::endl;
        std::vector<std::string> vec = args;
        if (vec.size() == 2) {
            std::string movie = vec[0];
            std::string theater = vec[1];

            return s.getAvailableSeats(movie, theater);
        }
    }
    else if (command_name == "bookSeats") {
        std::cout << "args --------- " << args << std::endl;
        std::cout << "args size: " << args.size() << std::endl;
        
        std::string client = args[0];
        std::string movie = args[1];
        std::string theater = args[2];

        std::cout << "client: " << client << std::endl;
        std::cout << "movie: " << movie << std::endl;
        std::cout << "theater: " << theater << std::endl;
        std::vector<size_t> seats = args[3];

        std::cout << "seats: " << seats.size() << " in total " << std::endl;

        return s.bookSeats(client, movie, theater, seats);
    }

    return {};
}

int main()
{
#if 0
    movie_booking::Service service;

	std::cout << "Hello World!" << std::endl;

    // 1. Create ZeroMQ context with a single IO thread
    zmq::context_t context(1);

    // 2. Create REP (reply) socket
    zmq::socket_t socket(context, zmq::socket_type::rep);

    // 3. Bind to TCP port
    socket.bind("tcp://*:52345");

    std::cout << "Server listening on port 52345...\n";

    while (true) {
        zmq::message_t request;

        // 4. Wait for next client request
        auto sock_reply = socket.recv(request, zmq::recv_flags::none);
        std::string received_msg(static_cast<char*>(request.data()), request.size());
        std::cerr << "Received: " << received_msg << std::endl;

        nlohmann::json json = nlohmann::json::parse(received_msg);
        if (json.contains("pid")) {
            std::cerr << "PID is " << json["pid"] << "; cmd=" << json["cmd"] << std::endl;
            nlohmann::json args;

            if (json.contains("args")) {
                args = json["args"];
            }
            json = execute_command(service, json["cmd"], args);
            std::cerr << "JSON result is: " << json << std::endl;
        }

        // 5. Send reply back to client
        std::string reply_msg = json.dump();
        zmq::message_t reply(reply_msg.size());
        memcpy(reply.data(), reply_msg.data(), reply_msg.size());
        socket.send(reply, zmq::send_flags::none);
    }
#else
    start_workers();

    movie_booking::SyncedService service;
    movie_booking::API api(service);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        auto movies = api.getPlayingMovies();
        auto theaters = api.getTheaterNamesForMovie("Movie A");
        //std::cout << "theater 2: " << theaters[1] << std::endl;
        auto seats_avail = api.getAvailableSeats("Movie A", "Theater 2");
        auto seats_booked = api.bookSeats("joe", "Movie A", "Theater 2", { 0 });
        seats_avail = api.getAvailableSeats("Movie A", "Theater 2");
    }
#endif

	return 0;
}
