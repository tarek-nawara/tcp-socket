/**
 * File: socket_utils.cpp
 * Description: implementation for socket wrapper functions
 * Created Date: 2017-11-15
 */

#include "socket_utils.h"


int socket_wrapper(int family, int type, int protocol) {
    int sock = socket(family, type, protocol) < 0;
    if (sock) {
        die_with_error("socket() failed");
    }
    return sock;
}

void connect_wrapper(int socket, const struct sockaddr *foreign_addr, socklen_t addr_len) {
    if (connect(socket, foreign_addr, addr_len) < 0) {
        die_with_error("connect() failed");
    }
}

ssize_t send_wrapper(int socket,
                     const void *msg,
                     size_t msg_len,
                     int flags,
                     const struct sockaddr *foreign_addr,
                     socklen_t addr_len) {
    if (sendto(socket, msg, msg_len, flags, foreign_addr, addr_len) != msg_len) {
        die_with_error("send() sent different number of bytes than expected");
    }
}

ssize_t rcv_wrapper(int socket,
                    void *rcv_buffer,
                    size_t buffer_len,
                    int flags,
                    struct sockaddr *foreign_addr,
                    socklen_t *addr_len) {
    ssize_t total_rcv = recvfrom(socket, rcv_buffer, buffer_len, flags, foreign_addr, addr_len);
    if (total_rcv < 0) {
        die_with_error("recv() failed or connection closed  prematurely");
    }
    return total_rcv;
}

void bind_wrapper(int socket, const struct sockaddr *local_addr, socklen_t addr_len) {
    if (bind(socket, local_addr, addr_len) < 0) {
        die_with_error("bind() failed");
    }
}

void listen_wrapper(int socket, int queue_limit) {
    if (listen(socket, queue_limit) < 0) {
        die_with_error("listen() failed");
    }
}

int accept_wrapper(int socket, struct sockaddr *client_addr, socklen_t *addr_len) {
    int client_socket = accept(socket, client_addr, addr_len);
    if (client_socket < 0) {
        die_with_error("accept() failed");
    }
    return client_socket;
}

void close_wrapper(int socket) {
    if (close(socket) < 0) {
        die_with_error("close() failed");
    }
}


void die_with_error(const char *error_msg) {
    perror(error_msg);
    exit(1);
}

