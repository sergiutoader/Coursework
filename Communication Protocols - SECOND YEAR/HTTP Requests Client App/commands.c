#include "commands.h"
	

// extrage cookie-ul dintr-un raspuns la cerere de login
void getCookie(char * response, char **cookie) {
	// cookie-ul se va afla pe linia 8
    int line = 8;
    int it = 0;
    // se parcurge bufferul pana la linia 8
    while(line != 0 && response[it]!= '\0') {
        if(response[it] == '\n') line--;
        it++;
    }
    // bufferul nu contine un cookie (s-a primit un mesaj de eroare)
    if(response[it] == '\0') {
    	return;
    }
    while(response[it] != ' ') {
        it++;
    }
    it++;
    int i = 0;
    while(response[it]!=';') {
        (*cookie)[i] = response[it];
        it++;
        i++;
    }
    (*cookie)[i] = '\0';
}

// extrage payload-ul dintr-un raspuns la cerere de acces la biblioteca
char * get_payload(char * response) {
	char *payload = (char *) malloc(500);
	// continutul payload-lui se afla pe linia 14
	int line = 14, i = 0, j = 0;
	// se parcurge buffer-ul pana la a 14-a linie
	while(line != 0  && response[i] != '\0') {
		if(response[i] == '\n') {
			line--;
		}
		i++;
	}
	// in cazul in care nu exista payload in buffer, se intoarce NULL
	if(response[i] != '{') {
		return NULL;
	}

	// transfer din response in payload
	while(response[i] != '\0') {
		payload[j++] = response[i++];
	}
	payload[j] = '\0';
	return payload;
}

// functie care extrage token-ul din raspuns
void get_token(char *response, char **token) {
	// extragere payload din raspuns
	char * payload = get_payload(response);
	if(!payload) return;
	// se parseaza payload-ul folosind functia json_parse_string
	// din biblioteca parson
    JSON_Value * j_payload = json_parse_string(payload);
    // se extrage intrarea din obiectul j_payload care are drept
    // cheie cuvantul "token" - se foloseste functia
    // json_object_get_string din biblioteca parson
    const char * aux = json_object_get_string(json_object(j_payload), "token");
    // obiectul nu contine intrarea "token" -> cerere invalida
    if(!aux) {
    	json_value_free(j_payload);
    	free(payload);
    	return;
    }
    strcpy(*token, aux);
    // se elibereaza memoria folosita
    json_value_free(j_payload);
    free(payload);
}

// Functii pentru operatii

// functie pentru operatia register
void send_register(char user[], char pass[], int *sockfd) {

	char **data = (char **) malloc(2 * sizeof(char *));
   	// se completeaza payload-ul cu username si password
    data[0] = (char *) malloc(200);
    data[1] = (char *) malloc(200);

    strcpy(data[0], "\"username\":\"");
    strcat(data[0], user);
    strcat(data[0], "\"");
    strcpy(data[1], "\"password\":\"");
    strcat(data[1], pass);
    strcat(data[1], "\"");

    // trimitere request la server
    char * message = compute_post_request(
    	"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080", 
        "/api/v1/tema/auth/register",
        "application/json",
        data, 2, NULL, 0, NULL);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }

    // afisarea raspunsului
    puts(response);
    // eliberare memorie
    free(data[0]);
    free(data[1]);
    free(data);
    free(message);
    free(response);
}

// functie pentru operatia de login
void send_login(char user[], char pass[], int *sockfd, char **cookie) {
	
	char **data = (char **) malloc(2 * sizeof(char *));
	// se completeaza payload-ul cu username si password
    data[0] = (char *) malloc(200);
    data[1] = (char *) malloc(200);
    strcpy(data[0], "\"username\":\"");
    strcat(data[0], user);
    strcat(data[0], "\"");
    strcpy(data[1], "\"password\":\"");
    strcat(data[1], pass);
    strcat(data[1], "\"");

    // trimitere request la server
    char * message = compute_post_request(
    	"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080", 
        "/api/v1/tema/auth/login",
        "application/json",
        data, 2, NULL, 0, NULL);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }

    // extragere cookie din raspuns si salvarea sa in variabila cookie
    getCookie(response, cookie);

    // afisare raspuns
    puts(response);
    // eliberare memorie
    free(data[0]);
    free(data[1]);
    free(data);
    free(message);
    free(response);
}

