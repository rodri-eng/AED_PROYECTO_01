cmake_minimum_required(VERSION 4.0)
project(implementacion_cpp)

set(CMAKE_CXX_STANDARD 20)

add_executable(implementacion_cpp main.cpp
        AssociativeQueue.h
        TraceLogger.h)
