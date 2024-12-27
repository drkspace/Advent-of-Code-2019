cmake_minimum_required(VERSION 3.30)
project(avoc2019)

set(CMAKE_CXX_COMPILER g++-14)
set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -Werror)

add_executable(day01 day01/main.cpp utils/utils.h)
add_executable(day02 day02/main.cpp utils/utils.h)
add_executable(day03 day03/main.cpp utils/utils.h)
