register_all_tests_in_dir("${CMAKE_CURRENT_SOURCE_DIR}" passgen_tests passgen_lib)

target_include_directories(test_passgen_tests_passgen_tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/src
)