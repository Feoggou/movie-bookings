/**
 * @file movie_booking.h
 * @brief Utility functions for checking values.
 */

#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <map>

namespace movie_booking
{
    struct Theater
    {
        std::string name;
        std::vector<bool> seats;
    };

    struct Store
    {
    public:
        std::map<std::string, std::vector<Theater>> theatersByMovie = {
            {"Movie A", { Theater{.name = "Theater 1"}, Theater{.name = "Theater 2"} } },
            {"Movie B", { Theater{.name="Theater 2"} } },
            {"Movie C", { Theater{.name="Theater 1"}, Theater{.name="Theater 3"}}},
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

        std::vector<size_t> getAvailableSeats(std::string_view movie, std::string_view theater) const;

    private:
        std::unique_ptr<Store> m_store;
    };

} // namespace movie_booking