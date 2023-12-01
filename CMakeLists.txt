cmake_minimum_required(VERSION 3.16)
project(OSI2_4 C)

set(CMAKE_C_STANDARD 11)

add_executable(OSI2_4
        list_threads.c
        list.c)

target_link_libraries(OSI2_4 pthread)

