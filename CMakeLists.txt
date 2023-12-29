cmake_minimum_required(VERSION 3.26)
project(CTetDx)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Sources to interact with win32
set(WIN32_SOURCES
        src/win32_main.cpp
        
        src/win32/win32_window.hpp src/win32/win32_window.cpp
        src/win32/win32_check-result.hpp src/win32/win32_check-result.cpp
        src/win32/win32_kill-program.hpp src/win32/win32_kill-program.cpp
        src/win32/win32_files.hpp src/win32/win32_files.cpp
        src/win32/win32_memory.hpp src/win32/win32_memory.cpp
)

# Sources to interact with directx11
set(DIRECTX11_SOURCES
        src/render/d3d11/d3d11_renderer.hpp src/render/d3d11/d3d11_renderer.cpp
        src/render/d3d11/d3d11_shader-pair.hpp src/render/d3d11/d3d11_shader-pair.cpp
        src/render/d3d11/d3d11_engine-rendering-context.hpp src/render/d3d11/d3d11_engine-rendering-context.cpp
        src/render/d3d11/d3d11_mesh.hpp src/render/d3d11/d3d11_mesh.cpp
        src/render/d3d11/d3d11_frame-mesh.hpp src/render/d3d11/d3d11_frame-mesh.cpp
        src/render/d3d11/d3d11_block-mesh.hpp src/render/d3d11/d3d11_block-mesh.cpp
        src/render/d3d11/d3d11_texture.hpp src/render/d3d11/d3d11_texture.cpp 
)

set(PLATFORM_SOURCES ${WIN32_SOURCES} ${DIRECTX11_SOURCES})

set(SOURCES
        src/control/control.hpp src/control/control.cpp
        
        src/render/block-batch.hpp src/render/block-batch.cpp
        src/render/bmp.hpp src/render/bmp.cpp

        src/util/fps-counter.hpp src/util/fps-counter.cpp

        src/math/vector2.hpp
        src/math/vector3.hpp
        src/math/vector2-cross-ctet.hpp
        src/math/rect.hpp
)

add_executable(CTetDx WIN32 ${SOURCES} ${PLATFORM_SOURCES})
add_subdirectory(extern/ctet)

target_link_libraries(CTetDx PUBLIC CTet)
target_link_libraries(CTetDx PUBLIC Shlwapi.lib)
target_link_libraries(CTetDx PUBLIC d3d11.lib dxguid.lib d3dcompiler.lib)

add_compile_options(-DUNICODE -D_UNICODE)

target_compile_options(CTetDx PRIVATE "/utf-8")
target_compile_options(CTetDx PRIVATE "/D_UNICODE" "/DUNICODE")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
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
else ()
    # Configuration for Debug mode
    add_definitions(-DDEBUG)
    message("Building in Debug mode")
    add_custom_command(
            TARGET CTetDx
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E
            create_symlink ${CMAKE_SOURCE_DIR}/resources ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
endif ()
