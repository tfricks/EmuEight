cmake_minimum_required(VERSION 3.5.0)
project(EmuEight VERSION 0.1.0 LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

aux_source_directory(. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})
#add_executable(${PROJECT_NAME} main.c)

# For macOS M2 systems with SDL2 installed via Homebrew
list(APPEND CMAKE_MODULE_PATH "/usr/local/lib/cmake/SDL2" "/opt/homebrew/lib/cmake/SDL2")
list(APPEND CMAKE_PREFIX_PATH "/usr/local" "/opt/homebrew")

find_package(SDL2 REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries( EmuEight
    PRIVATE
        SDL2::Main
)

include(CTest)
enable_testing()
