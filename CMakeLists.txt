cmake_minimum_required(VERSION 3.8)
project(socket_programming)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        utils/socket_utils.cpp
        utils/socket_utils.h tcp_client/TcpClient.cpp tcp_client/TcpClient.h)
add_executable(socket_programming ${SOURCE_FILES})