cmake_minimum_required(VERSION 3.16)
project(orthus)

set(CMAKE_BUILD_TYPE Debug)

file(GLOB SRC "src/*.c")

add_executable(${PROJECT_NAME} ${SRC})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
        include
        )