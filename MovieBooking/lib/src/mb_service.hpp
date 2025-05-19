/**
 * @file movie_booking.h
 * @brief Utility functions for checking values.
 */

#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <map>
#include <span>
#include <shared_mutex>

namespace movie_booking
{
    struct Theater
    {
        std::string name;
        std::vector<std::string> seats;
    };

    struct Store
    {
    public:
        // TODO: the store should use a separate std::vector<Theater>, and only create this map when needed
        std::map<std::string, std::vector<Theater>> theatersByMovie = {
            {"Movie A", { Theater{.name = "Theater 1"}, Theater{.name = "Theater 2", .seats = {"", "joe", "joe", "" }}}},
            {"Movie B", { Theater{.name="Theater 2", .seats = {"john", "", "joe", "" }} } },
            {"Movie C", { Theater{.name="Theater 1", .seats = {"", "joe", "joe", "" }}, Theater{.name="Theater 3", .seats = {"", "", "joe", "joe", "" }}}},
            {"Movie D", { } },
        };
    };


    /**
     * @brief MovieBookingService class
     *
     * This class provides a service for booking movies.
     */
    class Service
    {
    public:
        Service();
        virtual ~Service() = default;

        explicit Service(std::unique_ptr<Store>&& store);

        /**
         * @brief Get a list of currently playing movies.
         *
         * This function returns a vector of strings representing the titles of
         * movies that are currently playing.
         *
         * @return A vector of strings containing the titles of currently playing movies.
         */
        std::vector<std::string> getPlayingMovies() const;

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
        std::vector<std::string> getTheaterNamesForMovie(std::string_view movie) const;

        virtual std::vector<size_t> getAvailableSeats(std::string_view movie, std::string_view theater) const;

        virtual std::vector<size_t> bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats);

    private:
        bool _bookOneSeat(std::string_view client, std::vector<std::string>& all_seats, size_t seat);

    private:
        std::unique_ptr<Store> m_store;
    };

    class SyncedService : public Service
    {
    public:
        std::vector<size_t> getAvailableSeats(std::string_view movie, std::string_view theater) const override;
        std::vector<size_t> bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats) override;

    private:
        mutable std::shared_mutex m_rw_mutex;
    };

} // namespace movie_booking