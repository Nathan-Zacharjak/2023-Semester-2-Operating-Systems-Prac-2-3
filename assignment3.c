// Server to accept connections from clients to receive book data to add to the stored linked list

#include "server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// Including this just so intellisense works in VSCode, but it should work with this commented out
// (It should be included as part of unistd.h but I was having difficulties...)
#include <bits/getopt_posix.h>

#define MAX_LINE 10000
#define TRUE 1
#define FALSE 0
#define MAX_CLIENTS 20

int main(int argc, char* const *argv){

    // Reading the given arguments to run the server
    int portNum = 0;
    char* searchTerm = "";
    // Used for reading arguments
    int opt = 0;

    // l = port number
    // b = book number
    // Fancy command-line reading function "getopt()" from the UNIX standard library: unistd.h
    while ((opt = getopt(argc, argv, "l:p:")) != -1){
        switch (opt){
        case 'l':
            portNum = atoi(optarg);
            break;
        case 'p':
            searchTerm = strdup(optarg);
            break;
        case ':':
            printf("SERVER: Please provide a valid port and search argument: -l -p\n");
            return 0;
            break;
        case '?':
            printf("SERVER: Unknown argument passed: %c\n", optopt);
            return 0;
            break;
        default:
            printf("SERVER: getopt returned character code 0%o?\n", opt);
            return 0;
            break;
        }
    }

    printf("SERVER: Port: %d Search Term: %s\n", portNum, searchTerm);
    
    //initialise head of linked list
    struct Node *head = (struct Node*) malloc(sizeof(Node));
    // Creating an array of head of books
    struct Node *bookHeads[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++){
        bookHeads[i] = (struct Node*) malloc(sizeof(Node));
    }

    //create select stuff
    int client_socket[MAX_CLIENTS], max_sd = 0, sd = 0, activity = 0;
    for (int i = 0; i < MAX_CLIENTS; i++){
        client_socket[i] = 0;
    }

    //socket descriptors
    fd_set socket_set;

    // Server address nonsense
    struct sockaddr_in serverAddress;
    // Why idk this is a string function
    // to fill memory with a value, in this case "0"
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(portNum);

    // Creating master server socket
    int master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket < 0){
        printf("SERVER: socket creation error: %d\n", master_socket);
        return 0;
    }

    int errorStatus = bind(master_socket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (errorStatus < 0){
        printf("SERVER: bind error: %d\n", errorStatus);
        perror("Error: ");
        return 0;
    }

    // Have up to 5 no. of connections that can be waiting
    int status = listen(master_socket, 5);
    if (status < 0){
        printf("SERVER: listen error: %d\n", status);
        return 0;
    }

    char buffer[MAX_LINE];
    
    while (TRUE){
        // Clear set of sockets
        FD_ZERO(&socket_set);

        // Add master socket to set of sockets
        FD_SET(master_socket, &socket_set);
        max_sd = master_socket;

        for (int i = 0; i < MAX_CLIENTS; i++){
            sd = client_socket[i];

            // if socket descriptor is an actual socket descriptor, then add to socket set
            if (sd > 0){
                FD_SET(sd, &socket_set);
            }
            
            // update max socket descriptor number for select() function
            if (sd > max_sd){
                max_sd = sd;
            }    
        }

        // Actually selecting which socket to accept (select() function does its magic here)
        // Timeout is set to NULL, server will wait forever for a client connection
        activity = select(max_sd + 1, &socket_set, NULL, NULL, NULL);

        if (activity < 0){
            printf("SERVER: select error: %d\n", activity);
            return 0;
        }
        
        // Detecting if there is an incoming client connection
        if (FD_ISSET(master_socket, &socket_set)){
            socklen_t clientLen = sizeof(serverAddress);
            // Just use server address again lol because client and
            // server are running on same machine
            int newSock = accept(master_socket, (struct sockaddr *) &serverAddress, &clientLen);
            if (newSock < 0){
                printf("SERVER: accept error: %d\n", errorStatus);
                return 0;
            }

            printf("SERVER: New connection, fd = %d\n", newSock);

            // Add new socket to socket array
            for (int i = 0; i < MAX_CLIENTS; i++){
                // Finding an empty spot
                if (client_socket[i] == 0){
                    client_socket[i] = newSock;
                    printf("SERVER: Adding client to socket array: %d, index:%d\n", client_socket[i], i);

                    // Creating head node for this client's book
                    struct Node *bookHead = (struct Node*) malloc(sizeof(Node));

                    break;
                }
            }
        }
        
        // Going through every client and reading a line of the client's book
        for (int i = 0; i < MAX_CLIENTS; i++){
            int currentSocket = client_socket[i];

            if (FD_ISSET(currentSocket, &socket_set)){
                // Zero out the buffer
                bzero(buffer, MAX_LINE);
                int readStatus = read(currentSocket, buffer, MAX_LINE);
                // Checking for read error
                if (readStatus < 0){
                    printf("SERVER: read error: %d\n", readStatus);
                    return 0;
                // Checking for end of file
                } else if (readStatus == 0){
                    printf("SERVER: REACHED EOF!\n");
                    close(currentSocket);
                    client_socket[i] = 0;
                    printList(head);
                // Else, add the client's data to the server
                } else {
                    // printf("SERVER: Buffer content: %s\n", buffer);
                    if(head->text == NULL){
                        head->text = strdup(buffer); //<-- clutch
                    } else {
                        struct Node* newNode = (struct Node*) malloc(sizeof(Node));
                        newNode->text = strdup(buffer);
                        // printf("%s", newNode->text);

                        addNode(head, newNode);
                        printf("SERVER: Added node from socket:%d\n", currentSocket);
                    }
                }
            }   
        }
    }
    

    printf("SERVER: Done returning 0\n");
    return 0;
}