#include "mb_service.hpp"

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

		std::vector<std::string> result(view.begin(), view.end());
		return result;
	}

	std::vector<std::string> Service::getTheaterNamesForMovie(std::string_view movie) const
	{
		auto view = m_store->theatersByMovie[std::string(movie)]
			| std::views::transform([](const Theater& t) { return t.name; })
			| std::views::filter([](const std::string& theaterName) { return !theaterName.empty(); });

		std::vector<std::string> result(view.begin(), view.end());
		return result;
	}

	std::vector<std::string> Service::getAvailableSeats(std::string_view movie, std::string_view theater) const
	{
		return {};
	}

} // namespace movie_booking