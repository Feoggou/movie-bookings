/**
 * @file movie_booking.h
 * @brief Utility functions for checking values.
 */

#include <vector>
#include <string>
#include <memory>

struct BookingBackend
{
public:
    std::vector<std::string> movies = { "Movie A", "Movie B", "Movie C" };
};


/**
 * @brief MovieBookingService class
 *
 * This class provides a service for booking movies.
 */
class MovieBookingService
{
public:
    MovieBookingService();

    explicit MovieBookingService(std::unique_ptr<BookingBackend> &&backend);

    /**
     * @brief Get a list of currently playing movies.
     *
     * This function returns a vector of strings representing the titles of
     * movies that are currently playing.
     *
     * @return A vector of strings containing the titles of currently playing movies.
     */
    std::vector<std::string> getPlayingMovies() const;

private:
    std::unique_ptr<BookingBackend> m_backend;
};
