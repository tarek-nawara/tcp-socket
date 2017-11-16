/**
 * File: TcpServer.h
 * Description: Implementation of TCP server
 * Created Date: 2017-11-16
 */

#include "TcpServer.h"

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
    std::cout << "---Request comes from IP: " << client_addr_ip << std::endl;
    ssize_t request_size = utils::read_wrapper(client_socket, req_buf, MAXLINE);

    if (request_size == 0) {
        close(client_socket);
        return;
    }

    req_buf[request_size] = '\0';
    std::cout << "---Request body " << req_buf << std::endl;
    resolve_request(req_buf, (char *) url_buf, (char *) version_buf);

    std::cout << "---URL " << url_buf << " version " << version_buf << std::endl;
    std::cout << "---Requested file path: " << url_buf << std::endl;

    const char *notfound_str = "404 File Not Found";

    auto pair = open_file(url_buf, resp_buf, version_buf);
    int file_des = pair.first;
    bool read_ok = pair.second;


    utils::write_wrapper(client_socket, resp_buf, strlen(resp_buf));

    if (read_ok) {
        resolve_success_response(client_socket, file_des);
    } else {
        utils::write_wrapper(client_socket, notfound_str, strlen(notfound_str));
    }

    std::cout << "---Response sent" << std::endl;
    if (read_ok)
        close(file_des);
    close(client_socket);
    std::cout << "---Connection with IP" << client_addr_ip << " closed" << std::endl;
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
