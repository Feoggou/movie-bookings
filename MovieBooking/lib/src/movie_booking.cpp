#include "movie_booking.hpp"
#include "workers.hpp"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>
#include <memory>


namespace movie_booking {

    std::vector<std::string> API::getPlayingMovies() const
    {
        auto promise = std::make_shared<std::promise<std::vector<std::string>>>();
        std::future<std::vector<std::string>> future = promise->get_future();

        request_command([this, promise]() {
            std::vector<std::string> result = m_service.getPlayingMovies();
            promise->set_value(result);
            });

        std::vector<std::string> result = future.get();

        return result;
    }

    std::vector<std::string> API::getTheaterNamesForMovie(std::string_view movie) const
    {
        auto promise = std::make_shared<std::promise<std::vector<std::string>>>();
        std::future<std::vector<std::string>> future = promise->get_future();

        request_command([this, promise, m = std::string(movie)]() {
            std::vector<std::string> result = m_service.getTheaterNamesForMovie(m);
            promise->set_value(result);
            });

        std::vector<std::string> result = future.get();

        std::cout << "#### RESULT: vector size: " << result.size() << " [";
        for (const auto& s : result) {
            std::cout << s << ", ";
        }
        std::cout << "]" << std::endl;

        return result;
    }

    std::vector<size_t> API::getAvailableSeats(std::string_view movie, std::string_view theater) const
    {
        auto promise = std::make_shared<std::promise<std::vector<size_t>>>();
        std::future<std::vector<size_t>> future = promise->get_future();

        request_command([this, promise, m = std::string(movie), t = std::string(theater)]() {
            std::vector<size_t> result = m_service.getAvailableSeats(m, t);
            promise->set_value(result);
            });

        std::vector<size_t> result = future.get();

        std::cout << "#### SEATS AVAILABLE: vector size: " << result.size() << " [";
        for (const auto& s : result) {
            std::cout << s << ", ";
        }
        std::cout << "]" << std::endl;

        return result;
    }

    std::vector<size_t> API::bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats)
    {
        auto promise = std::make_shared<std::promise<std::vector<size_t>>>();
        std::future<std::vector<size_t>> future = promise->get_future();

        request_command([this, promise, c = std::string(client), m = std::string(movie), t = std::string(theater), s = std::vector(seats)]() {
            std::vector<size_t> result = m_service.bookSeats(c, m, t, s);
            promise->set_value(result);
            });

        std::vector<size_t> result = future.get();

        std::cout << "#### BOOKED: vector size: " << result.size() << " [";
        for (const auto& s : result) {
            std::cout << s << ", ";
        }
        std::cout << "]" << std::endl;

        return result;
    }
} // namespace movie_booking