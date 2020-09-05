cmake_minimum_required(VERSION 3.17)
project(untitled1)

set(CMAKE_CXX_FLAGS "-march=native")
set(CMAKE_CXX_STANDARD 14)

add_executable(untitled1 main.cpp)

target_link_libraries(untitled1 pthread atomic)