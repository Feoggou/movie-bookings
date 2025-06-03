cmake_minimum_required(VERSION 3.10)
project(MovieBookingLib)

add_library(MovieBookingLib STATIC)

target_include_directories(MovieBookingLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_sources(MovieBookingLib
  PRIVATE
  "src/movie_booking.cpp" "src/movie_booking.hpp"
  "src/mb_service.cpp" "src/mb_service.hpp"
  "src/workers.cpp" "src/workers.hpp"
  "src/zeromq_sync.cpp" "src/zeromq_sync.hpp"
  "src/zeromq_async.cpp" "src/zeromq_async.hpp"
  "src/cmd.cpp" "src/cmd.hpp"
  "src/utils.hpp"
)

target_compile_features(MovieBookingLib PRIVATE cxx_std_20)

target_link_libraries(MovieBookingLib PRIVATE libzmq)