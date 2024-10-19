cmake_minimum_required(VERSION 3.7)
project(EmuEight VERSION 0.1.0 LANGUAGES C)

include(CTest)

add_subdirectory(src)
add_subdirectory(platform)
add_subdirectory(external)
add_subdirectory(test)