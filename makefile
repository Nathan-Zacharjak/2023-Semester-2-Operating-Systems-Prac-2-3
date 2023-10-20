# Makefile to complie and run the server and clients

all: client1 server


server: compserver runserver

compserver: server.c
	gcc -Wall -O3 -o assignment3 server.c

# l = port number
# p = search term
runserver: assignment3
	./assignment3 -l 1234 -p "happy"


client1: compclient1 runclient1

compclient1: client.c
	gcc -Wall -O3 -o client client.c

# l = port number
# b = book number
runclient1: client
	./client -l 1234 -b 1
