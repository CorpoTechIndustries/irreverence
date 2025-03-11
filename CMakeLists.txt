cmake_minimum_required(VERSION 3.25)

project(irreverence)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game/bin)

include_directories(src)

find_package(PkgConfig)

add_subdirectory(src/engine)
add_subdirectory(src/runtime)
add_subdirectory(src/math)
add_subdirectory(src/platform)
