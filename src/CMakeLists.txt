find_package(Boost REQUIRED COMPONENTS program_options)

add_executable(${PROJECT_NAME} 
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    FileComparatorLib
    Boost::program_options
)

target_include_directories(${PROJECT_NAME} 
    PRIVATE 
    ${Boost_INCLUDE_DIRS}
)
