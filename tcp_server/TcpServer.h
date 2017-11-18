/**
 * File: TcpServer.h
 * Description: Holder for TCP server interface
 * Created Date: 2017-11-16
 */

#ifndef TCP_SERVER_TCPSERVER_H
#define TCP_SERVER_TCPSERVER_H

#include "socket_utils.h"

#define RCVBUFSIZE 32
#define MAX_WAIT_TIME 10

class TcpServer {
public:
    void handle_tcp_client_fork(int server_socket, sockaddr_in &server_addr);
private:
    void handle_post_request(int client_socket, const std::string &file_name, long file_len);

    void handle_get_request(int client_socket, char *req_buf);

    void write_success_header(int client_socket, long data);

    void write_fail_header(int client_socket);

    void handle_tcp_client(int client_socket, sockaddr_in client_addr);

    static int *child_count;

    pid_t process_id = 0;

    bool handle_single_request(int client_socket, const char *client_addr_ip);

    int set_connection_time_out(int client_socket);

    int calculate_time_out();
};


#endif //TCP_SERVER_TCPSERVER_H
