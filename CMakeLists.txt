cmake_minimum_required(VERSION 3.5)
project(bdd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES main.c bdd.h)
add_executable(bdd ${SOURCE_FILES})
