/**
 * @file movie_booking.h
 * @brief Utility functions for checking values.
 */

#include <vector>
#include <string>
#include <string_view>
#include <memory>

namespace movie_booking
{

    struct Store
    {
    public:
        std::vector<std::string> movies = { "Movie A", "Movie B", "Movie C" };
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
        std::vector<std::string> getTheatersForMovie(std::string_view movie) const;

    private:
        std::unique_ptr<Store> m_store;
    };

} // namespace movie_booking