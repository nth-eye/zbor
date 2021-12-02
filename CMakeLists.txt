cmake_minimum_required(VERSION 3.0.0)
project(zbor VERSION 0.1.0)

include_directories(inc)
file(GLOB SRC_FILES src/*.cpp) 
add_executable(zbor main.cpp ${SRC_FILES})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
