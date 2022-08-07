cmake_minimum_required(VERSION 3.0.0)
project(utl VERSION 0.1.0)

add_library(libutl INTERFACE)
target_include_directories(libutl INTERFACE inc)
target_compile_features(libutl INTERFACE cxx_std_17)
target_compile_options(libutl INTERFACE "-Wall" "-Wextra" "-Wpedantic")

add_executable(utl main.cpp)
target_compile_features(utl PRIVATE cxx_std_17)
target_link_libraries(utl PRIVATE libutl)

add_executable(testutl 
    test/base.cpp
    test/bit.cpp
    test/math.cpp
    test/physics.cpp
    test/str.cpp)
target_compile_features(testutl PRIVATE cxx_std_17)
target_link_libraries(testutl PRIVATE gtest_main libutl)

enable_testing()
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
gtest_discover_tests(testutl)