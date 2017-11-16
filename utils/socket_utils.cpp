/**
 * File: socket_utils.cpp
 * Description: implementation for socket wrapper functions
 * Created Date: 2017-11-15
 */

#include "socket_utils.h"

namespace utils {
    int socket_wrapper() {
        int server_socket;
        if ((server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            utils::die_with_error("failed");
        }
        return server_socket;
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

    int inet_pton_wrapper(int family, const char *strptr, struct in_addr *addrptr) {
        if (inet_pton(family, strptr, addrptr) != 1) {
            die_with_error("inet_pton() failed");
        }
        return 1;
    }

    const char *inet_ntop_wrapper(int family, const void *addrptr, char *strptr, socklen_t len) {
        if (inet_ntop(family, addrptr, strptr, len) == nullptr) {
            die_with_error("inet_ntop error");
        }
        return strptr;
    }

    ssize_t read_wrapper(int filedes, void *buf, size_t nbytes) {
        ssize_t n = read(filedes, buf, nbytes);
        if (n < 0) {
            die_with_error("read error");
        }

        return n;
    }

    ssize_t write_wrapper(int filedes, const void *buf, size_t nbytes) {
        if (write(filedes, buf, nbytes) != nbytes) {
            die_with_error("write() error");
        }

        return nbytes;
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
}

