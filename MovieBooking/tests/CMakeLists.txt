add_executable(moviebooking_test mb_service_test.cpp)

target_include_directories(moviebooking_test PUBLIC "${CMAKE_SOURCE_DIR}/lib")

target_link_libraries(
  moviebooking_test
  PRIVATE
  MovieBookingLib
  GTest::gmock_main
)

target_compile_features(moviebooking_test PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(moviebooking_test)
