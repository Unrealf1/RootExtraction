cmake_minimum_required(VERSION 3.5)

project(RootExtraction-Test)

enable_testing()
find_package(GTest REQUIRED)

file(GLOB_RECURSE SOURCES . source/*.cpp)

add_executable(Test ${SOURCES})
    
target_include_directories(Test PUBLIC "./include")

target_link_libraries(Test PUBLIC General GTest::GTest GTest::Main)

add_test(CommonTestsAll Test)
