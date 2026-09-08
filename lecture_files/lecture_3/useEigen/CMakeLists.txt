cmake_minimum_required(VERSION 3.10) # 设置 CMake 的最低版本要求
project( useEigen ) # 设置项目名称

set( CMAKE_BUILD_TYPE "Release" ) # 设置编译类型为 Release
set( CMAKE_CXX_FLAGS "-O3" ) # 设置编译选项为 -O3，表示优化级别为 3

# 添加 Eigen3 库的头文件路径
include_directories( /usr/include/eigen3 )

# 生成可执行程序：程序名字 eigenMatrix，由源码 eigenMatrix.cpp 编译得到
add_executable( eigenMatrix eigenMatrix.cpp )