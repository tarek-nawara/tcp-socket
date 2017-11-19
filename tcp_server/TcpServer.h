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

	/* Handle all requests by forking different processes for
	   each request */
    void handle_tcp_client_fork(int server_socket, sockaddr_in &server_addr);

private:

	/* Handle a client request on the given socket */
    void handle_tcp_client(int client_socket, sockaddr_in client_addr);

	/* Handle a client's single request, this method exist because the
	   Client can reuse the same socket for sending multiple requests
	   this method will handle only a single requst */
    bool handle_single_request(int client_socket, const char *client_addr_ip);


	/* Handle client POST requests comming on the given client_socket */
	void handle_post_request(int client_socket, const std::string &file_name, long file_len);

	/* Handle client GET requests comming on the given client_socket */
    void handle_get_request(int client_socket, char *req_buf);

	/* Write on the given client socket the success header
	   HTTP/1.1 200 OK */
    void write_success_header(int client_socket, long content_len);

	/* Write on the given client socket the failure header
	   HTTP/1.1 404 File Not Found */
    void write_fail_header(int client_socket);

	/* Sets the connection timeout on the given
	   client socket */
    int set_connection_time_out(int client_socket);

	/* Calculate the timeout for a socket dynamically
	   based on the number of different opend connections */
    int calculate_time_out();

	static int *child_count;

    pid_t process_id = 0;
};


#endif //TCP_SERVER_TCPSERVER_H
