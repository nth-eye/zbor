cmake_minimum_required(VERSION 3.0.0)
project(zbor VERSION 0.1.0)

include_directories(inc ../Util-c/inc)
file(GLOB SRC_FILES src/*.cpp ../Util-c/src/util.c) 
add_executable(zbor main.cpp ${SRC_FILES})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -Wno-implicit-fallthrough")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(FetchContent)
FetchContent_Declare(googletest URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(hello_test test.cpp ${SRC_FILES})

target_link_libraries(hello_test gtest_main)

include(GoogleTest)
gtest_discover_tests(hello_test)