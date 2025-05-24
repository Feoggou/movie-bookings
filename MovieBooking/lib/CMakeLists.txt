cmake_minimum_required(VERSION 3.10)
project(MovieBookingLib)

add_library(MovieBookingLib STATIC
            src/movie_booking.cpp src/movie_booking.hpp
            src/mb_service.cpp src/mb_service.hpp
            src/workers.cpp src/workers.hpp
            src/zeromq_sync.cpp src/zeromq_sync.hpp
            src/utils.hpp
)

target_include_directories(MovieBookingLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(MovieBookingLib PRIVATE libzmq)