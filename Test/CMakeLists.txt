cmake_minimum_required(VERSION 3.5)

project(RootExtraction-Test)

find_package(ROOT CONFIG REQUIRED)

enable_testing()
find_package(GTest REQUIRED)

# list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
# include(${ROOT_USE_FILE})

#file(GLOB_RECURSE SOURCES . source/*.cpp)
file(GLOB_RECURSE SOURCES2 . $ENV{ROOTSYS}/lib/*.so)

add_executable(Test source/test_main.cpp ${SOURCES2})

target_include_directories(Test PUBLIC "./include")

target_link_libraries(Test PUBLIC ${SOURCES2} General GTest::GTest GTest::Main ROOT::Core ROOT::Graf3d ROOT::Graf ROOT::RIO ROOT::Gpad)

add_test(CommonTestsAll Test)


