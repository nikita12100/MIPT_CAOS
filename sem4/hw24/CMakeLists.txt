cmake_minimum_required(VERSION 3.16)
project(hw24 C)

set(CMAKE_C_STANDARD 99)

#add_executable(hw24 matrix1.c)

set ( GCC_COVERAGE_LINK_FLAGS "-lssl -lcrypto")
find_package(PythonLibs 3.8 REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
python_add_module(aes256 24-1.c)
target_link_libraries(aes256 ${PYTHON_LIBRARIES})
