#include <stdio.h> /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h> /* for sockaddr_in and inet_addr() */
#include <stdlib.h> /* for atoi() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for close() */
#include "socket_utils.h"
#include "TcpClient.h"

#define RCVBUFSIZE 32

/* Size of receive buffer */
/* Error handling function */
int main(int argc, char *argv[])
{
    TcpClient *client = new TcpClient();
	
}

