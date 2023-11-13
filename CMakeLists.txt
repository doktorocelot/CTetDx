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
        src/die.hpp
        src/die.cpp
        src/d3d/d3d-game.hpp
        src/d3d/d3d-game.cpp
        src/d3d/shader-pair.hpp
        src/d3d/shader-pair.cpp
        src/control/control.hpp
        src/control/control.cpp
        src/d3d/block-batch.hpp
        src/d3d/block-batch.cpp
)

add_executable(CTetDx WIN32 ${SOURCES})
add_subdirectory(extern/ctet)

target_link_libraries(CTetDx PUBLIC CTet)
target_link_libraries(CTetDx PUBLIC d3d11.lib dxguid.lib d3dcompiler.lib)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Configuration for Release mode
    message("Building in Release mode")
    add_custom_target(CopyResources
            COMMAND ${CMAKE_COMMAND} -E remove_directory 
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
            
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/resources
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources
    )
    add_dependencies(CTetDx CopyResources)
else()
    # Configuration for Debug mode
    add_definitions(-DDEBUG)
    message("Building in Debug mode")
    add_custom_command(
            TARGET CTetDx
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
            create_symlink ${CMAKE_SOURCE_DIR}/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
endif()
