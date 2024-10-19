set(HEADER_LIST "${CMAKE_SOURCE_DIR}/include/cpu.h")

add_library(emueight STATIC cpu.c ${HEADER_LIST})

target_include_directories(emueight PUBLIC ../../include)

source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})
