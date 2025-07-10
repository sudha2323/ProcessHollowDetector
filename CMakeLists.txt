cmake_minimum_required(VERSION 3.10)
project(ProcessHollowDetector)

set(CMAKE_CXX_STANDARD 17)

add_executable(ProcessHollowDetector src/main.cpp)
target_link_libraries(ProcessHollowDetector psapi)
