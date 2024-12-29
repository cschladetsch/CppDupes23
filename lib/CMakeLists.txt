add_library(FileComparatorLib STATIC
    FileComparator.cpp
)

target_include_directories(FileComparatorLib 
    PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
