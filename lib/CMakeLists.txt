add_library(passgen_lib STATIC
    src/error.c
    src/cli_lexer.c
    src/charset.c
    src/config_parser.c
    src/prob_math.c
    src/generator.c
    src/passgen.c
)

target_include_directories(passgen_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(passgen_lib PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)