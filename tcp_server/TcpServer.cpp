//
// Created by tarek on 11/16/17.
//

#include "TcpServer.h"

void TcpServer::handle_tcp_client(int client_socket, struct sockaddr *client_addr) {
    char buf[RCV_BUF_SIZE];
    socklen_t client_addr_len = sizeof(client_addr);
    ssize_t rcv_msg_size = rcv_wrapper(client_socket, buf, RCV_BUF_SIZE, 0, client_addr, &client_addr_len);
    buf[rcv_msg_size] = '\0';
    
    close(client_socket);
/* Close client socket */
}
