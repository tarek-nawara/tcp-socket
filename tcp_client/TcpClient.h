/**
 * File: TcpClient.h
 * Description: Representation for the TCP client
 * Created Date: 2017-11-15
 */

#ifndef SOCKET_PROGRAMMING_TCPCLIENT_H
#define SOCKET_PROGRAMMING_TCPCLIENT_H

#include "socket_utils.h"

#define RCVBUFSIZE 32
#define STATUS_INDEX 0
#define CONTENT_INDEX 1

using namespace std;
using namespace utils;

class TcpClient {
public:
    explicit TcpClient(int port_number);

    void send_get_request(char *host_name, string file_name, int server_socket);

    void send_post_request(char *host_name, string file_name, int server_socket);

private:
    int port_number;

    bool rcv_ack(int socket);

    void resolve_post_file(int sock, string file_name);


    vector<int> * read_header(int socket, const string &file_name);

    vector<string> *split_header(string headers);

    vector<int> * parse_header(string header);

};


#endif //SOCKET_PROGRAMMING_TCPCLIENT_H
