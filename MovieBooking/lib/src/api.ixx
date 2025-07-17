module;

/**
 * @file api.ixx
 * @brief Not really an API. Refactoring is in progres...
 */

#include <functional>
#include <vector>
#include <string>
#include <memory>
#include <future>
#include <chrono>
#include <variant>

module mbooking:api;
import :service;

namespace mbooking
{
    struct ID
    {
        std::string client;
        std::string request;
    };

    //class Service;

    class IFutureWrapper
    {
    public:
        using Variant = std::variant<std::vector<size_t>, std::vector<std::string>>;
        using Result = std::pair<ID, Variant>;

        virtual ~IFutureWrapper() = default;
        virtual std::future_status check() const = 0;
        virtual Result result() = 0;
    };

    template <typename T>
    class FutureWrapper : public IFutureWrapper {
    public:
        explicit FutureWrapper(std::future<T> f) : fut(std::move(f)) {}
        std::future_status check() const override { return fut.wait_for(std::chrono::milliseconds(0)); }
        std::future<T>& get_future() { return fut; }
        Result result() override { return  fut.get(); }

    private:
        std::future<T> fut;
    };

    /**
 * @class API
 * @brief A short summary of what API does.
 *
 * A more detailed description of MyClass,
 * potentially spanning multiple lines.
 *
 */
    class API
    {
    public:
        using Variant = std::variant<std::vector<size_t>, std::vector<std::string>>;
        using Result = std::pair<ID, Variant>;
        using Future = FutureWrapper<Result>;
        using SharedFuture = std::shared_ptr<Future>;

        explicit API(Service& service) : m_service(service) {}
        /**
         * @brief Get a list of currently playing movies.
         *
         * This function returns a vector of strings representing the titles of
         * movies that are currently playing.
         *
         * @return A vector of strings containing the titles of currently playing movies.
         */
        SharedFuture getPlayingMovies(const ID& id) const;

        /**
         * @brief Get a list of theaters for the currently playing movie
         *
         * @param movie The movie for which to see available theaters
         *
         * This function returns a vector of strings representing the titles of
         * movies that are currently playing.
         *
         * @return A vector of strings containing the titles of currently playing movies.
         */
        SharedFuture getTheaterNamesForMovie(const ID& id, std::string_view movie) const;

        SharedFuture getAvailableSeats(const ID& id, std::string_view movie, std::string_view theater) const;

        SharedFuture bookSeats(const ID& id, std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats);

    private:
        Service& m_service;
    };
}
