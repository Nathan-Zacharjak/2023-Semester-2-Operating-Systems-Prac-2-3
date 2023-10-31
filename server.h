#ifndef SERVER_H_
#define SERVER_H_
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 10000

typedef struct Node
{
    // char array to hold the book text chunk sent by client
    char* text;

    // links to the next element in the shared list.   
    struct Node *next;

    // links to the next item in the same book. 
    struct Node *book_next;

    // links to the previous item in the same book. 
    // struct Node *book_prev;

    // for part two search, links to the next item that had the search terms
    struct Node *next_frequent_search;

} Node;


// function that adds a new node to the end of a linked list
void addNode(Node *head, Node *newNode) {
    Node *node = head;
    // printf("Initialisation works! %p %p\n", head, newNode);

    // nodeate over linked list to find the end
    int nodeCount = 0;
    while (node->next != NULL){
        nodeCount++;
        // printf("Processing loop... %p %d\n", node, nodeCount);
        node = node->next;
    }
    // printf("Loopdyloop is done :3\n");

    // adde new node to end of linked list
    node->next = newNode;
    // printf("FIN\n");
}


// function that prints out book text in linked list
void printList(Node *head){
    Node *iter = head;
    int i = 0;
    while (iter != NULL){
        printf("%s", iter->text);
        iter = iter->next;
        i++;
    }
    printf("\nlength of linked list : %d\n", i);
}

//function that prints books by book_next
void printBooks(Node **bookHeads){
    for (int i=0; i<20; i++){
        Node *iter = bookHeads[i];
        printf("Book number %d:\n", i+1);
        while (iter !=NULL){
            printf("%p\n%s\n", iter->book_next, iter->text);
            iter = iter->book_next;
        }
    }
}

#endif