// MovieBooking.cpp : Defines the entry point for the application.
//

#include <nlohmann/json.hpp>
#include <zmq.hpp>

#include <iostream>

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

int main()
{
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
        socket.recv(request, zmq::recv_flags::none);
        std::string received_msg(static_cast<char*>(request.data()), request.size());
        std::cout << "Received: " << received_msg << std::endl;

        // 5. Send reply back to client
        std::string reply_msg = "Hello from server";
        zmq::message_t reply(reply_msg.size());
        memcpy(reply.data(), reply_msg.data(), reply_msg.size());
        socket.send(reply, zmq::send_flags::none);
    }

	return 0;
}
