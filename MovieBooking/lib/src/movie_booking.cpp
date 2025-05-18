#include "mbooking/movie_booking.h"
#include <iostream>

MovieBookingService::MovieBookingService()
	: m_backend(std::make_unique<BookingBackend>())
{

}

MovieBookingService::MovieBookingService(std::unique_ptr<BookingBackend>&& backend)
	: m_backend(std::move(backend))
{

}

std::vector<std::string> MovieBookingService::getPlayingMovies() const
{
	return m_backend->movies;
}
