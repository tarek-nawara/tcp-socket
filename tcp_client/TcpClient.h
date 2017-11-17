/**
 * File: TcpClient.h
 * Description: Representation for the TCP client
 * Created Date: 2017-11-15
 */
#include "socket_utils.h"
#include <iostream>

#ifndef SOCKET_PROGRAMMING_TCPCLIENT_H
#define SOCKET_PROGRAMMING_TCPCLIENT_H
#define RCVBUFSIZE 32

using namespace std;
class TcpClient {
	private:
		string ip_address;
		int port_number;
	public:
		TcpClient(string ip_address, int port_number){
			this->ip_address = ip_address;
			this->port_number = port_number;
		}
		void send_get_request(char* host_name, string file_name);
		void send_post_request(char* host_name, string file_path);
};


#endif //SOCKET_PROGRAMMING_TCPCLIENT_H
