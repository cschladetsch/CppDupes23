enable_testing()

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(${PROJECT_TEST}
    test_basic.cpp
    test_advanced1.cpp
    test_advanced2.cpp
)

target_link_libraries(${PROJECT_TEST}
    PRIVATE
    FileComparatorLib
    GTest::gtest 
    GTest::gtest_main
)

target_include_directories(${PROJECT_TEST} 
    PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
)

gtest_discover_tests(${PROJECT_TEST})
