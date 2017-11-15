/**
 * File: socket_utils.h
 * Description: socket wrapper methods
 * Created Date: 2017-11-15
 */

#ifndef SOCKET_PROGRAMMING_SOCKET_UTILS_H
#define SOCKET_PROGRAMMING_SOCKET_UTILS_H

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

/**
 * Creating a socket.
 *
 * @param family   address family (eg. AF_INET)
 * @param type     determine socket semantics (eg. SOCK_STREAM for reliable stream)
 * @param protocol end-to-end protocol (eg. IPPROTO_TCP for TCP)
 * @return         -1 for failure, or socket number for success
 */
int socket_wrapper(int family, int type, int protocol);

/**
 * Establish connection with the given address.
 *
 * @param socket         socket number.
 * @param foreignAddress pointer to sockaddr containing internet address and port of the server
 * @param address_len    length of the address structure = {@code sizeof(struct sockaddr_in)}
 */
void connect_wrapper(int socket, const struct sockaddr *foreign_addr, socklen_t addr_len);

/**
 * Send a message to the given address.
 *
 * @param socket       socket number
 * @param msg          pointer on the message to send
 * @param msg_len      length of the message to send
 * @param flags        way to change the default behavior
 * @param foreign_addr pointer to sockaddr containing internet address and port of the server
 * @param addr_len     length of the address structure = {@code sizeof(struct sockaddr_in)}
 * @return             -1 for failure, length of sent message for success
 */
ssize_t send_wrapper(int socket,
                     const void *msg,
                     size_t msg_len,
                     int flags,
                     const struct sockaddr *foreign_addr,
                     socklen_t addr_len);

/**
 * Receive message from the given address.
 *
 * @param socket       socket number
 * @param rcv_buffer   pointer on rcv buffer
 * @param buffer_len   length of the rcv buffer
 * @param flags        way to change the default behavior
 * @param foreign_addr pointer to sockaddr containing internet address and port of the server
 * @param addr_len     length of the address structure = {@code sizeof(struct sockaddr_in)}
 * @return             -1 for failure, length of sent message for success
 */
ssize_t rcv_wrapper(int socket,
                    void *rcv_buffer,
                    size_t buffer_len,
                    int flags,
                    struct sockaddr *foreign_addr,
                    socklen_t *addr_len);

/**
 * Log an error message and terminate program execution.
 *
 * @param error_msg error message to log
 */
void die_with_error(const char *error_msg);

#endif //SOCKET_PROGRAMMING_SOCKET_UTILS_H
