cmake_minimum_required(VERSION 3.5)
project(tcp_client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
	TcpClient.cpp 
	TcpClient.h)

add_executable(tcp_client ${SOURCE_FILES})

target_link_libraries(tcp_client ${CMAKE_SOURCE_DIR}/../utils/cmake-build-debug/libutils.a)
include_directories(${CMAKE_SOURCE_DIR}/../utils)
