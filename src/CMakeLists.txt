find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    FileComparatorLib
    FileComparatorInclude
    Boost::program_options
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})

