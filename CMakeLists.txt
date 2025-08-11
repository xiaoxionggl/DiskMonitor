cmake_minimum_required(VERSION 3.10)
project(DiskMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(DiskMonitor
    src/main.cpp
    src/FileScanner.cpp
)
