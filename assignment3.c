// Server to accept connections from clients to receive book data to add to the stored linked list

#include "server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

int main(){
    
    //initialise head of linked list
    // struct Node *head = (struct Node*) malloc(sizeof(Node*));

    // Client adress nonsense
    // struct sockaddr_in clientAddress;

    // Server address nonsense
    struct sockaddr_in serverAddress;
    // Why idk this is a string function
    // to fill memory with a value, in this case "0"
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // TODO: Command line read port number not hardcode lol
    serverAddress.sin_port = htons(1234);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0){
        printf("SERVER: socket creation error: %d\n", sock);
        return 0;
    }

    int errorStatus = bind(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (errorStatus < 0){
        printf("SERVER: bind error: %d\n", errorStatus);
        perror("Error: ");
        return 0;
    }

    // Have up to 5 no. of connections that can be waiting
    // before system poops
    int status = listen(sock, 5);
    if (status < 0){
        printf("SERVER: listen error: %d\n", errorStatus);
        return 0;
    }

    // Just use server address again lol because client and
    // server are running on same machine
    socklen_t clientLen = sizeof(serverAddress);
    int newSock = accept(sock, (struct sockaddr *) &serverAddress, &clientLen);
    if (newSock < 0){
        printf("SERVER: accept error: %d\n", errorStatus);
        return 0;
    }

    char buffer[1000];
    bzero(buffer, 1000);
    int readError = read(newSock, buffer, 1000);
    if (readError < 0){
        printf("SERVER: read error: %d\n", readError);
        return 0;
    }

    printf("SERVER: Buffer content: %s\n", buffer);
    

    printf("SERVER: Done returning 0\n");
    return 0;
}