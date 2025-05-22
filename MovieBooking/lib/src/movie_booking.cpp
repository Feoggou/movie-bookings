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
    template <typename R, typename Func>
    static std::shared_ptr<std::future<R>> runAsync(Func func)
    {
        auto promise = std::make_shared<std::promise<R>>();
        std::future<R> future = promise->get_future();

        request_command([promise, func]() {
            R result = func();
            promise->set_value(result);
            });

        return std::make_shared<std::future<R>>(std::move(future));
    }

    std::shared_ptr<std::future<std::vector<std::string>>> API::getPlayingMovies() const
    {
        return runAsync<std::vector<std::string>>([this]() { return m_service.getPlayingMovies(); });
    }

    std::shared_ptr<std::future<std::vector<std::string>>> API::getTheaterNamesForMovie(std::string_view movie) const
    {
        return runAsync<std::vector<std::string>>([this, m=std::string(movie)]() {
            return m_service.getTheaterNamesForMovie(m);
            });
    }

    std::shared_ptr<std::future<std::vector<size_t>>> API::getAvailableSeats(std::string_view movie, std::string_view theater) const
    {
        return runAsync<std::vector<size_t>>([this, m=std::string(movie), t=std::string(theater)]() {
            return m_service.getAvailableSeats(m, t);
            });
    }

    std::shared_ptr<std::future<std::vector<size_t>>> API::bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats)
    {
        return runAsync<std::vector<size_t>>([this, c=std::string(client), m=std::string(movie), t=std::string(theater), s=std::vector(seats)]() {
            return m_service.getAvailableSeats(m, t);
            });
    }
} // namespace movie_booking