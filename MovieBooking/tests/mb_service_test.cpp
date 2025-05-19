#include "src/mb_service.hpp"

#include <gtest/gtest.h>

#include <ranges>

using namespace movie_booking;

namespace {


	inline std::vector<Theater> makeTheaters(const std::vector<std::string>& theaters)
	{
		auto view = theaters
			| std::views::transform([](const std::string& s) -> Theater { return Theater{ .name = s }; });

		std::vector<Theater> result(view.begin(), view.end());

		return result;
	}

	inline std::vector<Theater> makeNoTheaters()
	{
		return {};
	}

	inline std::vector<Theater> makeTheatersOfOne(const std::string& theater)
	{
		return { Theater{ .name = theater } };
	}

	inline Theater makeTheaterWithSeats(const std::string &theater, const std::vector<bool>& seats)
	{
		return {};
	}

	inline std::pair<std::string, std::vector<Theater>> makeMovie(const std::string& name, const std::vector<Theater>& theaters)
	{
		return std::make_pair(name, theaters);
	}

	inline std::unique_ptr<Store> makeStore(const std::map<std::string, std::vector<Theater>> &theatersByMovie)
	{
		auto store = std::make_unique<Store>();
		store->theatersByMovie = theatersByMovie;

		return store;
	}

	inline Service makeServiceWithMovies(const std::map<std::string, std::vector<Theater>>& theatersByMovie)
	{
		return Service(makeStore(theatersByMovie));
	}
}



// ************************ TESTS: MOVIES ************************


TEST(MovieBooking, willGetEmptyListOfMoviesForNoMovies)
{
	Service service = makeServiceWithMovies({});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, wontGetAMovieIfNotInTheater)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeNoTheaters()),
		});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, wontGetAMovieIfNotTheaterIsEmpty)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheatersOfOne("")),
		});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 0);
}

TEST(MovieBooking, canGetOneMovieInTheater)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheatersOfOne("The Theater")),
		});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 1);
	ASSERT_EQ(movies.at(0), "The Movie");
}

TEST(MovieBooking, canGetTwoMoviesInTheSameTheater)
{
	Service service = makeServiceWithMovies({
		makeMovie("Movie A", makeTheatersOfOne("The Theater")),
		makeMovie("Movie B", makeTheatersOfOne("The Theater")),
		});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 2);
	ASSERT_EQ(movies.at(0), "Movie A");
	ASSERT_EQ(movies.at(1), "Movie B");
}

TEST(MovieBooking, canGetTwoMoviesInDifferentTheaters)
{
	Service service = makeServiceWithMovies({
		makeMovie("Movie A", makeTheatersOfOne("Theater A")),
		makeMovie("Movie B", makeTheatersOfOne("Theater B")),
		});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 2);
	ASSERT_EQ(movies.at(0), "Movie A");
	ASSERT_EQ(movies.at(1), "Movie B");
}

TEST(MovieBooking, canGetAMovieInMoreThanOneTheaters)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheaters({"Theater A", "Theater B" })),
		});

	std::vector<std::string> movies = service.getPlayingMovies();

	ASSERT_EQ(movies.size(), 1);
	ASSERT_EQ(movies.at(0), "The Movie");
}

// ************************ TESTS: THEATERS ************************

TEST(MovieBooking, willFindNoTheaterForNoMovie)
{
	Service service = makeServiceWithMovies({
		makeMovie("A movie", makeTheatersOfOne("A Theater")),
		});

	std::vector<std::string> theaters = service.getTheaterNamesForMovie("");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenThereaAreNoTheatersWillFindNoTheater)
{
	Service service = makeServiceWithMovies({
		makeMovie("My movie", makeNoTheaters()),
		});

	std::vector<std::string> theaters = service.getTheaterNamesForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenTheaterNameIsEmptyWillFindNoTheater)
{
	Service service = makeServiceWithMovies({
		makeMovie("My movie", makeTheatersOfOne("")),
		});

	std::vector<std::string> theaters = service.getTheaterNamesForMovie("My movie");

	ASSERT_EQ(theaters.size(), 0);
}

TEST(MovieBooking, whenOneTheaterPlaysMovieReturnOneTheater)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheatersOfOne("The Theater")),
		});

	std::vector<std::string> theaters = service.getTheaterNamesForMovie("The Movie");

	ASSERT_EQ(theaters.size(), 1);
	ASSERT_EQ(theaters.at(0), "The Theater");
}

TEST(MovieBooking, whenTwoTheatersPlayMovieReturnTheaters)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheaters({ "Theater A", "Theater B",})),
		});

	std::vector<std::string> theaters = service.getTheaterNamesForMovie("The Movie");

	ASSERT_EQ(theaters.size(), 2);
	ASSERT_EQ(theaters.at(0), "Theater A");
	ASSERT_EQ(theaters.at(1), "Theater B");
}

TEST(MovieBooking, whenTwoTheatersPlayDifferentMoviesReturnExpectedTheaters)
{
	Service service = makeServiceWithMovies({
		makeMovie("Movie A", makeTheatersOfOne("Theater A")),
		makeMovie("Movie B", makeTheatersOfOne("Theater B")),
		});

	std::vector<std::string> theaters = service.getTheaterNamesForMovie("Movie A");
	EXPECT_EQ(theaters.size(), 1);
	EXPECT_EQ(theaters.at(0), "Theater A");

	theaters = service.getTheaterNamesForMovie("Movie B");
	EXPECT_EQ(theaters.size(), 1);
	EXPECT_EQ(theaters.at(0), "Theater B");
}

// ************************ TESTS: CHECK SEATS ************************

TEST(MovieBooking, whenMovieIsEmptyReturnNoAvailableSeats)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheatersOfOne("The Theater")),
		});

	std::vector<std::string> seats = service.getAvailableSeats("", "The Theater");
	
	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenTheaterIsEmptyReturnNoAvailableSeats)
{
	Service service = makeServiceWithMovies({
		makeMovie("The Movie", makeTheatersOfOne("The Theater")),
		});

	std::vector<std::string> seats = service.getAvailableSeats("The Movie", "");

	ASSERT_EQ(seats.size(), 0);
}
