cmake_minimum_required(VERSION 2.8)

project(thor C CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(libs)
add_subdirectory(src)
