cmake_minimum_required(VERSION 3.5)
project(tcp_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        TcpServer.cpp
        TcpServer.h)

add_executable(tcp_server ${SOURCE_FILES})

target_link_libraries(tcp_server ${CMAKE_SOURCE_DIR}/../utils/cmake-build-debug/libutils.a)
include_directories(${CMAKE_SOURCE_DIR}/../utils)
