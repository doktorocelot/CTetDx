cmake_minimum_required(VERSION 3.26)
project(CTetDx)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SOURCES
        src/main.cpp
        src/window/window.hpp
        src/window/window.cpp
        src/d3d/check-result.hpp
        src/d3d/check-result.cpp
        src/d3d/d3d-render.hpp
        src/d3d/d3d-render.cpp
)

add_executable(CTetDx WIN32 ${SOURCES})

target_link_libraries(CTetDx PUBLIC d3d11.lib dxguid.lib d3dcompiler.lib)