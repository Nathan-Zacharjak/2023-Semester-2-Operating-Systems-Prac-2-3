# Makefile to complie and run the server and clients

all: server 


server: compserver runserver

compserver: assignment3.c
	gcc -Wall -O3 -o assignment3 assignment3.c

# l = port number
# p = search term
runserver: assignment3
	./assignment3 -l 12345 -p "happy"


#client1: compclient1 runclient1

#compclient1: client.c
#	gcc -Wall -O3 -o client client.c

# l = port number
# b = book number
#runclient1: client
#	./client -l 12345 -b 1


#client2: compclient2 runclient2

#compclient2: client.c
#	gcc -Wall -O3 -o client client.c

# l = port number
# b = book number
#runclient2: client
#	./client -l 12345 -b 2

#client3: compclient3 runclient3

#compclient3: client.c
#	gcc -Wall -O3 -o client client.c

# l = port number
# b = book number
#runclient3: client
#	./client -l 12345 -b 3

#clients: client1 && client2
