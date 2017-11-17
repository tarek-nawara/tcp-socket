/**
 * File: TcpServer.h
 * Description: Holder for TCP server interface
 * Created Date: 2017-11-16
 */

#ifndef TCP_SERVER_TCPSERVER_H
#define TCP_SERVER_TCPSERVER_H

#include <iostream>
#include <sys/sendfile.h>
#include "socket_utils.h"

class TcpServer {
public:
    void handle_tcp_client(int server_socket, sockaddr_in &server_addr, sockaddr_in client_addr);
private:
    void append_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv);

    void handle_post_request(int client_socket, const std::string &file_name, long file_len);

    void write_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv);

    void handle_get_request(int client_socket, char *req_buf);

    long file_size(const char *file_name);

    void write_success_header(int client_socket, long data);

    void write_fail_header(int client_socket);
};


#endif //TCP_SERVER_TCPSERVER_H
