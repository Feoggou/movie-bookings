#include <gtest/gtest.h>

#include "src/mb_service.hpp"

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

TEST(MovieBooking, canGetTwoMovies)
{
	auto store = std::make_unique<Store>();
	store->movies = { "Movie A", "Movie B" };

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 2);
	ASSERT_EQ(movies.at(0), "Movie A");
	ASSERT_EQ(movies.at(1), "Movie B");
}

TEST(MovieBooking, willFindNoTheaterForNoMovie)
{
	auto store = std::make_unique<Store>();
	store->movies = { };
	store->theaters = {};

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenThereaAreNoTheatersWillFindNoTheater)
{
	auto store = std::make_unique<Store>();
	store->movies = { };
	store->theaters = {};

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenMovieIsNotPlayedInTheatersWillFindNoTheater)
{
	auto store = std::make_unique<Store>();
	store->movies = { };
	store->theaters = { "Theater 1", "Theater 2" };

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenOneTheaterPlaysMovieReturnOneTheater)
{
	auto store = std::make_unique<Store>();
	store->movies = { "My movie" };
	store->theaters = { "Theater 1", "Theater 2" };
	store->theatersByMovie = {
		{"My movie", {"Theater 1", "Theater 2"}}
	};

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}
