cmake_minimum_required(VERSION 3.16)
project(9804project)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. DIR_SRCS)
add_executable(9804project main.cpp sideWindowFilter.cpp detectSquare.cpp)

# opencv
set(OpenCV_DIR C:\\opencv\\mingw-build)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(9804project ${OpenCV_LIBS})

# armadillo
target_include_directories(9804project PUBLIC include)