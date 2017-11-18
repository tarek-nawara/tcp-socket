/**
 * File: TcpServer.h
 * Description: Holder for TCP server interface
 * Created Date: 2017-11-16
 */

#ifndef TCP_SERVER_TCPSERVER_H
#define TCP_SERVER_TCPSERVER_H

#include "socket_utils.h"

class TcpServer {
public:
    void handle_tcp_client_fork(int server_socket, sockaddr_in &server_addr);
private:
    void handle_post_request(int client_socket, const std::string &file_name, long file_len);

    void handle_get_request(int client_socket, char *req_buf);

    void write_success_header(int client_socket, long data);

    void write_fail_header(int client_socket);

    void handle_tcp_client(int client_socket, sockaddr_in client_addr);

    int child_count = 0;

    pid_t process_id = 0;
};


#endif //TCP_SERVER_TCPSERVER_H
