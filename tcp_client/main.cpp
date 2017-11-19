/**
 * File: main.cpp
 * Description: entry point of the client
 * Created Date: 2017-11-15
 */

#include "socket_utils.h"
#include "TcpClient.h"

using namespace std;

/* Initialize the server address struct by setting
   the server IP and port number */
void init_server_addr(sockaddr_in &server_addr, string ip_address, int port_number);

/* Read the client waiting time from a file named
   `client-wait.txt` */
int get_wait_time();

int main(int argc, char *argv[]) {
    if (argc > 4 || argc < 2) {
        utils::die_with_error("Invalid command line arguments");
    }
    /* Server socket creation */
    struct sockaddr_in server_addr;
    int server_socket = socket_wrapper();
    int port_number = argc == 3 ? atoi(argv[2]) : 80;
    init_server_addr(server_addr, argv[1], port_number);
    connect_wrapper(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));

    TcpClient *client = new TcpClient(port_number);
    fstream infile;
    infile.open ("client-input.txt");
    string line;
    while (!infile.eof()) {
        getline(infile, line);
        line = utils::trim(line);
        if (line.empty()) {
            continue;
        }
        cout << line << endl;
        char file_name[20];
        char hostname[20];
        sscanf(line.c_str(), "%*s %s %s", file_name, hostname);
        if (line[0] == 'G') {
            client->send_get_request(hostname, file_name, server_socket);
        } else if (line[0] == 'P') {
            client->send_post_request(hostname, file_name, server_socket);
        } else {
            cerr << "[TcpClient::main] Unsupported HTTP method" << endl;
        }
    }

    utils::close_wrapper(server_socket);
    /* Optional sleeping */
    std::this_thread::sleep_for(std::chrono::milliseconds(get_wait_time() * 1000));
    return 0;
}

int get_wait_time() {
    fstream infile;
    infile.open("client-wait.txt");
    int wait_time;
    infile >> wait_time;
    return wait_time;
}

void init_server_addr(sockaddr_in &server_addr, string ip_address, int port_number) {
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_address.c_str());
    server_addr.sin_port = htons(port_number);
}

