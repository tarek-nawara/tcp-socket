#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "socket_utils.h"
#include "TcpClient.h"

#define RCVBUFSIZE 32

using namespace std;

int main(int argc, char *argv[]) {
    if (argc > 3 || argc < 2) {
        utils::die_with_error("Invalid command line arguments");
    }
    TcpClient *client = new TcpClient(argv[1], argc == 3 ? atoi(argv[2]) : 80);
    fstream infile;
    infile.open ("client-input.txt");
    string line;
    while (!infile.eof()) {
        getline(infile, line);
        cout << line << endl;
        char file_name[20];
        char hostname[20];
        sscanf(line.c_str(), "%*s %s %s", file_name, hostname);
        if (line[0] == 'G') {
            client->send_get_request(hostname, file_name);
        } else if (line[0] == 'P') {
            client->send_post_request(hostname, file_name);
        } else {
            cerr << "[TcpClient::main] Unsupported HTTP method" << endl;
        }
    }
    return 0;
}

