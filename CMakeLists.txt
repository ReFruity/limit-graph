cmake_minimum_required(VERSION 3.6)
project(threshold_graph)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp graph.hpp graph.cpp partition.hpp partition.cpp transition.hpp transition.cpp algorithm.cpp algorithm.hpp test.hpp test.cpp)
add_executable(threshold_graph ${SOURCE_FILES})