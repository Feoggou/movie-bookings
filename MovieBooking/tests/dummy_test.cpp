#include <gtest/gtest.h>

#include "mbooking/movie_booking.h"


TEST(HelloTest, BasicAssertions)
{
	EXPECT_STRNE("hello", "world");
	EXPECT_EQ(7 * 6, 42);
}

TEST(MovieBooking, canGetEmptyListOfMovies)
{
	auto backend = std::make_unique<BookingBackend>();
	backend->movies = { };

	MovieBookingService service(std::move(backend));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, canGetOneMovie)
{
	auto backend = std::make_unique<BookingBackend>();
	backend->movies = {"The Movie"};

	MovieBookingService service(std::move(backend));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 1);
	ASSERT_EQ(movies.at(0), "The Movie");
}