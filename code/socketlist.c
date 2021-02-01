#include "socketlist.h"

//Sockets list

void CreateSocketList(SocketList *list) { //Creates an empty list
	list->first = (S_Pointer) malloc(sizeof(SocketCell));
	list->last = list->first;
	list->first->next = NULL; //First point to nothing
}

int IsEmpty_Socket(SocketList *list) { //Checks if list is empty
	return (list->first == list->last);
}

void NewSocket(struct client_data *cdata, SocketList *list) { //Adds a client to the end of the list
    list->last->next = (S_Pointer) malloc(sizeof(SocketCell));
    list->last = list->last->next;
    list->last->c_data = *cdata;
    list->last->next = NULL;
}

void RemovesSocket(int c_sock, SocketList *list) { //Removes the socket from the list
    //First finds the element previous to the one to be removed
    S_Pointer previous = list->first;
    while (previous->next->c_data.csock != c_sock) {
        previous = previous->next;
    }
    
    S_Pointer removed;
    if (IsEmpty_Socket(list) || previous == NULL || previous->next == NULL) {
        return;
    }

    removed = previous->next;
    previous->next = removed->next;
    if (previous->next == NULL) //If previous is now the last client on the list,
        list->last = previous;  //pointer of last element points to it
    DeleteTagList(&removed->c_data.subscribed_tags);
    free(removed);
}

void Print_S(SocketList list) { //Prints the sockets list
    S_Pointer aux;
    aux = list.first->next;
    while (aux != NULL) {
        printf("%d -> ", aux->c_data.csock);
        aux = aux->next;
    }
    printf("\n");
}

void DeleteSocketList(SocketList *list) { //Deletes a whole list
    S_Pointer current = list->first;
    S_Pointer next;

    while (current != NULL) {
        next = current->next;
        DeleteTagList(&current->c_data.subscribed_tags);
        free(current);
        current = next;
    }

    list->first = NULL;
    list->last = NULL;
}
