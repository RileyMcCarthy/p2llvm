add_compile_options(-falign-functions=128)

add_library(p2db STATIC
    p2db_isr.c
    p2db.c
)
# Prefer our freestanding libc headers over host libc to avoid glibc-only includes
target_include_directories(p2db BEFORE PRIVATE ${CMAKE_SOURCE_DIR}/../libc/include)
target_include_directories(p2db PRIVATE ${CMAKE_SOURCE_DIR}/include)