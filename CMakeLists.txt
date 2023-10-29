cmake_minimum_required(VERSION 3.26)
project(zydequeue C)

set(CMAKE_C_STANDARD 23)

include_directories(zyalloc)

file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true "zyalloc/*.h" "zyalloc/*.c")

set(SOURCES ${SOURCES} zy_dequeue.h zy_dequeue.c)

add_library(${CMAKE_PROJECT_NAME} STATIC ${SOURCES})