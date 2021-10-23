cmake_minimum_required (VERSION 3.14)
project (dspaces_perf VERSION 0.0.1 LANGUAGES C)

include(GNUInstallDirs)
include(CTest)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})
endif()

find_package(dspaces REQUIRED)
find_package(MPI COMPONENTS REQUIRED)

include (xpkg-import)
xpkg_import_module(apex REQUIRED apex)

add_subdirectory(src)
add_subdirectory(scripts)
add_subdirectory(tests)
