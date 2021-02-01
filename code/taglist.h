#pragma once

#include <stdio.h>
#include <stdlib.h>

#define MAX_TAG_LENGTH 100

typedef struct TagCell_str *T_Pointer; //Points to a cell

typedef struct TagCell_str { //Each cell:
    char tag[MAX_TAG_LENGTH]; //Saves a subscribed tag
    T_Pointer next; //Points to next tag in the list
} TagCell;

typedef struct { //Each list has a pointer to the first and last cell
    T_Pointer first, last;
} TagList;

void CreateTagList(TagList *list); //Creates an empty tag list

int IsEmpty_Tag(TagList *list); //Checks if list is empty

int CheckForTag(char tag[], TagList list); //Checks if tag is already on the list

void NewTag(char tag[], TagList *list); //Adds a tag to the end of the list

void RemovesTag(char tag[], TagList *list); //Removes tag from subscripitions

void Print_T(TagList list); //Prints the tag list; used for testing only

void DeleteTagList(TagList *list); //Deletes a whole list
