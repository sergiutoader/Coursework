
CFLAGS = -Wall -g

default: build

build: helpers.c list.c msg_list.c server.c subscriber.c
	gcc server.c helpers.c list.c msg_list.c -Wall -g -o server
	gcc subscriber.c helpers.c list.c msg_list.c -Wall -g -o subscriber

clean:
	rm -f server subscriber