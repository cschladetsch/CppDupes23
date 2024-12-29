cmake_minimum_required(VERSION 3.20)
project(FileComparator CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PROJECT_NAME fs2)
set(PROJECT_TEST fs2-test)

# Root CMakeLists.txt
include(FetchContent)
# Automatically add include directory to all executables
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${INCLUDE_DIR})


# Download rang.hpp if not present
set(RANG_HEADER_PATH ${CMAKE_BINARY_DIR}/_deps/rang.hpp)
if(NOT EXISTS ${RANG_HEADER_PATH})
    file(DOWNLOAD
        https://raw.githubusercontent.com/agauniyal/rang/master/include/rang.hpp
        ${RANG_HEADER_PATH}
        SHOW_PROGRESS
    )
endif()
include_directories(${CMAKE_BINARY_DIR}/_deps)

# Download and build GTest if not present
FetchContent_Declare(
    gtest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/_deps
)
FetchContent_MakeAvailable(gtest)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)

# include/CMakeLists.txt
add_library(FileComparatorInclude INTERFACE)
target_include_directories(FileComparatorInclude INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

