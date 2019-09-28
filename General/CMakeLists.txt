cmake_minimum_required(VERSION 3.5)

project(RootExtraction-General)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_library(General SHARED ${SOURCES})

target_include_directories(General PUBLIC "./include")
