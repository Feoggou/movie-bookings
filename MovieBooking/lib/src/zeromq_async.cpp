#include "zeromq_async.hpp"

#include <zmq.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <chrono>

zmq::context_t context(1);
zmq::socket_t router_socket(context, zmq::socket_type::router);

// Work queue
std::mutex task_mutex;
std::queue<std::pair<zmq::message_t, zmq::message_t>> completed_responses;

void long_running_task(zmq::message_t identity, zmq::message_t content) {
    std::string msg(static_cast<char*>(content.data()), content.size());
    std::cout << "Processing: " << msg << std::endl;

    // Simulate expensive computation
    std::this_thread::sleep_for(std::chrono::seconds(3));

    std::string reply_msg = "Reply to: " + msg;
    zmq::message_t reply(reply_msg.begin(), reply_msg.end());

    // Store the result with identity
    std::lock_guard<std::mutex> lock(task_mutex);
    completed_responses.emplace(std::move(identity), std::move(reply));
}

int zeromq_async_main() {
    router_socket.bind("tcp://*:5555");

    while (true) {
        // Check if new messages are ready to be sent
        {
            std::lock_guard<std::mutex> lock(task_mutex);
            while (!completed_responses.empty()) {
                auto [identity, reply] = std::move(completed_responses.front());
                completed_responses.pop();

                router_socket.send(identity, zmq::send_flags::sndmore);
                router_socket.send(zmq::message_t(), zmq::send_flags::sndmore);  // empty delimiter frame
                router_socket.send(reply, zmq::send_flags::none);
            }
        }

        // Poll for incoming messages (non-blocking)
        zmq::pollitem_t items[] = {
            {static_cast<void*>(router_socket), 0, ZMQ_POLLIN, 0}
        };
        zmq::poll(items, 1, std::chrono::milliseconds(100));  // short timeout

        if (items[0].revents & ZMQ_POLLIN) {
            // Receive the full multipart message: [identity][empty][content]
            zmq::message_t identity;
            zmq::message_t empty;
            zmq::message_t content;

            router_socket.recv(identity);
            router_socket.recv(empty);
            router_socket.recv(content);

            // Dispatch async task
            std::thread(long_running_task, std::move(identity), std::move(content)).detach();
        }
    }
}
