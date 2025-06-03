# CMakeList.txt : CMake project for MovieBooking, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (MovieBooking
"main.cpp"
"zeromq_async.cpp" "zeromq_async.hpp"
"zeromq_sync.cpp" "zeromq_sync.hpp"
)

target_include_directories(MovieBooking PUBLIC "${CMAKE_SOURCE_DIR}/lib")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET MovieBooking PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(MovieBooking PRIVATE MovieBookingLib nlohmann_json::nlohmann_json "cppzmq-static" Boost::boost)
