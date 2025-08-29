cmake_minimum_required(VERSION 3.28)
project(gameoflife VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)

find_package(SDL2 REQUIRED)
include_directories(gameoflife ${SDL2_INCLUDE_DIRS})

add_subdirectory(src)

add_executable(gameoflife src/main.cpp)
target_link_libraries(gameoflife ${SDL2_LIBRARIES})