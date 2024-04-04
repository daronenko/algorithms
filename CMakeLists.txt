cmake_minimum_required(VERSION 3.27)
project(vk_algorithms)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Werror -Wall -Wextra -Wpedantic -g -fno-omit-frame-pointer)

add_executable(vk_algorithms
        module-1/6-k-mean.cpp
)
