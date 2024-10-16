cmake_minimum_required(VERSION 3.5.0)
project(EmuEight VERSION 0.1.0 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

add_subdirectory(src)
