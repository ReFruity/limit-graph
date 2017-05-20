cmake_minimum_required(VERSION 3.6)
project(limit_graph)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp graph.hpp graph.cpp partition.hpp partition.cpp)
add_executable(limit_graph ${SOURCE_FILES})