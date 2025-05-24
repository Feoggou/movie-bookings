#include "cmd_parse.hpp"

#include <iostream>
#include <memory>

#include "mb_service.hpp"
#include <mbooking/movie_booking.h>

static std::list<std::shared_ptr<movie_booking::IFutureWrapper>> g_futures;

static std::mutex g_mutex;

namespace movie_booking {
    static SyncedService service;
    std::unique_ptr<API> g_API;

    void execute_command(const std::string& command_name, const nlohmann::json& args)
    {
        if (command_name == "getPlayingMovies") {
            g_futures.push_back(g_API->getPlayingMovies());
        }
        else if (command_name == "getTheaterNamesForMovie") {
            //std::cout << "args --------- " << args << std::endl;
            std::vector<std::string> vec = args;
            if (vec.size() == 1) {
                std::string movie = vec[0];

                const std::lock_guard<std::mutex> lock(g_mutex);
                g_futures.push_back(g_API->getTheaterNamesForMovie(movie));
            }
        }
        else if (command_name == "getAvailableSeats") {
            //std::cout << "args --------- " << args << std::endl;
            std::vector<std::string> vec = args;
            if (vec.size() == 2) {
                std::string movie = vec[0];
                std::string theater = vec[1];

                const std::lock_guard<std::mutex> lock(g_mutex);
                g_futures.push_back(g_API->getAvailableSeats(movie, theater));
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
            g_futures.push_back(g_API->bookSeats(client, movie, theater, seats));
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

    static void reply_thread_callback(std::stop_token stoken)
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

    void start_reply_thread()
    {
        std::jthread reply_thread(reply_thread_callback);
    }

    void create_service()
    {
        g_API = std::make_unique<API>(service);
    }

} // namespace movie_booking