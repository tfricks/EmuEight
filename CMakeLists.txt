cmake_minimum_required(VERSION 3.15)
project(EmuEight VERSION 0.1.0 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CTest)
include(CodeCoverage)

add_subdirectory(src)
#add_subdirectory(platform)
add_subdirectory(external)
add_subdirectory(test)
append_coverage_compiler_flags_to_target(emueight)
