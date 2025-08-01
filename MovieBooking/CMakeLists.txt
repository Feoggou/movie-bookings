﻿# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)

project ("MovieBooking")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API ON)

find_package(GTest CONFIG REQUIRED)
message(STATUS "Found GTest version: ${GTest_VERSION}")
find_package(ZeroMQ CONFIG REQUIRED)
message(STATUS "Found ZeroMQ version: ${ZeroMQ_VERSION}")
find_package(cppzmq CONFIG REQUIRED)
message(STATUS "Found cppzmq version: ${cppzmq_VERSION}")
find_package(Boost CONFIG REQUIRED COMPONENTS asio)
message(STATUS "Found Boost version: ${Boost_VERSION}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

find_package(nlohmann_json CONFIG REQUIRED)
message(STATUS "Found nlohmann_json version: ${nlohmann_json_VERSION}")

# find_package(Doxygen REQUIRED)

enable_testing()

# Include sub-projects.
add_subdirectory ("src")
add_subdirectory ("lib")
add_subdirectory ("tests")

# add_custom_target(doc
#     COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
#     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     COMMENT "Generating documentation with Doxygen"
#     VERBATIM
# )

# install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION share/doc)
