cmake_minimum_required( VERSION 3.10 )
project( useSophus )

# 为使用Sophus库，必须使用find_package命令来查找Sophus库
find_package( Sophus REQUIRED )
include_directories( ${Sophus_INCLUDE_DIRS} )

add_executable( useSophus useSophus.cpp )
target_link_libraries( useSophus ${Sophus_LIBRARIES} )