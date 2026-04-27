cmake_minimum_required(VERSION 3.16)
project(orthus)

# set(CMAKE_BUILD_TYPE Debug)

# set(CMAKE_C_STANDARD 11)
# set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)
file(GLOB SRC "src/*.c")

add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE
        include
        )