// MovieBooking.cpp : Defines the entry point for the application.
//

#include "src/mb_service.hpp"
#include "src/workers.hpp"
#include "src/zeromq_sync.hpp"

#include <mbooking/movie_booking.h>

#include <nlohmann/json.hpp>

#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>

#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <forward_list>
#include <format>


using namespace std;

static std::list<std::shared_ptr<movie_booking::IFutureWrapper>> g_futures;

static std::mutex g_mutex;

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

            const std::lock_guard<std::mutex> lock(g_mutex);
            g_futures.push_back(api.getTheaterNamesForMovie(movie));
        }
    }
    else if (command_name == "getAvailableSeats") {
        //std::cout << "args --------- " << args << std::endl;
        std::vector<std::string> vec = args;
        if (vec.size() == 2) {
            std::string movie = vec[0];
            std::string theater = vec[1];

            const std::lock_guard<std::mutex> lock(g_mutex);
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

        const std::lock_guard<std::mutex> lock(g_mutex);
        g_futures.push_back(api.bookSeats(client, movie, theater, seats));
    }
}

inline const char* future_status_name(std::future_status status)
{
    switch (status)
    {
    case std::future_status::ready: return "READY";
    case std::future_status::deferred: return "DEFERRED";
    case std::future_status::timeout: return "TIMEOUT";
    default: return "UNKNOWN";
    }
}

void reply_thread_callback(std::stop_token stoken)
{
    std::cerr << "reply_thread_callback started..." << std::endl;

    while (!stoken.stop_requested())
    {
        const std::lock_guard<std::mutex> lock(g_mutex);

        if (g_futures.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::cerr << "Found futures: " << g_futures.size() << std::endl;
        for (auto it = g_futures.begin(); it != g_futures.end();) {
            //auto future = dynamic_cast<movie_booking::FutureWrapper<int>&>(*(*it));
            auto future = *it;

            std::cerr << "future status check..." << std::endl;
            std::future_status status = future->check();
            std::cerr << "... status: " << future_status_name(status) << std::endl;

            if (status == std::future_status::ready) {
                auto variantResult = future->result();
                std::visit([](auto&& vecVal) {
                    using Result = std::decay<decltype(vecVal)>;
                    std::cerr << "RESULT: [" << vecVal.size() << "] ";
                    for (const auto& x : vecVal) {
                        std::cerr << x << ", ";
                    }
                    std::cerr << "]" << std::endl;
                    }, variantResult);
                
                it = g_futures.erase(it);
                std::cerr << "... removed from queue" << std::endl;
            }
            else {
                ++it;
            }
        }
    }
}

int main()
{
    start_workers();

    movie_booking::SyncedService service;
    movie_booking::API api(service);

	std::cerr << "Starting..." << std::endl;

    std::jthread reply_thread(reply_thread_callback);

    run_zero_mq_server([&api](std::string_view received_msg) -> std::string {
        nlohmann::json json = nlohmann::json::parse(received_msg);
        if (json.contains("pid"))
        {
            std::cerr << "[" << json["pid"] << "] " << json["cmd"] << std::endl;

            nlohmann::json args;

            if (json.contains("args")) {
                args = json["args"];
            }
            /*json = */execute_command(api, json["cmd"], args);
            //std::cerr << "JSON result is: " << json << std::endl;

            json = { };
            return json.dump();
        }
        return {};
    });

	return 0;
}
