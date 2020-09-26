
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

void getCookie(char * response, char **cookie);
char * get_payload(char * response);
void get_token(char *response, char **token);
void send_register(char user[], char pass[], int *sockfd);
void send_login(char user[], char pass[], int *sockfd, char **cookie);
void enter_library(int *sockfd, char *cookie, char** token);
void add_book(char *title, char* author, char *genre,
	char *publisher, char *page_count, int *sockfd, char *token);
void get_books(int *sockfd, char *token);
void get_book(char *id, int *sockfd, char *token);
void delete_book(char *id, int *sockfd, char *token);
void logout(int *sockfd, char **cookie, char **token);