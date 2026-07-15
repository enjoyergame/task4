add_library(passgen_lib
    src/error.c
    include/passgen/error.h
)

target_include_directories(passgen_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)