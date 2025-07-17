#include "cmd.hpp"

#include <mbooking/movie_booking.h>

#include <iostream>
#include <memory>
#include <format>

import mbooking;

static std::list<std::shared_ptr<movie_booking::IFutureWrapper>> g_futures;

static std::mutex g_mutex;

static std::jthread reply_thread;

namespace movie_booking {
    static mbooking::SyncedService service;
    std::unique_ptr<API> g_API;

    void execute_command(const ID &id, const std::string& command_name, const nlohmann::json& args)
    {
        if (command_name == "getPlayingMovies") {
            std::cerr << "command = 'getPlayingMovies' => pushing to futures list" << std::endl;
            g_futures.push_back(g_API->getPlayingMovies(id));
            std::cerr << "... pushed" << std::endl;
        }
        else if (command_name == "getTheaterNamesForMovie") {
            //std::cout << "args --------- " << args << std::endl;
            std::vector<std::string> vec = args;
            if (vec.size() == 1) {
                std::string movie = vec[0];

                const std::lock_guard<std::mutex> lock(g_mutex);
                g_futures.push_back(g_API->getTheaterNamesForMovie(id, movie));
            }
        }
        else if (command_name == "getAvailableSeats") {
            //std::cout << "args --------- " << args << std::endl;
            std::vector<std::string> vec = args;
            if (vec.size() == 2) {
                std::string movie = vec[0];
                std::string theater = vec[1];

                const std::lock_guard<std::mutex> lock(g_mutex);
                g_futures.push_back(g_API->getAvailableSeats(id, movie, theater));
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
            g_futures.push_back(g_API->bookSeats(id, client, movie, theater, seats));
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

    static void processResult(const IFutureWrapper::Result& resultPair, std::function<void(std::string_view, std::string_view)> process_reply)
    {
        const auto &[id, variantResult] = resultPair;

        std::visit([&id, process_reply](auto&& vecVal) {
            using Result = std::decay<decltype(vecVal)>;

            nlohmann::json json = { { "request_id", id.request }, { "response", vecVal } };

            process_reply(id.client, json.dump());

            }, variantResult);
    }

    static void reply_thread_callback(std::stop_token stoken, std::function<void(std::string_view, std::string_view)> process_reply)
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
                auto future = *it;

                std::cerr << "future status check..." << std::endl;
                std::future_status status = future->check();
                std::cerr << "... status: " << future_status_name(status) << std::endl;

                if (status == std::future_status::ready) {
                    processResult(future->result(), process_reply);

                    it = g_futures.erase(it);
                    std::cerr << "... removed from queue" << std::endl;
                }
                else {
                    ++it;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        }

        std::cerr << "Stop requested => response thread quitting!" << std::endl;
    }

    void start_reply_thread(std::function<void(std::string_view, std::string_view)> process_reply)
    {
        std::cerr << "Starting response thread..." << std::endl;
        reply_thread = std::move(std::jthread(reply_thread_callback, process_reply));
    }

    void create_service(std::function<void(std::string_view, std::string_view)> process_reply)
    {
        g_API = std::make_unique<API>(service);

        start_reply_thread(process_reply);
        start_workers();
    }

} // namespace movie_booking