cmake_minimum_required(VERSION 4.0)
project(nes)

set(CMAKE_CXX_STANDARD 26)

add_executable(nes main.cpp
        cpu.cpp
        cpu.hpp
        memory.hpp
)
