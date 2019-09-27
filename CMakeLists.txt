cmake_minimum_required(VERSION 3.8.2)  # CMake version check
project(simple_example)               # Create project "simple_example"
set(CMAKE_CXX_STANDARD 14)            # Enable c++14 standard

# Add main.cpp file of project root directory as source file
set(SOURCE_FILES
        src/common.c
        src/malloc_stub.c
        src/mem.c
        src/memshell.c
        src/test_base.c
        src/test_cheese.c
        src/test_fusion.c
        src/test_init.c)

# Add executable target with source files listed in SOURCE_FILES variable
add_executable(simple_example ${SOURCE_FILES})