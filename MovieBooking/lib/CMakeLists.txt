cmake_minimum_required(VERSION 3.10)
project(MovieBookingLib)

add_library(MovieBookingLib STATIC)

target_include_directories(MovieBookingLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(MovieBookingLib
  PRIVATE
  "src/movie_booking.cxx"
  "src/movie_booking.cpp"
  "src/mb_service.cpp"
  "src/workers.cpp"
  "src/cmd.cpp"
  PRIVATE
  FILE_SET header_files TYPE HEADERS FILES
  "src/movie_booking.hpp"
  "src/mb_service.hpp"
  "src/workers.hpp"
  "src/cmd.hpp"
  PUBLIC
  FILE_SET ixx_modules TYPE CXX_MODULES FILES
  "include/mbooking/movie_booking.ixx"
  "src/api.ixx"
)

target_compile_features(MovieBookingLib PRIVATE cxx_std_20)

target_link_libraries(MovieBookingLib PRIVATE libzmq)
