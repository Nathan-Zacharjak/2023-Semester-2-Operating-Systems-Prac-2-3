#include "server.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// Including this just so intellisense works in VSCode, but it should work with this commented out
// (It should be included as part of unistd.h but I was having difficulties...)
// #include <bits/getopt_posix.h>

#define MAX_LINE 10000
#define TRUE 1
#define FALSE 0
#define MAX_CLIENTS 20

//function creates server socket
int startServer(int port){
    
    struct sockaddr_in serverAddress;
    
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(port);

    // Creating server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0){
        perror("SERVER: socket creation error");
        exit(1);
    }

    //bind server socket 
    int errorStatus = bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (errorStatus < 0){
        perror("SERVER: bind error");
        //perror("Error: ");
        exit(1);
    }

    // Have up to 10 no. of connections that can be waiting
    int status = listen(serverSocket, 10);
    if (status < 0){
        perror("SERVER: listen error");
        exit(1);
    }

    printf("SERVER STARTED\n");

    return serverSocket;

}

//this function accepts clients 
int acceptConnection(int serverSocket){
    struct sockaddr_in clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    int clientLen = sizeof(clientAddress);
    int newSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, (socklen_t*)&clientLen);
    
    if (newSocket < 0){
        printf("SERVER: accept error: %d\n", newSocket);
        return 0;
    } 
    printf("SERVER: accepted new connection\n");

    return newSocket;
}


//this function reads book text sent by client
void readClient(int newSocket, struct Node *head, struct Node **bookHeads, int connectionNum){
    char buffer[MAX_LINE];
    bzero(buffer, MAX_LINE);
    int readStatus;
    int bookInd = 0;

    // find spot to put book in bookheads
    for (int i=0; i<MAX_CLIENTS; i++){
        if (bookHeads[i]->text == NULL){
            bookInd = i;
            break;
        }
    }

    struct Node* prevNode = (struct Node*)malloc(sizeof(Node));
    //readStatus = read(newSocket, buffer, MAX_LINE
    while ((readStatus = read(newSocket, buffer, MAX_LINE))){
        struct Node* newNode = (struct Node*)malloc(sizeof(Node));
        newNode->text = strdup(buffer);
        newNode->next = NULL;
        newNode->book_next = NULL;
        printf("%s\n", newNode->text);

        // First ever line from the first book
        if (head->text == NULL){
            // printf("THE START OF SUMN NEW HEHEHEE\n");
            head->text = strdup(buffer);
            bookHeads[bookInd] = newNode;
            prevNode = bookHeads[bookInd];
            // printf("SERVER: started head of linked list\n");
        } else {
            //printf("CONTINUED DILEMNA\n");
            // The first line from a book, that is not the very first book
            if (bookHeads[bookInd]->text==NULL){
                //printf("FIrst line from book that aint the very first book,, ehee\n");
                bookHeads[bookInd] = newNode;
                prevNode = bookHeads[bookInd];
                addNode(head,newNode);
                printf("line 1\n");
            } else {
                // printf("THE OTHER BOOKLINES,, ehee\n");
                // All other book lines
                //printf("line 1\n");
                prevNode->book_next = newNode;
                //printf("line 2\n");
                addNode(head, newNode);
                //printf("line 3\n");
                prevNode = (struct Node*)malloc(sizeof(Node));
                //printf("line 4\n");
                prevNode = newNode;
            }
            //printf("SERVER: added node to linked list\n");
        }

        bzero(buffer, MAX_LINE);
        //printf("READ STATUS %d\n", readStatus);
    }
    //printf("READ STATUS %d\n", readStatus);
    if (readStatus < 0){
        perror("SERVER: read error");
    } else if (readStatus == 0){
        printf("Server: client reached EOF! \n");
    }

    close(newSocket);
    //printf("SERVER: closing connection to client num %d\n", connectionNum);
    //printf("PRINTING LINKED LIST: \n");
    //printList(head);
    //printf("SERVER: Printing book\n");
    //printBooks(bookHeads);

    //write to file
    char fileName[50] = "";
    if (connectionNum < 10){
        sprintf(fileName,"book_0%d.txt",connectionNum);
    } else {
        sprintf(fileName,"book_%d.txt",connectionNum);
    }

    FILE *file = fopen(fileName,"a");
    struct Node* currentNode = bookHeads[bookInd];

    while(currentNode->book_next != NULL){
        int fileStatus = fputs(currentNode->text,file); 
                       
        if (fileStatus < 0){
            printf("SERVER: file write error: %d\n", fileStatus);
            return;
        }

        currentNode = currentNode->book_next;
    }

    int fileStatus = fputs(currentNode->text,file); 

    if (fileStatus < 0){
        printf("SERVER: file write error: %d\n", fileStatus);
        return;
    }

    fclose(file);
}


// MAIN FUNCTION!
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


    // Creating an array that stores head of books
    struct Node *bookHeads[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++){
        bookHeads[i] = (struct Node*) malloc(sizeof(Node));
    }

    //create and bind server socket to portNum
    int serverSocket = startServer(portNum);

    int maxSocket = serverSocket;
    //printf("server socket: %d\n", maxSocket);

    //initialise socket sets for select 
    fd_set connectedSockets, readSockets;

    //initialise connectedSockets
    FD_ZERO(&connectedSockets);
    FD_SET(serverSocket, &connectedSockets);

    //keep track of number of connections
    int connectionNum = 0;

    //accept and read from clients 
    while(TRUE){
        //getting available client sockets
        readSockets = connectedSockets;

        //using select, timeout set to NULL 
        struct timeval tv = {5, 0};   
        int activity = select(FD_SETSIZE, &readSockets, NULL, NULL, &tv);

        if (activity < 0){
            printf("SERVER: select error %d\n", activity);
            return 0;
        }

        //check ready client sockets 
        for (int i=0; i<=maxSocket; i++){

            //printf("i: %d\n", i);

            if (FD_ISSET(i, &readSockets)){
                //new client connection made to server
                if (i == serverSocket){
                    //accept new connection and add to connectedSockets set 
                    int newClient = acceptConnection(serverSocket);
                    FD_SET(newClient, &connectedSockets);

                    if (newClient > maxSocket) {
                        maxSocket = newClient;
                    }

                    //increment number of connections made
                    connectionNum++;
                    //printf("SERVER: connection number %d\n", connectionNum);

                } else {
                    //read book from connection
                    readClient(i, head, bookHeads, connectionNum);

                    //remove client socket from connectedSockets
                    FD_CLR(i, &connectedSockets);

                }
            }
        }
    }

    //printList(head);

    return 0;

}