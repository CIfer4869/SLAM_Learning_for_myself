cmake_minimum_required(VERSION 3.10)
project(helloSLAM)
# 可执行目标 1：直接编译 main.cpp
add_executable(helloslam main.cpp)
# 静态库目标：生成 libhello.a
add_library(hello libhello.cpp)
# 可执行目标 2：编译 useHello.cpp
add_executable(useHello useHello.cpp)
# 将静态库 hello 链接到 useHello 可执行程序
target_link_libraries(useHello hello)
# 设置 Debug 编译模式，开启调试信息（断点调试需要）
set(CMAKE_BUILD_TYPE "Debug")

