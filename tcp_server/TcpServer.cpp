/**
 * File: TcpServer.h
 * Description: Implementation of TCP server
 * Created Date: 2017-11-16
 */

#include <fstream>
#include <sstream>
#include "TcpServer.h"

#define RCVBUFSIZE 32

void
TcpServer::handle_tcp_client(int server_socket, struct sockaddr_in &server_addr, struct sockaddr_in client_addr) {
    char client_addr_ip[INET_ADDRSTRLEN];
    auto *req_buf = new char[MAXLINE + 1];

    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = utils::accept_wrapper(server_socket, (struct sockaddr *) &client_addr,
                                              &client_addr_len);

    utils::inet_ntop_wrapper(AF_INET, &client_addr.sin_addr, client_addr_ip, INET_ADDRSTRLEN);
    std::cout << "[handle_tcp_client]---Request comes from IP: " << client_addr_ip << std::endl;
    ssize_t request_size = utils::read_wrapper(client_socket, req_buf, MAXLINE);

    if (request_size == 0) {
        close(client_socket);
        return;
    }

    req_buf[request_size] = '\0';
    std::cout << "[handle_tcp_client]---Request body " << req_buf << std::endl;

    if (req_buf[0] == 'G') {
        handle_get_request(client_socket, req_buf);
    } else if (req_buf[0] == 'P') {
        char file_name[20];
        long file_len;
        sscanf(req_buf, "POST %s HTTP/1.1\r\nHOST: %*s\r\nContent-Length: %ld\r\n\r\n", file_name, &file_len);
        std::cout << "[handle_tcp_client]---Request=" << req_buf << std::endl;
        std::cout << "[handle_tcp_client]---POST file name=" << file_name << " file length=" << file_len << std::endl;
        handle_post_request(client_socket, file_name, file_len);
    } else {
        std::cout << "[handle_tcp_client]---Error unsupported HTTP method, ignoring" << std::endl;
        std::cout << "[handle_tcp_client]---Connection with IP" << client_addr_ip << " closed" << std::endl;
    }
}

void
TcpServer::handle_get_request(int client_socket, char *req_buf) {
    char file_name[20];
    sscanf(req_buf, "GET %s HTTP/1.1", file_name);
    std::cout << "[handle_get_request]---Sending the GET file" << std::endl;
    std::cout << "[handle_get_request]---File name=" << file_name << std::endl;

    FILE *pFile;
    if ((pFile = fopen(file_name, "r"))) {
        std::cout << "File found, start sending the file" << std::endl;
        long remain_data = file_size(file_name);
        write_success_header(client_socket, remain_data);
        off_t offset = 0;
        ssize_t sent_bytes;
        while (((sent_bytes = sendfile(client_socket, fileno(pFile), &offset, BUFSIZ)) > 0) && (remain_data > 0)) {
            remain_data -= sent_bytes;
        }
    } else {
        std::cout << "File not found" << std::endl;
        write_fail_header(client_socket);
    }
    utils::close_wrapper(client_socket);
}

void
TcpServer::handle_post_request(int client_socket, const std::string &file_name, long file_len) {
    std::cout << "[handle_post_request]---Start sending ack" << std::endl;
    /* Sending ack */
    std::string ack_msg = "HTTP/1.1 200 OK\r\n";
    utils::send_wrapper(client_socket, ack_msg.c_str(), ack_msg.length(), 0);

    char rcv_buf[100];
    long total_byte_rcv = 0;
    bool first_write = true;
    std::cout << "[handle_post_request]---Start receiving POST file" << std::endl;
    while (total_byte_rcv < file_len) {
        ssize_t byte_rcv = utils::recv_wrapper(client_socket, rcv_buf, RCVBUFSIZE - 1, 0);
        total_byte_rcv += byte_rcv;
        rcv_buf[byte_rcv] = '\0';
        if (first_write) {
           write_to_file(file_name, rcv_buf, byte_rcv);
        } else {
            append_to_file(file_name, rcv_buf, byte_rcv);
        }
        first_write = false;
    }
    utils::close_wrapper(client_socket);
}

void
TcpServer::write_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv) {
    std::ofstream outfile (file_name);
    outfile.write(rcv_buf, byte_rcv);
    outfile.close();
}

void
TcpServer::append_to_file(const std::string &file_name, char *rcv_buf, ssize_t byte_rcv) {
    std::ofstream outfile;
    outfile.open(file_name, std::ios_base::app);
    outfile.write(rcv_buf, byte_rcv);
    outfile.close();
}

long
TcpServer::file_size(const char *file_name) {
    struct stat stat_buf;
    int rc = stat(file_name, &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}

// TODO check if will work
void
TcpServer::write_success_header(int client_socket, long data) {
    std::string header = "HTTP/1.1 200 OK\r\nContent-Length:";
    header += std::to_string(data);
    header += "\r\n\r\n";
    utils::write_wrapper(client_socket, header.c_str(), header.size());
}

void
TcpServer::write_fail_header(int client_socket) {
    std::string header = "HTTP/1.1 404 Not Found\r\n\r\n";
    utils::write_wrapper(client_socket, header.c_str(), header.size());
}
