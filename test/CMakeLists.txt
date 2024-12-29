enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(${PROJECT_TEST} test_main.cpp)
target_include_directories(${PROJECT_TEST} PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_TEST} PRIVATE FileComparatorLib GTest::GTest GTest::Main)

# Discover GoogleTest cases
gtest_discover_tests(${PROJECT_TEST})

