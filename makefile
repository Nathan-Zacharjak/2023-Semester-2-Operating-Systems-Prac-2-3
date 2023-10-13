# Makefile to complie and run the server and clients

server: compserver runserver


compserver: server.c
	gcc -Wall -O3 -o assignment3 server.c

runserver: assignment3
	./assignment3 -l 12345  -p "happy"


client1: compclient1 runclient

compclient1: client.c
	gcc -Wall -O3 -o client client.c

runclient1: client
	./client -l 12345  -b "input/book1.txt"
