#include "zeromq_async.hpp"

#include <zmq.hpp>
#include <zmq_addon.hpp>


#include <string>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <chrono>
#include <regex>

template<typename... T>
concept AllArgsAreRValueRef = (std::is_rvalue_reference_v<T&&> && ...);

zmq::context_t context(1);
zmq::socket_t router_socket(context, zmq::socket_type::router);

template<typename... T> requires AllArgsAreRValueRef<T...>
inline auto make_vector(T&&... items)
{
    using ValueType = std::decay_t<std::common_type_t<T...>>;

    std::vector<ValueType> vec;

    vec.reserve(sizeof...(items));
    (vec.push_back(std::move(items)), ...);

    return vec;
}

// Work queue
std::mutex task_mutex;
std::queue<std::pair<zmq::message_t, zmq::message_t>> completed_responses;

void long_running_task(zmq::message_t identity, zmq::message_t content) {
    std::string msg(static_cast<char*>(content.data()), content.size());
    std::cerr << "Processing: " << msg << std::endl;

    // Simulate expensive computation
    std::this_thread::sleep_for(std::chrono::seconds(1));

    zmq::message_t reply(msg.begin(), msg.end());

    std::cerr << std::format("Sending back reply to '{}':\n    {}", identity.to_string_view(), msg) << std::endl;

    // Store the result with identity
    std::lock_guard<std::mutex> lock(task_mutex);
    completed_responses.emplace(std::move(identity), std::move(reply));
}

inline void reply_to(zmq::message_t &&identity, std::string_view reply_msg)
{
    std::cerr << std::format("REPLY TO '{}': {}\n", identity.to_string_view(), reply_msg);
    zmq::send_multipart(router_socket, make_vector(std::move(identity), zmq::message_t(reply_msg)));
}

void zeromq_async_reply(std::string_view client_id, std::string_view reply_msg)
{
    reply_to(zmq::message_t(client_id), reply_msg);
}

inline bool is_human_readable(std::string_view sv) {
    return std::ranges::all_of(sv, [](unsigned char c) {
        return std::isprint(c);
        });
}

void zeromq_async_main(std::function<void(std::string_view, std::string_view)> process_request)
{
    router_socket.bind("tcp://*:52345");

    while (true) {
#if 0
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
#endif

        // Poll for incoming messages (non-blocking)
        zmq::pollitem_t items[] = {
            {static_cast<void*>(router_socket), 0, ZMQ_POLLIN, 0}
        };
        zmq::poll(items, 1, std::chrono::milliseconds(100));  // short timeout

        if (items[0].revents & ZMQ_POLLIN) {

            std::vector<zmq::message_t> frames;
            zmq::recv_result_t result = zmq::recv_multipart(router_socket, std::back_inserter(frames));
            if (!result) {
                std::cerr << "Failed to receive multipart message!" << std::endl;
                continue;
            }

            if (frames.size() != 2) {
                std::cerr << std::format("Received the wrong number of frames. Expected: {} frames; Actual: {} frames. Please make sure your client socket is a DEALER.", 2, frames.size()) << std::endl;
                reply_to(std::move(frames[0]), R"({"error": "Internal error"})");
                continue;
            }

            zmq::message_t identity = std::move(frames[0]);
            zmq::message_t content = std::move(frames[1]);

            std::cerr << "Received identity: " << identity << std::endl;
            std::cerr << "Received content: " << content << std::endl;
            std::string_view id_view = identity.to_string_view();

            if (not is_human_readable(id_view)) {
                reply_to(std::move(identity), R"({"error": "Identity is expected to be a string/UTF-8"})");
                continue;
            }

            std::regex pattern(R"(client-\d+)");
            if (not std::regex_match(id_view.cbegin(), id_view.cend(), pattern)) {
                reply_to(std::move(identity), R"({"error": "Identity is expected to have a format like `client-<number>`"})");
                continue;
            }

            process_request(id_view, content.to_string_view());
        }
    }
}
