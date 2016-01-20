project(Bob CXX)

cmake_minimum_required(VERSION 3.3)

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

find_library(SDL2_LIB SDL2)
find_library(SDL2_GFX_LIB SDL2_gfx)
find_library(SDL2_IMAGE_LIB SDL2_image)
find_library(SDL2_TTF_LIB SDL2_ttf)
find_library(BOOST_UUID_LIB boost/uuid)

add_subdirectory(src)