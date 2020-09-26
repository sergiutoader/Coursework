#include "read.h"

// functii pentru citirea input-ului de la tastatura

// ofera prompt si realizeaza citirea username-ului si parolei
void read_user_data(char *buf, size_t buf_size, char* user[], char* pass[]) {
	printf("username=");
	getline(&buf, &buf_size, stdin);
	memcpy(*user, buf, strlen(buf));
	(*user)[strlen(buf) - 1] = '\0';
	printf("password=");
	getline(&buf, &buf_size, stdin);
	memcpy(*pass, buf, strlen(buf));
	(*pass)[strlen(buf) - 1] = '\0';
}

// ofera prompt si realizeaza citirea id-ului
void read_id(char *buf, size_t buf_size, char ** id) {
	printf("id=");
	getline(&buf, &buf_size, stdin);
	sscanf(buf, "%s\n", *id);
}

// ofera prompt si realizeaza citirea datelor despre o carte
void read_book_data(char *buf, size_t buf_size, char **title, char ** author,
	char **genre, char **publisher, char **page_count) {
	printf("title=");
	getline(&buf, &buf_size, stdin);
	memcpy(*title, buf, strlen(buf));
	(*title)[strlen(buf) - 1] = '\0';
	printf("author=");
	getline(&buf, &buf_size, stdin);
	memcpy(*author, buf, strlen(buf));
	(*author)[strlen(buf) - 1] = '\0';
	printf("genre=");
	getline(&buf, &buf_size, stdin);
	memcpy(*genre, buf, strlen(buf));
	(*genre)[strlen(buf) - 1] = '\0';
	printf("publisher=");
	getline(&buf, &buf_size, stdin);
	memcpy(*publisher, buf, strlen(buf));
	(*publisher)[strlen(buf) - 1] = '\0';
	printf("page_count=");
	getline(&buf, &buf_size, stdin);
	memcpy(*page_count, buf, strlen(buf));
	(*page_count)[strlen(buf) - 1] = '\0';
}