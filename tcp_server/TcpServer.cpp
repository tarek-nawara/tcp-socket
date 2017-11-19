/**
 * File: TcpServer.h
 * Description: Implementation of TCP server
 * Created Date: 2017-11-16
 */

#include "TcpServer.h"

int *TcpServer::child_count = static_cast<int *>(mmap(NULL, sizeof *child_count, PROT_READ | PROT_WRITE,
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
        handle_tcp_client(client_socket, client_addr);
        exit(0);
    }

    std::cout << "[handle_tcp_client_fork]---With child process: " << process_id << std::endl;
    std::cout << "[handle_tcp_client_fork]---Number of children: " << (*child_count) << std::endl;
    ++(*child_count);
    while (*child_count > 0) {
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
    bool client_closed_con = false;
    char client_addr_ip[INET_ADDRSTRLEN];
    while (!client_closed_con) {
        utils::inet_ntop_wrapper(AF_INET, &client_addr.sin_addr, client_addr_ip, INET_ADDRSTRLEN);
        std::cout << "[handle_tcp_client]---Request comes from IP: " << client_addr_ip << std::endl;
        int con_timeout_res = set_connection_time_out(client_socket);
        if (con_timeout_res == -1) {
            std::cout << "[handle_tcp_client]---Select error happened" << std::endl;
            utils::die_with_error("[handle_tcp_client]--set_connection_time_out() failed");
            return;
        } else if (con_timeout_res == 0) {
            std::cout << "[handle_tcp_client]---Connection closed due to timeout" << std::endl;
            utils::close_wrapper(client_socket);
            return;
        } else {
            client_closed_con = handle_single_request(client_socket, client_addr_ip);
        }
    }
    utils::close_wrapper(client_socket);
}

bool
TcpServer::handle_single_request(int client_socket, const char *client_addr_ip) {
    auto *req_buf = new char[MAXLINE + 1];
    ssize_t request_size = utils::recv_wrapper(client_socket, req_buf, MAXLINE, 0);

    if (request_size <= 0) {
        std::cout << "[handle_single_request]---Client closed connection" << std::endl;
        delete[] req_buf;
        return true;
    }

    req_buf[request_size] = '\0';
    std::cout << "[handle_single_request]---Request body:\"" << req_buf << "\"" << std::endl;
    if (req_buf[0] == 'G') {
        handle_get_request(client_socket, req_buf);
        std::cout << "[handle_single_request]---Finished serving GET request" << std::endl;
    } else if (req_buf[0] == 'P') {
        char file_name[20];
        long file_len;
        sscanf(req_buf, "POST %s HTTP/1.1\r\nHOST: %*s\r\nContent-Length: %ld\r\n\r\n", file_name, &file_len);
        std::cout << "[handle_tcp_client]---POST file name=" << file_name << " file length=" << file_len
                  << std::endl;
        handle_post_request(client_socket, file_name, file_len);
        std::cout << "[handle_single_request]---Finished serving POST request" << std::endl;
    } else {
        std::cout << "[handle_single_request]---Error unsupported HTTP method, ignoring" << std::endl;
        std::cout << "[handle_single_request]---Connection with IP" << client_addr_ip << " closed" << std::endl;
    }
    delete[] req_buf;
    return false;
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

int
TcpServer::set_connection_time_out(int client_socket) {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(client_socket, &set);
    struct timeval timeout = {calculate_time_out(), 0};
    return select(client_socket + 1, &set, NULL, NULL, &timeout);
}

int
TcpServer::calculate_time_out() {

    return 2 + (MAX_WAIT_TIME / (*child_count + 1));
}

void
TcpServer::write_success_header(int client_socket, long content_len) {
    std::string header = "HTTP/1.1 200 OK\r\nContent-Length: ";
    header += std::to_string(content_len);
    header += "\r\n\r\n";
    utils::write_wrapper(client_socket, header.c_str(), header.size());
}

void
TcpServer::write_fail_header(int client_socket) {
    std::string header = "HTTP/1.1 404 Not Found\r\n\r\n";
    utils::write_wrapper(client_socket, header.c_str(), header.size());
}
