cmake_minimum_required(VERSION 3.28)
project(droom)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)

add_executable(droom main.cpp
        WAD/Wad.cpp
        WAD/Wad.h
        Application.cpp
        Application.h
        Screen/Screen.cpp
        Screen/Screen.h)

target_link_libraries(droom PRIVATE SDL2::SDL2)

add_executable(mapvis WAD/Wad.cpp
        WAD/Wad.h
        Application.cpp
        Application.h
        MapVisualizer.cpp
        Screen/Screen.cpp
        Screen/Screen.h)
target_link_libraries(mapvis PRIVATE SDL2::SDL2)
