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
    char url_buf[MEDIUMLINE + 1];
    char version_buf[MINILINE + 1];
    char client_addr_ip[INET_ADDRSTRLEN];
    auto *req_buf = new char[MAXLINE + 1];
    auto *resp_buf = new char[MAXLINE + 1];

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
//    resolve_request(req_buf, (char *) url_buf, (char *) version_buf);
//    std::cout << "[handle_tcp_client]---URL " << url_buf << " version " << version_buf << std::endl;
//    std::cout << "[handle_tcp_client]---Requested file path: " << url_buf << std::endl;

    if (req_buf[0] == 'G') {
        handle_get_request(client_socket);
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
TcpServer::resolve_success_response(int client_socket, int file_des) const {
    ssize_t read_size;
    void *read_buf = new char[MAXLINE + 1];
    while ((read_size = utils::read_wrapper(file_des, read_buf, MAXLINE)) > 0)
        utils::write_wrapper(client_socket, read_buf, read_size);
}


std::pair<int, bool>
TcpServer::open_file(char *url_buf,
                     char *resp_buf,
                     char *version_buf) const {
    std::string status_code, phrase;
    int file_des = open(url_buf, O_RDONLY);
    bool read_ok;
    if (file_des < 0) {
        status_code = "404";
        phrase = "Not Found";
        read_ok = false;
        std::cout << "---File " << url_buf << " not found, generate 404 Not Found response" << std::endl;
    } else {
        status_code = "200";
        phrase = "OK";
        read_ok = true;
        std::cout << "---File " << url_buf << " found, generate 200 Found response" << std::endl;
    }
    sprintf(resp_buf, "%s %s %s\r\nConnection: closed\r\n\r\n", version_buf, status_code, phrase);
    return {file_des, read_ok};
}


void
TcpServer::resolve_request(const char *req_buf, char *url_buf, char *version_buf) {
    const char *pos, *pos2;

    /* copy url */
    pos = strchr(req_buf, (int) ' ');
    pos2 = strchr(pos + 1, (int) ' ');
    strncpy(url_buf, pos + 1, pos2 - pos - 1);
    url_buf[pos2 - pos - 1] = '\0';

    /* copy version */
    pos = strchr(pos2 + 1, (int) '\r');
    strncpy(version_buf, pos2 + 1, pos - pos2 - 1);
    version_buf[pos - pos2 - 1] = '\0';
}

void TcpServer::handle_get_request(int client_socket) {
    //auto pair = open_file(url_buf, resp_buf, version_buf);
    //int file_des = pair.first;
    //bool read_ok = pair.second;
    const char *echo_str = "hello world";
    const char *notfound_str = "404 File Not Found";


//    utils::write_wrapper(client_socket, resp_buf, strlen(resp_buf));
//
//    if (read_ok) {
//        resolve_success_response(client_socket, file_des);
//    } else {
//        utils::write_wrapper(client_socket, notfound_str, strlen(notfound_str));
//    }

    utils::write_wrapper(client_socket, echo_str, strlen(echo_str));

    std::cout << "---Response sent" << std::endl;
//    if (read_ok)
//        close(file_des);
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
