cmake_minimum_required(VERSION 3.17)

add_library(Core STATIC
    src/Token.cpp
    src/Task.cpp
)

target_include_directories(Core 
    PUBLIC inc)
