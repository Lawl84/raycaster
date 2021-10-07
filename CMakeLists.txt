cmake_minimum_required(VERSION 3.20.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(raycaster)

find_package(SDL2 CONFIG REQUIRED)

add_executable(raycaster
src/main.cpp
src/window.cpp
src/window.h
)

target_link_libraries(raycaster PRIVATE SDL2::SDL2 SDL2::SDL2main)

set_property(TARGET raycaster PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})