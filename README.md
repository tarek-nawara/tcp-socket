# socket-programming
Simple Implementation for a webserver using Unix socket API

### To test the TCP server
after you run the server use curl as follows:
``curl -X GET 'http://0.0.0.0:5000/home/tarek/tmp/notes.md``

### current Issues
##### Client Issues

- Fix client timeout problem based on response size. (considered)
- Handle port number optional. (ask)

#### Server Issues
- Handle request type in better way.
- Implement HTTP/1.1.
- Timeout on server side.
- Persistent connections
        * Server don't close the connections after the serving the request.
        * Client should use the same connection.
    * Dynamic according to number of opened connections

#### General problems