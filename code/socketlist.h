#pragma once

#include "taglist.h"

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>

struct client_data {
    int csock;
    struct sockaddr_storage storage;
    TagList subscribed_tags;
};

typedef struct SocketCell_str *S_Pointer; //Points to a cell

typedef struct SocketCell_str { //Each cell:
    struct client_data c_data; //Saves the client socket
	S_Pointer next; //Points to next client in the list
} SocketCell;

typedef struct { //Each list has a pointer to the first and last cell
	S_Pointer first, last;
} SocketList;

void CreateSocketList(SocketList *list); //Creates an empty list

int IsEmpty_Socket(SocketList *list); //Checks if list is empty

void NewSocket(struct client_data *cdata, SocketList *list); //Adds a client to the end of the list

void RemovesSocket(int c_sock, SocketList *list); //Removes the socket from the list

void Print_S(SocketList list); //Prints the sockets list; used for testing only

void DeleteSocketList(SocketList *list); //Deletes a whole list
