cmake_minimum_required(VERSION 3.28)
project(droom)

set(CMAKE_CXX_STANDARD 23)

find_package(SDL2 REQUIRED)

add_executable(droom main.cpp)

target_link_libraries(droom PRIVATE SDL2::SDL2)
