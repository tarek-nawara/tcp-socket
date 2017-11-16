/**
 * File: main.c
 * Description: holder for server loop
 * Created Date: 2017-11-15
 */

#include <iostream>
#include "TcpServer.h"

#define SERVER_PORT 5000

int main(int argc, char *argv[]) {
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    std::cout << "---Server Starting---" << std::endl;
    int server_socket = utils::socket_wrapper();
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERVER_PORT);

    utils::bind_wrapper(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    utils::listen_wrapper(server_socket, MAXPINDING);
    TcpServer tcp_server;

    while(true) {
        tcp_server.handle_tcp_client(server_socket, server_addr, client_addr);
    }

    return 0;
}

