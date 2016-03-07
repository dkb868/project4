cmake_minimum_required(VERSION 3.3)
project(project4)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp DiskMultiMap.h MultiMapTuple.h BinaryFile.h IntelWeb.h InteractionTuple.h DiskMultiMap.cpp)
add_executable(project4 ${SOURCE_FILES})