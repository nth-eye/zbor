cmake_minimum_required(VERSION 3.0.0)
project(zbor VERSION 0.1.0)

add_subdirectory(lib/utl)

add_library(libzbor INTERFACE)
target_include_directories(libzbor INTERFACE inc)
target_compile_features(libzbor INTERFACE cxx_std_17)
target_compile_options(libzbor INTERFACE "-Wall" "-Wextra" "-Wpedantic")
target_link_libraries(libzbor INTERFACE libutl)

add_executable(zbor main.cpp)
target_link_libraries(zbor PRIVATE libzbor)

add_executable(testzbor 
    test/decode.cpp
    test/encode.cpp
    test/object.cpp)
target_link_libraries(testzbor PRIVATE gtest_main libzbor)
