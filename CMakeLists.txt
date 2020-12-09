cmake_minimum_required(VERSION 3.17)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(coder main.cpp coder/encoding.cpp coder/decoding.cpp coder/decoding.h)
