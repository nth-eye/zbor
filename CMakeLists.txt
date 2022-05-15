cmake_minimum_required(VERSION 3.0.0)
project(zbor VERSION 0.1.0)

add_library(xborlib INTERFACE)
target_compile_features(xborlib INTERFACE cxx_std_17)
target_compile_options(xborlib INTERFACE "-Wall" "-Wextra" "-Wpedantic")
target_include_directories(xborlib INTERFACE inc)

add_executable(zbor main.cpp)
target_compile_features(zbor PUBLIC cxx_std_14)
target_link_libraries(zbor PUBLIC xborlib)

enable_testing()
file(GLOB TEST_FILES test/*.cpp) 

add_executable(xbortest ${TEST_FILES})
target_compile_features(xbortest PUBLIC cxx_std_17)
target_link_libraries(xbortest PUBLIC gtest_main xborlib)

include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
gtest_discover_tests(xbortest)