all:
	gcc -Wall -c taglist.c
	gcc -Wall -c socketlist.c
	gcc -Wall -c common.c
	gcc -Wall client.c common.o -o cliente
	gcc -Wall server.c taglist.o socketlist.o common.o -lpthread -o servidor

clean:
	rm taglist.o socketlist.o common.o client server server
