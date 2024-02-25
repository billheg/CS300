cmake_minimum_required(VERSION 3.26)
project(LinkedList)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

add_executable(LinkedList LinkedList.cpp)
