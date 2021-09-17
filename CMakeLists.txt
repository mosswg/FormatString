cmake_minimum_required(VERSION 3.10)
project(FormatString C)

set(CMAKE_C_STANDARD 11)

option(BUILD_EXAMPLE_CODE "Build example code instead of building as library" OFF)

if (BUILD_EXAMPLE_CODE)
    add_executable(FormatStringExample Example.c
                FormatString.h FormatString.c)
else()
    add_library(FormatString
            FormatString.h FormatString.c)

endif()