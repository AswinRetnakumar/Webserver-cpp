cmake_minimum_required(VERSION 3.13)

project(webserver-cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard


# Add the include directory for ThreadPool.h
include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(threadpool STATIC src/ThreadPool.cpp)

add_executable(server   src/main.cpp)


# Add the unit test executable
add_executable(ThreadPoolTest src/utest/ThreadPoolTest.cpp)
target_link_libraries(ThreadPoolTest PRIVATE threadpool gtest gtest_main pthread)