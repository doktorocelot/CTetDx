cmake_minimum_required(VERSION 3.26)
project(CTetDx)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        src/main.cpp
        src/window/window.hpp
        src/window/window.cpp
)

add_executable(CTetDx WIN32 ${SOURCES})
