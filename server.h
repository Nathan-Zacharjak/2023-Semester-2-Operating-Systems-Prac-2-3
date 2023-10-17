#ifndef SERVER_H_
#define SERVER_H_
#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    // char array to hold the book text chunk sent by client
    char text[100];

    // links to the next element in the shared list.   
    struct Node *next;

    // links to the next item in the same book. 
    struct Node *book_next;

    // for part two search, links to the next item that had the search terms
    struct Node *next_frequent_search;

} Node;


// function that adds a new node to the end of a linked list
void addNode(Node *head, Node *newNode) {
    Node *iter = head;

    // iterate over linked list to find the end
    while (iter->next != NULL){
        iter = iter->next;
    }

    // adde new node to end of linked list
    iter->next = newNode;
}


// function that prints out book text in linked list
void printList(Node *head){
    Node *iter = head;

    while (iter != NULL){
        printf("%s", iter->text);
        iter = iter->next;
    }
}

#endif