#include "taglist.h"

#include <string.h>

void CreateTagList(TagList *list) { //Creates an empty list
	list->first = (T_Pointer) malloc(sizeof(TagCell));
	list->last = list->first;
	list->first->next = NULL; //First point to nothing
}

int IsEmpty_Tag(TagList *list) { //Checks if list is empty
	return (list->first ==  NULL);
}

int CheckForTag(char tag[], TagList list) { //Checks if tag is already in the list
    T_Pointer aux;
    aux = list.first->next;
    while (aux != NULL) {
        if (strcmp(aux->tag, tag) == 0) { //If tag in pointed cell is equal to the received;
            return 1;                     //return 1
        }
        aux = aux->next;
    }
    return 0; //If no matches are found, return 0
}

void NewTag(char tag[], TagList *list) { //Adds a tag to the end of the list
    list->last->next = (T_Pointer) malloc(sizeof(TagCell));
    list->last = list->last->next;
    strcpy(list->last->tag, tag);
    list->last->next = NULL;
}

void RemovesTag(char tag[], TagList *list) { //Removes tag from subscripitions
    //First finds the element previous to the one to be removed
    T_Pointer previous = list->first;
    while (strcmp(previous->next->tag, tag) != 0)
        previous = previous->next;
    
    T_Pointer removed;
    if (IsEmpty_Tag(list) || previous == NULL || previous->next == NULL) {
        return;
    }

    removed = previous->next;
    previous->next = removed->next;
    if (previous->next == NULL) //If previous is now the last tag on the list,
        list->last = previous;  //pointer of last element points to it
    free(removed);
}

void Print_T(TagList list) { //Prints the tag list
    T_Pointer aux;
    aux = list.first->next;
    while (aux != NULL) {
        printf("%s -> ", aux->tag);
        aux = aux->next;
    }
    printf("\n");
}

void DeleteTagList(TagList *list) { //Deletes a whole list
    T_Pointer current = list->first;
    T_Pointer next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    list->first = NULL;
    list->last = NULL;
}
