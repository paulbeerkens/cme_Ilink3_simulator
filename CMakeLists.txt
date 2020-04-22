cmake_minimum_required(VERSION 3.8)

project(CMESimulator VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_subdirectory(src)
add_subdirectory(external)