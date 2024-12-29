add_library(FileComparatorLib STATIC
    FileComparator.cpp
)
target_link_libraries(FileComparatorLib PRIVATE stdc++fs)
target_link_libraries(FileComparatorLib PRIVATE FileComparatorInclude)
target_include_directories(FileComparatorLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(FileComparatorLib PUBLIC ${CMAKE_OURCE_DIR}/include)

