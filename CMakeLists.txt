cmake_minimum_required(VERSION 3.6)
project(vscale)

set(CMAKE_CXX_STANDARD 11)
set(SOURCE_FILES src/vscale.cpp main.cpp)
include_directories(include)

add_executable(vscale ${SOURCE_FILES})
target_link_libraries(vscale curl jsoncpp)