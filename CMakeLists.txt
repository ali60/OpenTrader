cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_STANDARD 11)
project(OpenTrader)
 
#Bring the headers, such as Student.h into the project
include_directories(include)
 
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB LIB_SOURCES "lib/*.cpp")
file(GLOB LIB_CL_SOURCES "lib/cl/*.cpp")

set(SOURCES ${LIB_SOURCES} ${LIB_CL_SOURCES} OpenTrader.cpp)

add_executable(OpenTrader ${SOURCES})
target_link_libraries(OpenTrader OpenCL)

