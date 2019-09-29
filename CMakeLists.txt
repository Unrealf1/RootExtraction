cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(RootExtraction)
option(DEBUG "Build with debug flags" OFF)
option(TESTS "Build tests" OFF)

if (DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=address,undefined -g")
endif(DEBUG)

find_package(ROOT REQUIRED)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
include(${ROOT_USE_FILE})

add_subdirectory("./General")
add_subdirectory("./Test")
# if (TESTS)
#     add_subdirectory("./Test")
# endif(TESTS)
