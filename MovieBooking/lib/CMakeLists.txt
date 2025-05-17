cmake_minimum_required(VERSION 3.10)
project(MovieBookingLib)

add_library(MovieBookingLib STATIC src/movie_booking.cpp)

target_include_directories(MovieBookingLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
