/* Toader Sergiu-Cristian, 325CB */

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
#include "commands.h"
#include "parson.h"
#include "read.h"

int main() {
	// alocare memorie pentru toate bufferele folosite pentru datele
	// citite de la tastatura + pentru stocare cookie, token
	char *buf = (char *) malloc(1000);
	size_t buf_size = 1000;
	if(!buf) { fprintf(stderr, "Could not malloc buffer\n"); return 0; }
	char *id = (char *) malloc(50);
	if(!id) { fprintf(stderr, "Could not malloc id\n"); return 0; }
	char *user = (char *) malloc(100);
	if(!user) { fprintf(stderr, "Could not malloc user\n"); return 0; }
	char *pass = (char *) malloc(100);
	if(!pass) { fprintf(stderr, "Could not malloc pass\n"); return 0; }
	char *title = (char *) malloc(200);
	if(!title) { fprintf(stderr, "Could not malloc title\n"); return 0; }
	char *author = (char *) malloc(200);
	if(!author) { fprintf(stderr, "Could not malloc author\n"); return 0; }
	char *genre = (char *) malloc(200);
	if(!genre){ fprintf(stderr, "Could not malloc genre\n"); return 0; }
	char *page_count = (char *) malloc(200);
	if(!page_count){ fprintf(stderr, "Could not malloc page_cnt\n"); return 0; }
	char *publisher = (char *) malloc(200);
	if(!publisher){ fprintf(stderr, "Could not malloc publisher\n"); return 0; }
	char *cookie = (char *) malloc(200);
	if(!cookie) { fprintf(stderr, "Could not malloc cookie\n"); return 0; } 
	char *token = (char *) malloc(500);
	if(!token) { fprintf(stderr, "Could not malloc token\n"); return 0;}

	// deschidere conexiune
	int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);

	while(1) {
		// citire comanda
		getline(&buf, &buf_size, stdin);
		// identificare tip de comanda + citire parametrii suplimentari
		// + executare functii de trimitere mesaje HTTP
		if(strcmp(buf, "register\n") == 0){
			read_user_data(buf, buf_size, &user, &pass);
			send_register(user, pass, &sockfd);
		} else if(strcmp(buf, "login\n") == 0){
			read_user_data(buf, buf_size, &user, &pass);
			send_login(user, pass, &sockfd, &cookie);
		} else if(strcmp(buf, "enter_library\n") == 0) {
			enter_library(&sockfd, cookie, &token);
		} else if(strcmp(buf, "get_books\n") == 0) {
			get_books(&sockfd, token);
		} else if(strcmp(buf, "get_book\n") == 0) {
			read_id(buf, buf_size, &id);
			get_book(id, &sockfd, token);
		} else if(strcmp(buf, "add_book\n") == 0) {
			read_book_data(buf, buf_size, &title, &author,
				&genre, &publisher, &page_count);
			add_book(title, author, genre, publisher,
				page_count, &sockfd, token);
		} else if (strcmp(buf, "delete_book\n") == 0) {
			read_id(buf, buf_size, &id);
			delete_book(id, &sockfd, token);
		} else if(strcmp(buf, "logout\n") == 0) {
			logout(&sockfd, &cookie, &token);
		} else if(strcmp(buf, "exit\n") == 0) {
			break;
		}
	}
	// inchidere conexiune si eliberare memorie
	close_connection(sockfd);
	free(token);   		free(cookie);   	free(buf);     	free(user);
	free(pass);			free(title);   		free(author);	free(genre);
	free(publisher);	free(page_count);	free(id);
	return 0;
}