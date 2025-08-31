cmake_minimum_required(VERSION 3.28)
project(gameoflife VERSION 0.2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(SDL2 REQUIRED)
include_directories(gameoflife "${SDL2_INCLUDE_DIRS}")
#target_include_directories(gameoflife PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")

#add_subdirectory(src)

add_executable(gameoflife src/main.cpp)
target_link_libraries(gameoflife PUBLIC "${SDL2_LIBRARIES}")
