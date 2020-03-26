cmake_minimum_required(VERSION 3.14)
project(hw17 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-pthread -lm")

add_executable(hw17 17-1.c)