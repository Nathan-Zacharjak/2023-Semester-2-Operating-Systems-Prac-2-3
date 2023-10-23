// Client to send book data for the server to store in the giant linked list

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// Including this just so intellisense works in VSCode, but it should work with this commented out
// (It should be included as part of unistd.h but I was having difficulties...)
#include <bits/getopt_posix.h>

int main(int argc, char* const *argv){
    // Reading the command-line arguments
    int port = 0;
    int bookNumber = 0;
    int opt = 0;

    // l = port number
    // b = book number
    // Fancy command-line reading function "getopt()" from the UNIX standard library: unistd.h
    while ((opt = getopt(argc, argv, "l:b:")) != -1){
        switch (opt){
        case 'l':
            port = atoi(optarg);
            break;
        case 'b':
            bookNumber = atoi(optarg);
            break;
        case ':':
            printf("Client: Please provide a valid port and book number argument: -l -b\n");
            return 0;
            break;
        case '?':
            printf("Client: Unknown argument passed: %c\n", optopt);
            return 0;
            break;
        default:
            printf("Client: getopt returned character code 0%o?\n", opt);
            return 0;
            break;
        }
    }
    printf("Client %d: Processing book %d, on port %d\n", bookNumber, bookNumber, port);

    // Formatting command
    // nc localhost <port> -i <delay> < file.txt
    char command[50] = "\0";
    sprintf(command, "nc localhost %d -i 1 < input/book%d.txt", port, bookNumber);

    // Executing command
    printf("Client %d: Executing command: '%s'\n", bookNumber, command);
    int result = system(command);

    if (result == 0){
        printf("Client %d: system() call failed!\n", bookNumber);
    }


    return 0;
}
