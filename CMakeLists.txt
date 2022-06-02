cmake_minimum_required(VERSION 3.0.0)
project(zbor VERSION 0.1.0)

add_library(zborlib INTERFACE)
target_include_directories(zborlib INTERFACE inc)
target_compile_features(zborlib INTERFACE cxx_std_20)
target_compile_options(zborlib INTERFACE "-Wall" "-Wextra" "-Wpedantic")

add_executable(zbor main.cpp)
target_link_libraries(zbor PRIVATE zborlib)

add_executable(zbortest 
    test/test_decode.cpp
    test/test_encode.cpp
    test/test_object.cpp)
target_link_libraries(zbortest PRIVATE gtest_main zborlib)

enable_testing()
include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)
gtest_discover_tests(zbortest)
