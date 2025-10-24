add_library(p2
    crt0.c
    propeller2.c
    cstd_init_dummy.c
)

# Ensure we prefer the project's freestanding libc headers over host libc
# This fixes CI on Linux where system stdint.h pulls in glibc internals
target_include_directories(p2 BEFORE PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/../libc/include
)

target_link_libraries(p2 builtins)