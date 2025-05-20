# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.28)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MovieBooking")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(GTest CONFIG REQUIRED)
message(STATUS "Found GTest version: ${GTest_VERSION}")
find_package(ZeroMQ CONFIG REQUIRED)
message(STATUS "Found ZeroMQ version: ${ZeroMQ_VERSION}")
find_package(cppzmq CONFIG REQUIRED)
message(STATUS "Found cppzmq version: ${cppzmq_VERSION}")

find_package(nlohmann_json CONFIG REQUIRED)
message(STATUS "Found nlohmann_json version: ${nlohmann_json_VERSION}")

find_package(Doxygen REQUIRED)

enable_testing()

# Include sub-projects.
add_subdirectory ("src")
add_subdirectory ("lib")
add_subdirectory ("tests")

add_custom_target(doc
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating documentation with Doxygen"
    VERBATIM
)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/html DESTINATION share/doc)
