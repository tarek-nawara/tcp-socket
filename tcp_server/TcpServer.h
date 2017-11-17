/**
 * File: TcpServer.h
 * Description: Holder for TCP server interface
 * Created Date: 2017-11-16
 */

#ifndef TCP_SERVER_TCPSERVER_H
#define TCP_SERVER_TCPSERVER_H

#include <iostream>
#include "socket_utils.h"

class TcpServer {
public:
    void handle_tcp_client(int server_socket, sockaddr_in &server_addr, sockaddr_in client_addr);
private:
    void resolve_request(const char *req_buf, char *url_buf, char *version_buf);
    std::pair<int, bool> open_file(char *url_buf, char *resp_buf, char *version_buf) const;
    void resolve_success_response(int client_socket, int file_des) const;

    void handle_get_request(int client_socket);

    void append_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv);

    void handle_post_request(int client_socket, const std::string &file_name, long file_len);

    void write_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv);
};


#endif //TCP_SERVER_TCPSERVER_H
