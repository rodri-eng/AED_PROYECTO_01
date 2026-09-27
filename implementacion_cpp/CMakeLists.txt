cmake_minimum_required(VERSION 3.20)
project(implementacion_cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(implementacion_cpp main.cpp
        AssociativeQueue.h
        TraceLogger.h)

include(CTest)
if(BUILD_TESTING)
    add_executable(queue_tests tests/queue_tests.cpp)
    add_test(NAME queue_tests COMMAND queue_tests)
endif()
