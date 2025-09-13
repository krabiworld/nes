cmake_minimum_required(VERSION 4.0)
project(nes)

set(CMAKE_CXX_STANDARD 26)

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3)

add_executable(nes main.cpp
        cpu.cpp
        cpu.hpp
        memory.hpp
)

target_link_libraries(nes PRIVATE SDL3::SDL3)
