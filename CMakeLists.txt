cmake_minimum_required(VERSION 3.10)
project(FormatString C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(FormatString
        Example.c
        FormatString.h FormatString.c)
