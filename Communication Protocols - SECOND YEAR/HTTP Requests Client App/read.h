#include "parson.h"

#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

// functii pentru citirea inputului de la tastatura
void read_user_data(char *buf, size_t buf_size, char* user[], char* pass[]);
void read_id(char *buf, size_t buf_size, char ** id);
void read_book_data(char *buf, size_t buf_size, char **title, char ** author,
	char **genre, char **publisher, char **page_count);