cmake_minimum_required(VERSION 4.0)
project(peaEngine C)

set(CMAKE_C_STANDARD 11)

include(FetchContent)

# Raylib master
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
)
FetchContent_MakeAvailable(raylib)

add_executable(peaEngine main.c
        rendering.c
        rendering.h
        engine.c
        engine.h)

target_link_libraries(peaEngine PRIVATE raylib)
