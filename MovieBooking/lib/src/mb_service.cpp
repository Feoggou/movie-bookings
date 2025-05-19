#include <iostream>

#include "mb_service.hpp"

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
		return m_store->movies;
	}

	std::vector<std::string> Service::getTheatersForMovie(std::string_view movie) const
	{
		return {};
	}

} // namespace movie_booking