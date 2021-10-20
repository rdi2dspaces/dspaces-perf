cmake_minimum_required (VERSION 3.14)
project (dspaces_perf VERSION 0.0.1 LANGUAGES C)

include(GNUInstallDirs)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()

find_package(dspaces)
find_package(MPI COMPONENTS REQUIRED)

add_subdirectory(src)
add_subdirectory(scripts)
