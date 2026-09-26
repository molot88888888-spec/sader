cmake_minimum_required(VERSION 3.20)

project(sader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(sader
    src/main.cpp
    src/CommandParser.cpp
    src/Executor.cpp
    src/HashWorker.cpp
    src/FileWorker.cpp
)

target_include_directories(sader PRIVATE include)
