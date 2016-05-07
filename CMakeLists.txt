cmake_minimum_required(VERSION 3.5)
project(bdd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES example.c bdd-for-c.h)
add_executable(example ${SOURCE_FILES})
