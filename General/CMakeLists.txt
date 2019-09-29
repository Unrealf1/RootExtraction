cmake_minimum_required(VERSION 3.5)

project(RootExtraction-General)

SET(CMAKE_CXX_COMPILER /usr/bin/clang++)

file(GLOB_RECURSE SOURCES . source/*.cpp)

ROOT_GENERATE_DICTIONARY(GeoManagerExtractor LINKDEF LinkDef.h)

add_library(General SHARED ${SOURCES} GeoManagerExtractor.cxx)

target_link_libraries(General PUBLIC ${ROOT_LIBRARIES})

target_include_directories(General PUBLIC "./include")
