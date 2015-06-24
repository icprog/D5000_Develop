cmake_minimum_required(VERSION 3.2)
project(D5000_Develop)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(D5000_Develop ${SOURCE_FILES})

INCLUDE_DIRECTORIES(/Users/JasonYang/Documents/src/include_d5000)