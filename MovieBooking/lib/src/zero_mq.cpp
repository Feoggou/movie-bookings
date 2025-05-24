#include "zero_mq.hpp"

#include <zmq.hpp>

#include <iostream>

void run_zero_mq_server(std::function<std::string (std::string_view)> process_request)
{
    std::cerr << "Starting ZeroMQ server..." << std::endl;

    // 1. Create ZeroMQ context with a single IO thread
    zmq::context_t context(1);

    // 2. Create REP (reply) socket
    zmq::socket_t socket(context, zmq::socket_type::rep);

    // 3. Bind to TCP port
    socket.bind("tcp://*:52345");

    std::cerr << "Server listening on port 52345...\n";

    while (true) {
        zmq::message_t request;

        // 4. Wait for next client request
        auto sock_reply = socket.recv(request, zmq::recv_flags::none);

        auto reply_cb = [&socket](std::string_view reply_msg) {
            zmq::message_t reply(reply_msg.size());
            memcpy(reply.data(), reply_msg.data(), reply_msg.size());
            socket.send(reply, zmq::send_flags::none);
            };
        
        std::string_view request_view(static_cast<const char*>(request.data()), request.size());

        std::string reply_msg = process_request(request_view);
        
        zmq::message_t reply(reply_msg.size());
        memcpy(reply.data(), reply_msg.data(), reply_msg.size());
        socket.send(reply, zmq::send_flags::none);
    }
}