# socket-programming
Simple Implementation for a webserver using Unix socket API

### To test the TCP server
after you run the server use curl as follows:
``curl -X GET 'http://0.0.0.0:5000/home/tarek/tmp/notes.md``

### current Issues
##### Client Issues

- Fix client timeout problem based on response size. (considered)

#### Server Issues
- Handle request type correctly.
- Timeout on server side.
    * Persistent connections
    * Dynamic according to number of opened connections
- Fork or threads in both client and server
