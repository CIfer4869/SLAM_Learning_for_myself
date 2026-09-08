cmake_minimum_required( VERSION 3.10 )
project( useGeometry )

# 添加Eigen3库的头文件路径
include_directories( /usr/include/eigen3 )

add_executable( eigenGeometry eigenGeometry.cpp )