cmake_minimum_required(VERSION 3.5)

project(RootExtraction-General)

find_package(ROOT CONFIG REQUIRED)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
# include(${ROOT_USE_FILE})

file(GLOB_RECURSE SOURCES . source/*.cpp)
file(GLOB_RECURSE ROOT_LIBS . $ENV{ROOTSYS}/lib/*.so)
#message(FATAL_ERROR $ENV{ROOTSYS})
#message(FATAL_ERROR ${ROOT_LIBS})

add_library(General SHARED ${SOURCES} ${ROOT_LIBS} GeoManagerExtractor.cxx)

target_include_directories(General PUBLIC "./include" ${ROOT_INCLUDE_DIRS})

target_link_libraries(General PUBLIC ${ROOT_LIBS})

ROOT_GENERATE_DICTIONARY(GeoManagerExtractor LINKDEF LinkDef.h)
