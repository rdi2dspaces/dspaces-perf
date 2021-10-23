configure_file(
    ${PROJECT_SOURCE_DIR}/tests/test_script.sh.in
    ${PROJECT_BINARY_DIR}/tests/test_script.sh
    @ONLY
)

find_program(BASH_PROGRAM bash)

if(BASH_PROGRAM)
    add_test(Test_fast_put ${BASH_PROGRAM} test_script.sh 1)
    set_tests_properties(Test_fast_put PROPERTIES TIMEOUT 30)
endif()
