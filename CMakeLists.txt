cmake_minimum_required(VERSION 3.10)
project(ufluka LANGUAGES C CXX)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDenInfo)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ufluka
    src/ufluka.cpp
)
target_compile_options(ufluka PRIVATE "-Wall")