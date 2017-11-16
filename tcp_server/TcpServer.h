//
// Created by tarek on 11/16/17.
//

#ifndef TCP_SERVER_TCPSERVER_H
#define TCP_SERVER_TCPSERVER_H

#include "socket_utils.h"

class TcpServer {
public:
    void handle_tcp_client(int client_socket, struct sockaddr *client_addr);

private:
    const size_t RCV_BUF_SIZE = 32;
};


#endif //TCP_SERVER_TCPSERVER_H
