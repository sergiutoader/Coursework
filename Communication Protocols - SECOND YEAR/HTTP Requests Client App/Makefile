CC=gcc
CFLAGS=-I.

client: client.c requests.c helpers.c buffer.c parson.c commands.c read.c
	$(CC) -o client client.c requests.c helpers.c buffer.c parson.c commands.c read.c -Wall -g

run: client
	./client

clean: 
	rm -f *.o client
