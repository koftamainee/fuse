cmake_minimum_required(VERSION 3.10)

project(fuse)

set(CMAKE_C_STANDARD 99)

include_directories(include)

file(GLOB_RECURSE SRC_FILES
    src/*.c
)

add_executable(fuse ${SRC_FILES})

set_target_properties(fuse PROPERTIES RUNTIME_OUTPUT_DIRECTORY x86_64)

target_link_libraries(fuse m)