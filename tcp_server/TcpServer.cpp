/**
 * File: TcpServer.h
 * Description: Implementation of TCP server
 * Created Date: 2017-11-16
 */

#include <sys/mman.h>
#include "TcpServer.h"

#define RCVBUFSIZE 32

int * TcpServer::child_count = static_cast<int *>(mmap(NULL, sizeof *child_count, PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0));

void
TcpServer::handle_tcp_client_fork(int server_socket, struct sockaddr_in &server_addr) {
    struct sockaddr_in client_addr;

    socklen_t client_addr_len = sizeof(client_addr);
    int client_socket = utils::accept_wrapper(server_socket, (struct sockaddr *) &client_addr,
                                              &client_addr_len);
    process_id = utils::fork_wrapper();
    if (process_id == 0) {
        std::cout << "[handle_tcp_client_fork]---Start serving the request from the child" << std::endl;

        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        handle_tcp_client(client_socket, client_addr);
        exit(0);
    }

    std::cout << "[handle_tcp_client_fork]---With child process: " << process_id << std::endl;
    std::cout << "[handle_tcp_client_fork]---Number of children: " << (*child_count) << std::endl;
    ++(*child_count);
    while (child_count > 0) {
        process_id = utils::waitpid_wrapper();
        if (process_id == 0) {
            break;
        } else {
            std::cout << "[handle_tcp_client_fork]---Closing child with process id: " << process_id << std::endl;
            --(*child_count);
        }
    }
}

void
TcpServer::handle_tcp_client(int client_socket, struct sockaddr_in client_addr) {
    while (true) {
        char client_addr_ip[INET_ADDRSTRLEN];
        auto *req_buf = new char[MAXLINE + 1];

        utils::inet_ntop_wrapper(AF_INET, &client_addr.sin_addr, client_addr_ip, INET_ADDRSTRLEN);
        std::cout << "[handle_tcp_client]---Request comes from IP: " << client_addr_ip << std::endl;
        ssize_t request_size = utils::recv_wrapper(client_socket, req_buf, MAXLINE, 0);

        req_buf[request_size] = '\0';
        std::cout << "[handle_tcp_client]---Request body " << req_buf << std::endl;

        if (req_buf[0] == 'G') {
            handle_get_request(client_socket, req_buf);
            std::cout << "[handle_tcp_client]---Finished serving GET request" << std::endl;
        } else if (req_buf[0] == 'P') {
            char file_name[20];
            long file_len;
            sscanf(req_buf, "POST %s HTTP/1.1\r\nHOST: %*s\r\nContent-Length: %ld\r\n\r\n", file_name, &file_len);
            std::cout << "[handle_tcp_client]---POST file name=" << file_name << " file length=" << file_len
                      << std::endl;
            handle_post_request(client_socket, file_name, file_len);
            std::cout << "[handle_tcp_client]---Finished serving POST request" << std::endl;
        } else {
            std::cout << "[handle_tcp_client]---Error unsupported HTTP method, ignoring" << std::endl;
            std::cout << "[handle_tcp_client]---Connection with IP" << client_addr_ip << " closed" << std::endl;
        }
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
        long remain_data = utils::file_size(file_name);
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
           utils::write_to_file(file_name, rcv_buf, byte_rcv);
        } else {
            utils::append_to_file(file_name, rcv_buf, byte_rcv);
        }
        first_write = false;
    }
}

void
TcpServer::write_success_header(int client_socket, long data) {
    std::string header = "HTTP/1.1 200 OK\r\nContent-Length: ";
    header += std::to_string(data);
    header += "\r\n\r\n";
    utils::write_wrapper(client_socket, header.c_str(), header.size());
}

void
TcpServer::write_fail_header(int client_socket) {
    std::string header = "HTTP/1.1 404 Not Found\r\n\r\n";
    utils::write_wrapper(client_socket, header.c_str(), header.size());
}
