cmake_minimum_required(VERSION 3.16)
project(orthus C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_executable(Orthus
        src/main.c
        src/utils.c
        src/configuration.c
        src/matcher.c
        src/logs.c
)

target_include_directories(Orthus PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})