// realizeaza o cerere de acces la biblioteca
// intoarce un token folosit mai departe penntru a demonstra accesul
// la biblioteca
void enter_library(int *sockfd, char *cookie, char** token) {
	// se seteaza ca payload cookie-ul obtinut la logare
	char ** cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) malloc(200);
	strcpy(cookies[0], cookie);
	// trimitere request la server
	char * message = compute_get_request(
        "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/tema/library/access",
        NULL, cookies, 1, NULL);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }
    // afisare raspuns
    puts(response);

    // extragere content din response + parsare
    get_token(response, token);
    // eliberare memorie
    free(cookies[0]);
    free(cookies);
    free(message);
    free(response);
}

// functie pentru adaugare carte
void add_book(char *title, char* author, char *genre,
	char *publisher, char *page_count, int *sockfd, char *token){

	// completare payload cu date despre carte
	char **data = (char **) malloc(5 * sizeof(char *));
    for(int i = 0; i < 5; i++) {
    	data[i] = (char *) malloc(500);
    }

    strcpy(data[0], "\"title\":\"");
    strcat(data[0], title);
    strcat(data[0], "\"");

    strcpy(data[1], "\"author\":\"");
    strcat(data[1], author);
    strcat(data[1], "\"");

    strcpy(data[2], "\"genre\":\"");
    strcat(data[2], genre);
    strcat(data[2], "\"");

    strcpy(data[3], "\"publisher\":\"");
    strcat(data[3], publisher);
    strcat(data[3], "\"");

    strcpy(data[4], "\"page_count\":\"");
    strcat(data[4], page_count);
    strcat(data[4], "\"");
    // trimitere request la server
    char * message = compute_post_request(
    	"ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080", 
        "/api/v1/tema/library/books",
        "application/json",
        data, 5, NULL, 0, token);
    send_to_server(*sockfd, message);

	// primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }
    // afisare raspuns
    puts(response);

    // eliberare memorie
    for(int i = 0; i < 5; i++) {
    	free(data[i]);
    }
    free(data);
    free(message);
    free(response);
}


// functie pentru afisarea informatiilor despre toate cartile adaugate
void get_books(int *sockfd, char *token){

	// trimitere request la server
	char * message = compute_get_request(
        "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
        "/api/v1/tema/library/books",
        NULL, NULL, 0, token);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }
    // afisare raspuns
    puts(response);
    // eliberare memorie
    free(message);
    free(response);
}

// functie pentru afisarea informatiilor despre o anumita carte
void get_book(char *id, int *sockfd, char *token) {

	// se creeaza path-ul pana la cartea respectiva
	char *aux = (char *) malloc(200);
	strcpy(aux, "/api/v1/tema/library/books/");
	strcat(aux, id);

	// trimitere request la server
	char * message = compute_get_request(
        "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
        aux, NULL, NULL, 0, token);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }
    // afisare raspuns
    puts(response);
    // eliberare memorie
    free(message);
    free(response);
    free(aux);
}

// functie pentru stergerea unei carti
void delete_book(char *id, int *sockfd, char *token) {

	// se creeaza path-ul pana la cartea respectiva
	char *aux = (char *) malloc(200);
	strcpy(aux, "/api/v1/tema/library/books/");
	strcat(aux, id);

	// trimitere request la server
	char * message = compute_delete_request(
        "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080",
        aux, token);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }
    // afisare raspuns
    puts(response);
    // eliberare memorie
    free(message);
    free(response);
    free(aux);
}

// functie pentru comanda logout
void logout(int *sockfd, char **cookie, char **token) {

	// completare payload cu cookie-ul obtinut la logare
	char ** cookies = (char **) malloc(sizeof(char *));
	cookies[0] = (char *) malloc(200);
	strcpy(cookies[0], *cookie);
	// trimitere request la server
	char * message = compute_get_request(
        "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
        "/api/v1/tema/auth/logout",
        NULL, cookies, 1, NULL);
    send_to_server(*sockfd, message);

    // primire raspuns de la server
    char *response = receive_from_server(*sockfd);
    // raspuns de dimensiune 0 - conexiune pierduta
    // se reconecteaza, se retrimite request-ul si se reprimeste raspunsul
    if(strlen(response) == 0) {
    	free(response);
    	*sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
    	send_to_server(*sockfd, message);
    	response = receive_from_server(*sockfd);
    }

    // resetare cookie si token la logout
    memset(*cookie, 0, strlen(*cookie));
    memset(*token, 0, strlen(*token));
    // afisare raspuns
    puts(response);
    // eliberare memorie
    free(cookies[0]);
    free(cookies);
    free(message);
    free(response);
}