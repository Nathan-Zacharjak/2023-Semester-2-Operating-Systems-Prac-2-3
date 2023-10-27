# Client-Server Book Processing Program

A README document detailing the commands used to run your server and client programs (e.g., Client program using netcat) to send data to the server.

## Server

In one terminal window we compiled the server with: `gcc -Wall -O3 -o assignment3 assignment3.c`
and ran the server with: `./assignment3 -l 1234 -p "happy"`
or `make server`.

## Client

In another terminal window we compiled the client with: `gcc -Wall -O3 -o client client.c`
and ran the client with: `./client -l 1234 -b [Book number]`
or `make client[Book number]`.
