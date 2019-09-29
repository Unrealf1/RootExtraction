cmake_minimum_required(VERSION 3.5)

project(RootExtraction-General)

file(GLOB_RECURSE SOURCES . source/*.cpp)

ROOT_GENERATE_DICTIONARY(GeoManagerExtractor LINKDEF LinkDef.h)

add_library(General SHARED ${SOURCES} GeoManagerExtractor.cxx)

target_link_libraries(General ROOT::Graf3d)

target_include_directories(General PUBLIC "./include")
