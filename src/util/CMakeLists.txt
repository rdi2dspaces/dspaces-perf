add_executable(fast_put fast_put.c)

target_include_directories(fast_put PRIVATE 
    ${MPI_C_INCLUDE_PATH}
    ${PROJECT_SOURCE_DIR}/src/util)
target_compile_options(fast_put PRIVATE ${MPI_C_COMPILE_FLAGS})
target_link_libraries(fast_put ${MPI_C_LIBRARIES} ${MPI_C_LINK_FLAGS} dspaces::dspaces apex)

