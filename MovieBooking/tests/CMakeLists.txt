
enable_testing()

add_executable(dummy_test dummy_test.cpp)

target_include_directories(dummy_test PUBLIC "${CMAKE_SOURCE_DIR}/lib")

target_link_libraries(
  dummy_test
  PRIVATE
  MovieBookingLib
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(dummy_test)


