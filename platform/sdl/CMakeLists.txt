find_package(SDL2 REQUIRED COMPONENTS SDL2)

add_executable(${PROJECT_NAME} main.c)

# target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

target_link_libraries( ${PROJECT_NAME}
    PRIVATE
        SDL2::SDL2
        emueight
)