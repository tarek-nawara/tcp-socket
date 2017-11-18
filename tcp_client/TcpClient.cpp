/**
 * File: TcpClient.cpp
 * Description: implementation of the TCP client
 * Created Date: 2017-11-15
 */

#include "TcpClient.h"

TcpClient::TcpClient(string ip_address, int port_number) {
    this->ip_address = std::move(ip_address);
    this->port_number = port_number;
}

void
TcpClient::send_get_request(char *host_name, string file_name) {
    struct sockaddr_in server_addr;
    char *req_buf;
    int sock = socket_wrapper();
    init_server_addr(server_addr);

    /* Start sending */
    connect_wrapper(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    req_buf = new char[200];
    int req_buf_len = sprintf(req_buf, "GET %s HTTP/1.1\r\nHOST: %s:%d\r\n\r\n", file_name.c_str(), host_name,
                              port_number);
    send_wrapper(sock, req_buf, req_buf_len, 0);

    cout << "Received: ";
    char rcv_buf[100];
    int total_byte_rcv = 0;
    auto *header = read_header(sock, file_name);
    if ((*header)[0] != 200) {
        cout << "Get request failed to retrieve file" << endl;
        cout << "HTTP status: " << (*header)[0] << endl;
        return;
    }
    bool first_write = true;
    while (total_byte_rcv < (*header)[1]) {
        ssize_t byte_rcv = recv_wrapper(sock, rcv_buf, RCVBUFSIZE - 1, 0);
        total_byte_rcv += byte_rcv;
        rcv_buf[byte_rcv] = '\0';
        printf("%s", rcv_buf);
        if (first_write) {
            write_to_file(file_name, rcv_buf, byte_rcv);
        } else {
            append_to_file(file_name, rcv_buf, byte_rcv);
        }
        first_write = false;
    }
    printf("\n");
    close(sock);
}

void
TcpClient::init_server_addr(sockaddr_in &server_addr) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server_addr.sin_port = htons(port_number);
}

void
TcpClient::send_post_request(char *host_name, string file_name) {
    struct sockaddr_in server_addr;
    char *req_buf;
    int sock = socket_wrapper();
    init_server_addr(server_addr);

    /* Start sending */
    connect_wrapper(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));
    req_buf = new char[200];
    long file_len = file_size(file_name);
    int req_buf_len = sprintf(req_buf, "POST %s HTTP/1.1\r\nHOST: %s\r\nContent-Length: %ld\r\n\r\n", file_name.c_str(),
                              host_name, file_len);
    cout << "file_len=" << file_len << endl;
    send_wrapper(sock, req_buf, req_buf_len, 0);

    bool ack = rcv_ack(sock);
    if (ack) {
        resolve_post_file(sock, file_name);
    } else {
        cout << "failed to send POST request" << endl;
    }
}

bool
TcpClient::rcv_ack(int socket) {
    auto *resp_buf = new char[100];
    recv(socket, resp_buf, 100, 0); /* make sure to get the exact response */
    int resp_status;
    sscanf(resp_buf, "HTTP/1.1 %d OK\r\n", &resp_status);
    cout << "---Response on POST request status: " << resp_status << endl;

    /* scanning response to learn its type */
    delete[] resp_buf;
    return resp_status == 200;
}

void
TcpClient::resolve_post_file(int sock, string file_name) {
    cout << "[resolve_post_file]---Uploading the POST file" << endl;

    FILE *pFile = fopen(file_name.c_str(), "r");
    long remain_data = file_size(file_name);
    off_t offset = 0;
    ssize_t sent_bytes;
    while (((sent_bytes = sendfile(sock, fileno(pFile), &offset, BUFSIZ)) > 0) && (remain_data > 0)) {
        remain_data -= sent_bytes;
    }
}

vector<int> *
TcpClient::read_header(int socket, const string &file_name) {
    bool end_of_header = false;
    string header = "";
    char rcv_buf[100];

    auto *res = new vector<int>();
    while (!end_of_header) {
        ssize_t byte_rcv = recv_wrapper(socket, rcv_buf, 1, 0);
        rcv_buf[byte_rcv] = '\0';
        string token(rcv_buf);
        header += token;
        size_t found = header.find("\r\n\r\n");
        if (found != string::npos) {
            end_of_header = true;
            res = parse_header(header.substr(0, header.length() - 4));
        }
    }
    return res;
}

vector<int> *
TcpClient::parse_header(string header) {
    int resp_status;
    int content_len = 0;
    auto *res = new vector<int>();
    auto *split_strs = split_header(header);
    sscanf((*split_strs)[STATUS_INDEX].c_str(), "HTTP/1.1 %d", &resp_status);
    res->push_back(resp_status);
    if (resp_status == 200) {
        sscanf((*split_strs)[CONTENT_INDEX].c_str(), "Content-Length: %d", &content_len);
    }
    res->push_back(content_len);
    return res;
}

vector<string> *
TcpClient::split_header(string headers) {
    istringstream MyStream(headers);
    vector<string> *v = new vector<string>();
    string s;
    while (getline(MyStream, s)) {
        v->push_back(s);
    }
    return v;
}