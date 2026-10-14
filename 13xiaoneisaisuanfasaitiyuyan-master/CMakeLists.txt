cmake_minimum_required(VERSION 3.22)

project(vision)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

add_executable(vision vision.cpp)

target_link_libraries(vision PRIVATE ${OpenCV_LIBS})