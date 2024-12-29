cmake_minimum_required(VERSION 3.20)

project(FileComparator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PROJECT_NAME fs2)
set(PROJECT_TEST fs2-test)

find_package(Boost REQUIRED COMPONENTS filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

# Download and build GTest
include(FetchContent)
FetchContent_Declare(
    gtest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/_deps
)
FetchContent_MakeAvailable(gtest)

# Download rang.hpp if not present
set(RANG_HEADER_PATH ${CMAKE_BINARY_DIR}/_deps/rang.hpp)
if(NOT EXISTS ${RANG_HEADER_PATH})
    file(DOWNLOAD
        https://raw.githubusercontent.com/agauniyal/rang/master/include/rang.hpp
        ${RANG_HEADER_PATH}
        SHOW_PROGRESS
    )
endif()

# Add include directories globally
include_directories(${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR}/_deps)

# Add subdirectories
add_subdirectory(lib)
add_subdirectory(src)
