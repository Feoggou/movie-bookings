#include "mb_service.hpp"

#include "utils.hpp"

#include <iostream>
#include <ranges>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>

namespace movie_booking {
	Service::Service()
		: m_store(std::make_unique<Store>())
	{

	}

	Service::Service(std::unique_ptr<Store>&& store)
		: m_store(std::move(store))
	{

	}

	std::vector<std::string> Service::getPlayingMovies() const
	{	
		auto view = m_store->theatersByMovie
			| std::views::filter([](const auto& pair) {
					return !pair.second.empty()
							&& std::ranges::any_of(pair.second, [](const Theater& t) { return !t.name.empty(); });
				})
			| std::views::transform([](const auto& pair) -> const auto& { return pair.first; });

		return { view.begin(), view.end() };
	}

	std::vector<std::string> Service::getTheaterNamesForMovie(std::string_view movie) const
	{
		auto view = m_store->theatersByMovie[std::string(movie)]
			| std::views::transform([](const Theater& t) { return t.name; })
			| std::views::filter([](const std::string& theaterName) { return !theaterName.empty(); });

		return { view.begin(), view.end() };
	}

	std::vector<size_t> Service::getAvailableSeats(std::string_view movie, std::string_view theater) const
	{
		const auto &theaters = m_store->theatersByMovie[std::string(movie)];
		auto found_theater = utils::find_if_optional(theaters, [=](const Theater& t) { return t.name == theater; });
		if (!found_theater)
			return {};

		auto indices = std::views::iota(0u, found_theater->seats.size())
			| std::views::filter([&](std::size_t i) { return found_theater->seats[i].empty(); });

		std::vector<std::size_t> result;
		std::ranges::copy(indices, std::back_inserter(result));

		return result;
	}

	bool Service::_bookOneSeat(std::string_view client, std::vector<std::string> &all_seats, size_t seat)
	{
		if (seat >= all_seats.size())
			return false;

		if (!all_seats[seat].empty())
			return false;

		all_seats[seat] = client;

		return true;
	}

	std::vector<size_t> Service::bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t> &seats)
	{
		auto& theaters = m_store->theatersByMovie[std::string(movie)];
		auto theater_it = std::ranges::find_if(theaters, [=](const Theater& t) { return t.name == theater; });
		if (theater_it == std::ranges::end(theaters))
			return {};

		std::vector<size_t> booked;

		for (const size_t seat : seats) {
			if (_bookOneSeat(client, theater_it->seats, seat))
				booked.push_back(seat);
		}

		return booked;
	}

	std::vector<size_t> SyncedService::getAvailableSeats(std::string_view movie, std::string_view theater) const
	{
		std::shared_lock lock(m_rw_mutex);

		return Service::getAvailableSeats(movie, theater);
	}

	std::vector<size_t> SyncedService::bookSeats(std::string_view client, std::string_view movie, std::string_view theater, const std::vector<size_t>& seats)
	{
		std::unique_lock lock(m_rw_mutex);

		return Service::bookSeats(client, movie, theater, seats);
	}

} // namespace movie_booking