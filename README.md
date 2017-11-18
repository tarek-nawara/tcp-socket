# socket-programming
Simple Implementation for a webserver using Unix socket API

### How to build
In each tcp_server, tcp_client and utils create a folder called ``cmake-build-debug``
then put both ``client-input.txt`` and ``client-wait.txt`` inside ``tcp_client/cmake-build-debug``.
#### Building order
first build ``utils`` then build both ``tcp_client`` and ``tcp_server``

#### How to run client and server
for running the server, ``./tcp_server <port number>``
for running the client, ``./tcp_client <host id> <port number>``.
### current Issues
##### Client Issues

- Handle port number optional. (ask)

#### Server Issues
- Handle request type in better way.

#### General problems