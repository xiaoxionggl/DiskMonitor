cmake_minimum_required(VERSION 3.16)
project(DiskMonitor LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 可按需把 src 下的 cpp 全部加入，示例简单写法
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_executable(DiskMonitor ${SRC_FILES})
target_include_directories(DiskMonitor PRIVATE ${CMAKE_SOURCE_DIR}/include)
