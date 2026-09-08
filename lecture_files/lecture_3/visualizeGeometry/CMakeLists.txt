cmake_minimum_required(VERSION 3.10)
project(visualizeGeometry)
set(CMAKE_CXX_STANDARD 17)

find_package(Eigen3 REQUIRED)
find_package(Pangolin REQUIRED)

include_directories(
    ${EIGEN3_INCLUDE_DIR}
    ${Pangolin_INCLUDE_DIRS}
)

add_executable(visualizeGeometry visualizeGeometry.cpp)

target_link_libraries(visualizeGeometry
    -Wl,--no-as-needed
    tiff
    -Wl,--as-needed
    ${Pangolin_LIBRARIES}
)
