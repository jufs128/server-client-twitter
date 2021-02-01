#include "taglist.h"
#include "socketlist.h"
#include "common.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#define MAX_MSG_LENGTH 500
//MAX_TAG_LENGTH is 100; defined in taglist.h

SocketList client_list = {}; //Creates global list of connected sockets

void usage(int argc, char **argv) { //If wrongly executed, prints example of correct form
    printf("usage: %s <server port>\n", argv[0]);
    printf("example: %s 51511\n", argv[0]);
    exit(EXIT_FAILURE);
}

void close_client_connection(struct client_data *cdata) { //Diconnects a specific client
    RemovesSocket(cdata->csock, &client_list);

    close(cdata->csock);
}

void close_all_connections() {
    S_Pointer aux = client_list.first->next;
    while(aux != NULL) { //Goes through all clients on the list to close connection
        close_client_connection(&aux->c_data);
    }

    exit(EXIT_SUCCESS);
}

int IsInvalidCharacter(char c) {
    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
        || (strchr(" ,.?!:;+-*/=@#$%()[]{}", c) != NULL) //Returns NULL if character not found
        || (c == '\n')) //New line character is also valid
        return 0;
    else
        return 1;
}

int send_message(char msg[], struct client_data *cdata) { //Select subscribed users and sends message
    TagList msg_tags; //List of tags in the message
    CreateTagList(&msg_tags);
    
    char tag[MAX_TAG_LENGTH];
    memset(tag, 0, MAX_TAG_LENGTH);
    for (int i = 0; i < strlen(msg); i++) {
        if (IsInvalidCharacter(msg[i]) == 1) { //If there's an invalid character, return -2
            return -2;
        }
        if (msg[i] == '#' && (i == 0 || msg[i - 1] == ' ')) { //Finds a # at beggining of message or after a space
            i++;
            for(int j = 0; i < strlen(msg); j++) { //Tries to register the tag;
                                                   //j is the position in tag, i the position in the whole message
                if (IsInvalidCharacter(msg[i]) == 1) {
                    return -2;
                }

                if (msg[i] == ' ' || msg[i] == '\n') { //End of valid tag
                    break;
                } else if (msg[i] == '#') { //Invalid tag, break
                    memset(tag, 0, MAX_TAG_LENGTH);
                    break;
                }
                tag[j] = msg[i];
                i++; //Next character of message
            }

            if(strlen(tag) > 0 && CheckForTag(tag, msg_tags) == 0) { //Tag is not empty and not registered
                //printf("%s\n", tag);
                NewTag(tag, &msg_tags);
                memset(tag, 0, MAX_TAG_LENGTH);
            }
        }
    }

    S_Pointer aux_c; //To navigate client list
    T_Pointer aux_ct; //To navigate the client's tag list

    aux_c = client_list.first->next;
    while(aux_c != NULL) { //For every connected client
        aux_ct = aux_c->c_data.subscribed_tags.first->next;

        if (aux_c->c_data.csock != cdata->csock) { //If it's not the client who sent the message
            while (aux_ct != NULL) { //For all his subscribed tags
                if (CheckForTag(aux_ct->tag, msg_tags) == 1) {
                    write(aux_c->c_data.csock, msg, strlen(msg)); //Sends the client the message
                        break;
                }

                aux_ct = aux_ct->next;
            }
        }
        
        aux_c = aux_c->next; //Next client
    }
    
    return 0;
}

int handle_message(char msg[], struct client_data *cdata) { //Processing the message
    if(msg[0] == '+') { //If it's subscribing to a tag
        msg[strlen(msg) - 1] = '\0'; //To avoid same tags be different because of \n

        char tag[MAX_TAG_LENGTH];
        memset(tag, 0, MAX_TAG_LENGTH);
        char reply[MAX_MSG_LENGTH];
        memset(reply, 0, MAX_MSG_LENGTH);

        for(int i = 1; i < strlen(msg); i++) { //Saves the tag on the variable 'tag'
            if (IsInvalidCharacter(msg[i]) == 1) {
                return -2;
            }

            if (msg[i] == ' ')
                break;
            else if (msg[i] == '#') { //No # in the middle of the tag are allowed
                return -1;
            }
            tag[i-1] = msg[i];
        }

        if(strlen(tag) == 0) //If tag is empty, error
            return -1;
        
        if (CheckForTag(tag, cdata->subscribed_tags) == 0) { //If tag not subscribed, subscribes
            NewTag(tag, &cdata->subscribed_tags);
            //Print_T(cdata->subscribed_tags);
            
            sprintf(reply, "subscribed +%s\n", tag);

        } else {
            sprintf(reply, "already subscribed +%s\n", tag);
        }

        if(strlen(reply) <= MAX_TAG_LENGTH && reply[strlen(reply) - 1] == '\n') //Checks reply's size and completeness
            write(cdata->csock, reply, strlen(reply));
        
        return 0;

    } else if(msg[0] == '-') { //If it's unsubscribing
        msg[strlen(msg) - 1] = '\0';

        char tag[MAX_TAG_LENGTH];
        memset(tag, 0, MAX_TAG_LENGTH);
        char reply[MAX_MSG_LENGTH];
        memset(reply, 0, MAX_MSG_LENGTH);

        for(int i = 1; i < strlen(msg); i++) { //Saves the tag on the variable 'tag'
            if (IsInvalidCharacter(msg[i]) == 1) {
                return -2;
            }

            if (msg[i] == ' ')
                break;
            else if (msg[i] == '#') { //No # in the middle of the tag are allowed
                memset(tag, 0, MAX_TAG_LENGTH);
                break;
            }
            tag[i-1] = msg[i];
        }

        if (strlen(tag) == 0) //If tag is empty, error
            return -1;

        if (CheckForTag(tag, cdata->subscribed_tags) == 1) { //If tag subscribed, unsubscribes
            RemovesTag(tag, &cdata->subscribed_tags);
            //Print_T(cdata->subscribed_tags);

            sprintf(reply, "unsubscribed -%s\n", tag);

        } else { 
            sprintf(reply, "not subscribed -%s\n", tag);
        }

        if(strlen(reply) <= MAX_TAG_LENGTH && reply[strlen(reply) - 1] == '\n') //Checks reply's size and completeness
            write(cdata->csock, reply, strlen(reply));

        return 0;
    } else {
        return send_message(msg, cdata);
    }
}

