# README

This is a simple example created during the lecture:
- a "server" listens to a port and from a named pipe in the filesystem for chunks of data
- a "client" splits a file in chunks and send them to the server via socket and pipe

The server reconstructs the final file given the chunks he receives.

# Build

```
gcc client.c -o client
gcc server.c -o server
```

# Run

Create a named pipe:
```
mkfifo test
```

Run server:
```
./server test 8888
```

Run client:
```
./client file-to-send.txt test 127.0.0.1 8888 
```

Compare result file (created by the server with the one sent in chunks):
```
diff result.txt file-to-send.txt
```

