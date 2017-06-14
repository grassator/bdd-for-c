cmake_minimum_required(VERSION 3.5)
project(bdd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(EXAMPLE_SOURCES example.c bdd-for-c.h)
add_executable(example_test ${EXAMPLE_SOURCES})

set(ARRAY_SOURCES array.c bdd-for-c.h array.h)
add_executable(array_test ${ARRAY_SOURCES})