int detects_messages(char full_msg[], struct client_data *cdata) { //Checks for and selects multiple messages on full_msg
    int j = 0;
    char msg[MAX_MSG_LENGTH];
    memset(msg, 0, MAX_MSG_LENGTH);
    int return_value = 0;

    for (int i = 0; i < strlen(full_msg); i++) {
        msg[j] = full_msg[i]; 
        j++; //j saves the current position in msg, while i saves in full_msg

        if (full_msg[i] == '\n' && j != 0) { //Found the end of a message that's not empty
            int h = handle_message(msg, cdata); //End of one message, process it
            if (h == -2) { 
                return_value = -2; //Changes return value to -2, but continues search
            } else if (h < 0){
                printf("Couldn't process message: %s", msg);
            }

            memset(msg, 0, MAX_MSG_LENGTH); //Reset msg to search for another message
            j = 0;
        }
    }

    return return_value;
}

void * client_thread(void *data) {
    struct client_data *cdata = (struct client_data *)data;
    struct sockaddr *caddr = (struct sockaddr *)(&cdata->storage);

    char caddrstr[MAX_MSG_LENGTH];
    addrtostr(caddr, caddrstr, MAX_MSG_LENGTH);
    printf("[log] connection from %s\n\n", caddrstr);

    NewSocket(cdata, &client_list); //Puts client's socket in the list
    //Print_S(client_list);

    char msg[MAX_MSG_LENGTH];
    char buf[MAX_MSG_LENGTH];
    size_t count;
    while (1) {
        memset(msg, 0, MAX_MSG_LENGTH); //New message
        int msg_length = 0; //msg is initially empty

        while (msg[strlen(msg) - 1] != '\n') { //Not end of message yet
            memset(buf, 0, MAX_MSG_LENGTH); //Reset buf
            count = recv(cdata->csock, buf, MAX_MSG_LENGTH, 0); //Receive part of message on buf
            if(count == 0) {
                printf("Connection to %s terminated\n", caddrstr);
                close_client_connection(cdata);
                pthread_exit(EXIT_SUCCESS);
            }

            if ((msg_length + strlen(buf) > MAX_MSG_LENGTH) //If total received surpasses max size
                || ((msg_length + strlen(buf) == MAX_MSG_LENGTH) && (buf[strlen(buf) - 1] != '\n'))) { //Or there's more to receive but max size reached
                printf("Message too long! Terminating user connection\n");
                close_client_connection(cdata);
                pthread_exit(EXIT_SUCCESS);
            }

            for (int i = 0; i < strlen(buf); i++){
                msg[msg_length + i] = buf[i]; //Concatenates with what's on the end of msg
            }

            msg_length = strlen(msg); //Updates end of message
        }

        if (strlen(msg) >= 2) { //If message is not empty
            printf("[msg] %s, %d bytes: %s\n", caddrstr, (int)count, msg);

            if (strcmp(msg, "##kill\n") == 0) { //This command kills the server
                printf("Closing all connections and server\n");
                close_all_connections();
            }

            if (detects_messages(msg, cdata) == -2) {   //Checks if more than one message was received
                                                        //Then sends each message to be handled
                printf("Invalid character! Terminating user connection\n");
                printf("Connection to %s terminated\n", caddrstr);
                close_client_connection(cdata);
                pthread_exit(EXIT_SUCCESS);
            }
        }
    }

    printf("Connection to %s terminated\n", caddrstr);
    close_client_connection(cdata);

    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argc, argv);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init_v4(argv[1], &storage)) {
        printf("port must be different than 0\n");
        exit(EXIT_FAILURE);
    }
    
    int s;
    //s = socket(storage.ss_family, SOCK_STREAM, 0);
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    char addrstr[MAX_MSG_LENGTH];
    addrtostr(addr, addrstr, MAX_MSG_LENGTH);
    printf("bound to %s, waiting connections\n", addrstr);

    CreateSocketList(&client_list); //Makes global client list an empty list

    //Multithread to receive from multiple clients
    while (1) {
        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

	    struct client_data *cdata = malloc(sizeof(*cdata));
	    if (!cdata) {
	    	logexit("malloc");
	    }
	    cdata->csock = csock;
	    memcpy(&(cdata->storage), &cstorage, sizeof(cstorage));
        CreateTagList(&cdata->subscribed_tags);

        pthread_t tid;
        pthread_create(&tid, NULL, client_thread, cdata);
    }

    exit(EXIT_SUCCESS);
}
