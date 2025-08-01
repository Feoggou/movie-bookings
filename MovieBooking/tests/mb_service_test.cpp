#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <ranges>

import mbooking;

using namespace mbooking;

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

	inline Theater makeTheaterWithSeats(const std::string &theater, const std::vector<std::string>& seats)
	{
		return { Theater{.name = theater, .seats = seats } };
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

	inline Service makeServiceForTheaterWithSeats(const std::string &movie, const std::string &theater, const std::vector<std::string> &seats)
	{
		return Service(makeStore({
			makeMovie(movie, { makeTheaterWithSeats(theater, seats), })
			}));
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

TEST(MovieBooking, whenMovieGivenIsEmptyReturnNoAvailableSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "" });

	std::vector<size_t> seats = service.getAvailableSeats("", "The Theater");
	
	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenTheaterGivenIsEmptyThenReturnNoAvailableSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "" });

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "");

	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenTheaterHasEmptyListOfSeatsReturnNoAvailableSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", {});

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "The Theater");

	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenTheaterHasOneSeatAndNotAvailableReturnNoAvailableSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "john" });

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "The Theater");

	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenMovieIsNotInTheatersReturnNoAvailableSeats)
{
	Service service = makeServiceForTheaterWithSeats("Movie A", "The Theater", { "" });

	std::vector<size_t> seats = service.getAvailableSeats("Movie B", "The Theater");

	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenTheaterDoesntExistReturnNoAvailableSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "Theater A", { "" });

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "Theater B");

	ASSERT_EQ(seats.size(), 0);
}

TEST(MovieBooking, whenTheaterHasOneSeatAndAvailableReturnSeat)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "" });

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "The Theater");

	ASSERT_EQ(seats.size(), 1);
	ASSERT_EQ(seats.at(0), 0);
}

TEST(MovieBooking, whenTheaterHasTwoSeatsAndOneAvailableReturnAvailableSeat)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "john", ""});

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "The Theater");

	ASSERT_EQ(seats.size(), 1);
	ASSERT_EQ(seats.at(0), 1);
}

TEST(MovieBooking, whenTheaterHasMultipleSeatsAvailableReturnSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "", "joe", "", "mark", "", ""});

	std::vector<size_t> seats = service.getAvailableSeats("The Movie", "The Theater");

	ASSERT_EQ(seats.size(), 4);
	ASSERT_EQ(seats.at(0), 0);
	ASSERT_EQ(seats.at(1), 2);
	ASSERT_EQ(seats.at(2), 4);
	ASSERT_EQ(seats.at(3), 5);
}

// ************************ TESTS: BOOK SEATS ************************

TEST(MovieBooking, whenTheaterIsEmptyBookSeatFail)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { });

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 0 });

	ASSERT_EQ(booked.size(), 0);
}

TEST(MovieBooking, whenTheaterHasNoAvailableSeatsFail)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "joe" });

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 0 });

	ASSERT_EQ(booked.size(), 0);
}

TEST(MovieBooking, whenTryingToBookTheUnavailableSeatFail)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "", "john" });

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 1 });

	ASSERT_EQ(booked.size(), 0);
}

TEST(MovieBooking, whenTryingToBookTheOutOfBoundsSeatFail)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "", "joe" });

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 23 });

	ASSERT_EQ(booked.size(), 0);
}

TEST(MovieBooking, canBookAvailableSeatInTheaterOfOneSeat)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "" });

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 0 });

	ASSERT_EQ(booked.size(), 1);
	ASSERT_EQ(booked.at(0), 0);
}

TEST(MovieBooking, canBookOneAvailableSeatInTheaterOfManySeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "", "", "joe" , "", "mark"});

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 1 });

	ASSERT_EQ(booked.size(), 1);
	ASSERT_EQ(booked.at(0), 1);
}

TEST(MovieBooking, bookingAvailableSeatMakesItUnavailable)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "", "", "joe", "", "mark"});

	std::vector<size_t> seats_before = service.getAvailableSeats("The Movie", "The Theater");
	ASSERT_EQ(seats_before.size(), 3);
	ASSERT_EQ(seats_before.at(0), 0);
	ASSERT_EQ(seats_before.at(1), 1);
	ASSERT_EQ(seats_before.at(2), 3);

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 1 });
	ASSERT_EQ(booked.size(), 1);
	ASSERT_EQ(booked.at(0), 1);

	std::vector<size_t> seats_after = service.getAvailableSeats("The Movie", "The Theater");

	ASSERT_EQ(seats_after.size(), 2);
	ASSERT_EQ(seats_after.at(0), 0);
	ASSERT_EQ(seats_after.at(1), 3);
}

TEST(MovieBooking, bookingTwoSeatsAllSucceed)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "joe", "", "joe", "", "joe"});

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 1, 3 });
	
	ASSERT_EQ(booked.size(), 2);
	ASSERT_EQ(booked.at(0), 1);
	ASSERT_EQ(booked.at(1), 3);
}

TEST(MovieBooking, bookingTwoSeatsAllFail)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "joe", "", "mark", "", "joe"});

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 0, 2 });

	ASSERT_EQ(booked.size(), 0);
}

TEST(MovieBooking, bookingManySeatsSomeFail)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "joe", "", "joe", "", "joe"});

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 0, 1, 3, 15 });

	EXPECT_THAT(booked, ::testing::ElementsAreArray({ 1, 3 }));
}

TEST(MovieBooking, bookingDuplicateSeats)
{
	Service service = makeServiceForTheaterWithSeats("The Movie", "The Theater", { "joe", "", "joe", "", "joe"});

	std::vector<size_t> booked = service.bookSeats("john", "The Movie", "The Theater", { 0, 1, 3, 1, 1 });

	EXPECT_THAT(booked, ::testing::ElementsAreArray({ 1, 3 }));
}
