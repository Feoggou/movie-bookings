#include "mb_service.hpp"

#include "utils.hpp"

#include <iostream>
#include <ranges>
#include <string>
#include <vector>
#include <algorithm>

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
			| std::views::filter([&](std::size_t i) { return found_theater->seats[i] == true; });

		std::vector<std::size_t> result;
		std::ranges::copy(indices, std::back_inserter(result));

		return result;
	}

} // namespace movie_booking