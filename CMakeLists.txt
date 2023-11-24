cmake_minimum_required(VERSION 3.26)
project(c_fs C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

add_executable(c_fs
        include/filesystem.h
        src/filesystem.c
        #src/main.c
        test/test_cases.c)
