cmake_minimum_required(VERSION 3.25)

project(irreverence)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/game/bin)

include_directories(src)

find_package(PkgConfig)

option(CGLM_STATIC "Static build" ON)
add_subdirectory(vendor/cglm)

set(CGLM_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cglm/include)

add_subdirectory(vendor/joltc)

set(JOLTC_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/joltc/include)

set(STB_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb)

set(IMGUI_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cimgui)
set(IMGUI_BACKEND_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/vendor/cimgui/imgui/backends)

# set(IMGUI_STATIC TRUE)
add_subdirectory(vendor/cimgui)

if (CMAKE_C_BYTE_ORDER EQUAL BIG_ENDIAN)
	add_definitions(-DCPU_BIG_ENDIAN)
else()
	add_definitions(-DCPU_LITTLE_ENDIAN)
endif()

if (WIN32)
	add_definitions(-DPLATFORM_WINDOWS)
elseif (UNIX)
	add_definitions(-DPLATFORM_LINUX)
endif()

if (CMAKE_BUILD_TYPE EQUAL Debug)
	add_definitions(-DDEBUG)
else()
	add_definitions(-DRELEASE)
endif()

add_subdirectory(src/engine)
add_subdirectory(src/runtime)
add_subdirectory(src/math)
add_subdirectory(src/platform)
add_subdirectory(src/util)
add_subdirectory(src/dlls)
add_subdirectory(src/cl_dll)
