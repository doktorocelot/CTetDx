cmake_minimum_required(VERSION 3.26)
project(CTetDx)

set(CMAKE_CXX_STANDARD 20)

set(SOURCES
        src/main.cpp
        src/window/window.hpp
        src/window/window.cpp
        src/d3d/check-result.hpp
        src/d3d/check-result.cpp
)

add_executable(CTetDx WIN32 ${SOURCES})
