/**
 * File: TcpClient.cpp
 * Description: implementation of the TCP client
 * Created Date: 2017-11-15
 */

#include "TcpClient.h"
using namespace utils;

void TcpClient::send_get_request(char* host_name, string file_name){
	struct sockaddr_in echoServAddr;
	char* echoString;
	int sock = socket_wrapper();
	memset(&echoServAddr, 0, sizeof(echoServAddr)); /* Zero out structure */
    echoServAddr.sin_family = AF_INET; /* Internet address family */
    echoServAddr.sin_addr.s_addr = inet_addr(ip_address); /* Server IP address */
    echoServAddr.sin_port = htons(port_number); /* Server port */
    /* Establish the connection to the echo server */
   	connect_wrapper(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr));
	echoString = new char[200];
	int echoStringLen = sprintf(echoString,"GET /%s HTTP/1.1\r\nHOST: %s:%d\r\n\r\n",file_name.c_str(), host_name, port_number);
	send_wrapper(sock, echoString, echoStringLen, 0);
	int totalBytesRcvd = 0;
    printf("Received: "); /* Setup to print the echoed string */
    while (totalBytesRcvd < echoStringLen)
    {
        /* Receive up to the buffer size (minus i to leave space for
        a null terminator) bytes from the sender */
        int bytesRcvd = recv_wrapper(sock, echoBuffer, RCVBUFSIZE - 1, 0);
        totalBytesRcvd += bytesRcvd; /* Keep tally of total bytes */
        echoBuffer[bytesRcvd] = '\0'; /* Terminate the string! */
        printf(echoBuffer); /* Print the echo buffer */
    }
    printf("\n"); /* Print a final linefeed */
    close(sock);
}

void TcpClient::send_post_request(char* host_name, string file_path){

}
