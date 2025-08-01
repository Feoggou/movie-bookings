#include "movie_booking.hpp"

#include "cmd.hpp"

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>
#include <memory>

import mbooking;


namespace movie_booking {
    template <typename Func>
    static API::SharedFuture runAsync(const ID &id, Func func)
    {
        auto promise = std::make_shared<std::promise<API::Result>>();
        std::future<API::Result> future = promise->get_future();

        request_command([promise, func, req_id=id]() {
            API::Result result = std::make_pair(req_id, func());
            promise->set_value(result);
            });

        return std::make_shared<API::Future>(std::move(future));
    }

    API::SharedFuture API::getPlayingMovies(const ID& id) const
    {
        return runAsync(id, [this]() { return m_service.getPlayingMovies(); });
    }

    API::SharedFuture API::getTheaterNamesForMovie(const ID& id, std::string_view movie) const
    {
        return runAsync(id, [this, m=std::string(movie)]() {
            return m_service.getTheaterNamesForMovie(m);
            });
    }

    API::SharedFuture API::getAvailableSeats(const ID& id, std::string_view movie, std::string_view theater) const
    {
        return runAsync(id, [this, m=std::string(movie), t=std::string(theater)]() {
            return m_service.getAvailableSeats(m, t);
            });
    }

    API::SharedFuture API::bookSeats(const ID& id, std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats)
    {
        return runAsync(id, [this, c=std::string(client), m=std::string(movie), t=std::string(theater), s=std::vector(seats)]() {
            return m_service.bookSeats(c, m, t, s);
            });
    }
} // namespace movie_booking