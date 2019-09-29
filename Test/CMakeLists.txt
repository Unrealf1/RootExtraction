cmake_minimum_required(VERSION 3.5)

project(RootExtraction-Test)

SET(CMAKE_CXX_COMPILER /usr/bin/clang++)

enable_testing()
find_package(GTest REQUIRED)

#file(GLOB_RECURSE SOURCES . source/*.cpp)

add_executable(Test source/test_main.cpp)

target_include_directories(Test PUBLIC "./include")

target_link_libraries(Test PUBLIC General GTest::GTest GTest::Main ${ROOT_LIBRARIES})

add_test(CommonTestsAll Test)


