/**
 * File: main.c
 * Description: holder for server loop
 * Created Date: 2017-11-15
 */

#include "TcpServer.h"
#include "socket_utils.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        utils::die_with_error("expected port number");
    }
    int server_port = atoi(argv[1]);
    struct sockaddr_in server_addr;
    std::cout << "---Server Starting---" << std::endl;
    int server_socket = utils::socket_wrapper();
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);

    utils::bind_wrapper(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    utils::listen_wrapper(server_socket, MAXPINDING);
    TcpServer tcp_server;

    while(true) {
        std::cout << "Handling client {ip:port} " << inet_ntoa(server_addr.sin_addr) << ":" << server_port << std::endl;
        tcp_server.handle_tcp_client_fork(server_socket, server_addr);
    }

    return 0;
}

