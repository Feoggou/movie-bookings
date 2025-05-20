/**
 * @file movie_booking.h
 * @brief The main API to be used by clients of the library
 */

#include <functional>
#include <vector>
#include <string>

namespace movie_booking
{
    class Service;

    class API
    {
    public:
        explicit API(Service& service) : m_service(service) {}
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

        std::vector<size_t> bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats);

    private:
        Service& m_service;
    };
}
