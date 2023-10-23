// Server to accept connections from clients to receive book data to add to the stored linked list

#include "server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 10000
#define PORT_NUM 12345

int main(){
    
    //initialise head of linked list
    struct Node *head = (struct Node*) malloc(sizeof(Node));


    // Server address nonsense
    struct sockaddr_in serverAddress;
    // Why idk this is a string function
    // to fill memory with a value, in this case "0"
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    // TODO: Command line read port number not hardcode lol
    serverAddress.sin_port = htons(PORT_NUM);

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

    // GO GO CREATE NEW THREAD for each client connection :)
    
    char buffer[MAX_LINE];
    while (1){
        bzero(buffer, MAX_LINE);
        int readStatus = read(newSock, buffer, MAX_LINE);
        if (readStatus < 0){
            printf("SERVER: read error: %d\n", readStatus);
            return 0;
        } else if (readStatus == 0){
            printf("SERVER: REACHED EOF!\n");
            break;
        }

        // printf("SERVER: Buffer content: %s\n", buffer);
        if(head->text == NULL){
            head->text = strdup(buffer); //<-- clutch
        } else {
            struct Node* newNode = (struct Node*) malloc(sizeof(Node));
            newNode->text = strdup(buffer);
            // printf("%s", newNode->text);

            addNode(head, newNode);
        }
    }
    printList(head);
    

    printf("SERVER: Done returning 0\n");
    return 0;
}