/**
 * File: TcpClient.h
 * Description: Representation for the TCP client
 * Created Date: 2017-11-15
 */

#include "socket_utils.h"
#include <iostream>
#include <utility>
#include <cstdio>
#include <sys/sendfile.h>

#ifndef SOCKET_PROGRAMMING_TCPCLIENT_H
#define SOCKET_PROGRAMMING_TCPCLIENT_H
#define RCVBUFSIZE 32
#define STATUS_INDEX 0
#define CONTENT_INDEX 1

using namespace std;

class TcpClient {
public:
    TcpClient(string ip_address, int port_number);

    void send_get_request(char *host_name, string file_name);

    void send_post_request(char *host_name, string file_path);

private:
    string ip_address;
    int port_number;

    void append_to_file(const string &file_name, char *rcv_buf, ssize_t byte_rcv);

    long file_size(const string &filename);

    bool rcv_ack(int socket);

    void resolve_post_file(int sock, string file_name);

    void init_server_addr(sockaddr_in &server_addr);

    vector<int> * read_header(int socket, const string &file_name);

    vector<string> *split_header(string headers);

    vector<int> * parse_header(string header);
};


#endif //SOCKET_PROGRAMMING_TCPCLIENT_H
