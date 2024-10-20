add_library(emueight_libretro SHARED libretro.c libretro.h)
target_link_libraries(emueight_libretro PRIVATE emueight)
