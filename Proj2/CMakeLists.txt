cmake_minimum_required(VERSION 3.26)
project(Proj2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")

add_executable(Proj2 main.cpp)
