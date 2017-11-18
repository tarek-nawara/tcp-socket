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
	/* Constructor */
    explicit TcpClient(int port_number);

	/* Send a get request for a host listening on the given socket and
	   requesting the given file */
    void send_get_request(char *host_name, string file_name, int server_socket);

	/* Send a POST request to a host listening on the given socket
	   and uploading the given form */
    void send_post_request(char *host_name, string file_name, int server_socket);

private:
    int port_number;

	/* wait for acknowledge from the server after sending a 
	   POST request */
    bool rcv_ack(int socket);

	/* Upload the given file as a form to a host listening
	   on the given socket */
    void resolve_post_file(int sock, string file_name);

	/* Get the GET response header to get the Content-length */
    vector<int> * read_header(int socket);

	/* Split the headers on the \r\n character */
    vector<string> *split_header(string headers);
	
	/* Extract the response status and content-length
	   if found from the response header */
    vector<int> * parse_header(string header);

};


#endif //SOCKET_PROGRAMMING_TCPCLIENT_H
