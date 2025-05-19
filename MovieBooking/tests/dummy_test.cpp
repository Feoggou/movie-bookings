#include <gtest/gtest.h>

#include "mbooking/movie_booking.h"

using namespace movie_booking;


TEST(HelloTest, BasicAssertions)
{
	EXPECT_STRNE("hello", "world");
	EXPECT_EQ(7 * 6, 42);
}

TEST(MovieBooking, canGetEmptyListOfMovies)
{
	auto store = std::make_unique<Store>();
	store->movies = { };

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, canGetOneMovie)
{
	auto store = std::make_unique<Store>();
	store->movies = {"The Movie"};

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 1);
	ASSERT_EQ(movies.at(0), "The Movie");
}