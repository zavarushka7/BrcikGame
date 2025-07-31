cmake_minimum_required(VERSION 3.10)
project(tetris_lib)

set(CMAKE_C_STANDARD 11)

add_library(tetris SHARED backend.c)
target_include_directories(tetris PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)