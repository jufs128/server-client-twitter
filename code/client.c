#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void usage(int argc, char **argv) {
	printf("usage: %s <server IP> <server port>\n", argv[0]);
	printf("example: %s 127.0.0.1 51511\n", argv[0]);
	exit(EXIT_FAILURE);
}

#define MAX_MSG_LENGTH 500

int main(int argc, char **argv) {
	if (argc < 3) {
		usage(argc, argv);
	}

	struct sockaddr_storage storage;
	if (0 != addrparse(argv[1], argv[2], &storage)) {
		usage(argc, argv);
	}

	int s;
	//s = socket(storage.ss_family, SOCK_STREAM, 0);
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == -1) {
		logexit("socket");
	}
	struct sockaddr *addr = (struct sockaddr *)(&storage);
	if (0 != connect(s, addr, sizeof(storage))) {
		logexit("connect");
	}

	char addrstr[MAX_MSG_LENGTH];
	addrtostr(addr, addrstr, MAX_MSG_LENGTH);

	char msg[MAX_MSG_LENGTH];
	char reply[MAX_MSG_LENGTH];
	memset(msg, 0, MAX_MSG_LENGTH);
	memset(reply, 0, MAX_MSG_LENGTH);
	size_t count;

	fd_set read;
	fd_set write;
	fd_set except;
	int max_fd = s;

	while(1) {
		//Select() updates fd_set, so they need to be re-built for every call
		FD_ZERO(&read);
		FD_SET(STDIN_FILENO, &read);
		FD_SET(s, &read);

		FD_ZERO(&write);
		if (strlen(msg) != 0) //If there's something to send
			FD_SET(s, &write);

		FD_ZERO(&except);
		FD_SET(STDIN_FILENO, &except);
		FD_SET(s, &except);

		int pick = select(max_fd + 1, &read, &write, &except, NULL);

		switch(pick) {
			case -1: //Error in select()
				logexit("select");
			
			case 0: //Should never get here
				logexit("select");

			default:
				if (FD_ISSET(STDIN_FILENO, &read)) //Read from stdin
					fgets(msg, MAX_MSG_LENGTH-1, stdin);

				if (FD_ISSET(STDIN_FILENO, &except)) //Error on stdin
					exit(EXIT_FAILURE);

				if (FD_ISSET(s, &read)) { //Receive message from server
					memset(reply, 0, MAX_MSG_LENGTH);
					count = recv(s, reply, MAX_MSG_LENGTH, 0);
					if (count == 0) {
						//Connection terminated.
						return 0;
					}
					printf("%s", reply);
				}

				if (FD_ISSET(s, &write)) { //Send message to server
					count = send(s, msg, strlen(msg) + 1, 0);
					if (count != strlen(msg) + 1) 
						logexit("send");
					memset(msg, 0, MAX_MSG_LENGTH); //Makes msg empty to receive a new one
				}

				if (FD_ISSET(s, &except)) //Error on server
					exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}