#include <gtest/gtest.h>

#include "src/mb_service.hpp"

using namespace movie_booking;

namespace {

	inline std::vector<std::string> makeTheaters(const std::vector<std::string>& theaters)
	{
		return theaters;
	}

	inline std::vector<std::string> makeNoTheaters()
	{
		return {};
	}

	inline std::vector<std::string> makeTheatersOfOne(const std::string& theater)
	{
		return { theater };
	}

	inline std::pair<std::string, std::vector<std::string>> makeMovie(const std::string& name, const std::vector<std::string>& theaters)
	{
		return std::make_pair(std::string(name), theaters);
	}

	inline std::unique_ptr<Store> makeStore(const std::map<std::string, std::vector<std::string>> &theatersByMovie)
	{
		auto store = std::make_unique<Store>();
		store->theatersByMovie = theatersByMovie;

		return store;
	}
}



// ************************ TESTS: MOVIES ************************


TEST(MovieBooking, canGetEmptyListOfMovies)
{
	auto store = makeStore({ });


	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, wontGetAMovieIfNotInTheater)
{
	auto store = makeStore({
		makeMovie("The Movie", makeNoTheaters()),
		});

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, wontGetAMovieIfNotTheaterIsEmpty)
{
	auto store = makeStore({
		makeMovie("The Movie", makeTheatersOfOne("")),
		});

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, canGetOneMovieInTheater)
{
	auto store = makeStore({
		makeMovie("The Movie", makeTheatersOfOne("The Theater")),
		});

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 1);
	ASSERT_EQ(movies.at(0), "The Movie");
}

TEST(MovieBooking, canGetTwoMoviesInTheSameTheater)
{
	auto store = makeStore({
		makeMovie("Movie A", makeTheatersOfOne("The Theater")),
		makeMovie("Movie B", makeTheatersOfOne("The Theater")),
		});

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 2);
	ASSERT_EQ(movies.at(0), "Movie A");
	ASSERT_EQ(movies.at(1), "Movie B");
}

TEST(MovieBooking, canGetTwoMoviesInDifferentTheaters)
{
	auto store = makeStore({
		makeMovie("Movie A", makeTheatersOfOne("Theater A")),
		makeMovie("Movie B", makeTheatersOfOne("Theater B")),
		});

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 2);
	ASSERT_EQ(movies.at(0), "Movie A");
	ASSERT_EQ(movies.at(1), "Movie B");
}

TEST(MovieBooking, canGetAMovieInMoreThanOneTheaters)
{
	auto store = makeStore({
		makeMovie("The Movie", makeTheaters({"Theater A", "Theater B" })),
		});

	Service service(std::move(store));
	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 1);
	ASSERT_EQ(movies.at(0), "The Movie");
}

// ************************ TESTS: THEATERS ************************

TEST(MovieBooking, willFindNoTheaterForNoMovie)
{
	auto store = makeStore({
		makeMovie("A movie", makeTheatersOfOne("A Theater")),
		});

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenThereaAreNoTheatersWillFindNoTheater)
{
	auto store = makeStore({
		makeMovie("My movie", makeNoTheaters()),
		});

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenTheaterNameIsEmptyWillFindNoTheater)
{
	auto store = makeStore({
		makeMovie("My movie", makeTheatersOfOne("")),
		});

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenOneTheaterPlaysMovieReturnOneTheater)
{
	auto store = makeStore({
		makeMovie("The Movie", makeTheatersOfOne("The Theater")),
		});

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("The Movie");

	ASSERT_EQ(theaters.size(), 1);
	ASSERT_EQ(theaters.at(0), "The Theater");
}

TEST(MovieBooking, whenTwoTheatersPlayMovieReturnTheaters)
{
	auto store = makeStore({
		makeMovie("The Movie", makeTheaters({ "Theater A", "Theater B",})),
		});

	Service service(std::move(store));
	std::vector<std::string> theaters = service.getTheatersForMovie("The Movie");

	ASSERT_EQ(theaters.size(), 2);
	ASSERT_EQ(theaters.at(0), "Theater A");
	ASSERT_EQ(theaters.at(1), "Theater B");
}

TEST(MovieBooking, whenTwoTheatersPlayDifferentMoviesReturnExpectedTheaters)
{
	auto store = makeStore({
		makeMovie("Movie A", makeTheatersOfOne("Theater A")),
		makeMovie("Movie B", makeTheatersOfOne("Theater B")),
		});

	Service service(std::move(store));

	std::vector<std::string> theaters = service.getTheatersForMovie("Movie A");
	EXPECT_EQ(theaters.size(), 1);
	EXPECT_EQ(theaters.at(0), "Theater A");

	theaters = service.getTheatersForMovie("Movie B");
	EXPECT_EQ(theaters.size(), 1);
	EXPECT_EQ(theaters.at(0), "Theater B");
}