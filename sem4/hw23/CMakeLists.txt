cmake_minimum_required(VERSION 3.16)
project(hw23 C)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw23 23-1.cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE REQUIRED fuse3)

link_libraries(${FUSE_LIBRARIES})         # -lfuse3 -lpthread
include_directories(${FUSE_INCLUDE_DIRS}) # -I/usr/include/fuse3
add_compile_options(${FUSE_CFLAGS_OTHER})     # empty since fuse 3.0