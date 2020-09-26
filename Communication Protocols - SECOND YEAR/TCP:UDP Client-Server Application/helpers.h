/* Toader Sergiu-Cristian, 325CB */

#ifndef _HELPERS_H
#define _HELPERS_H 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "list.h"

/*
 * Macro de verificare a erorilor
 * Exemplu:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)

#define TOPIC_LEN	50
#define BUFLEN		1551	// dimensiunea maxima a calupului de date
#define MAX_CLIENTS	20		// numarul maxim de clienti in asteptare
#define INT 		0
#define SHORT_REAL	1
#define FLOAT		2
#define STRING		3

// structura pentru un mesaj pe ruta: client udp -> server
typedef struct {
	char topic[TOPIC_LEN];
	uint8_t data_type;
	char payload[1500];
} udp_msg;

// structura pentru un mesaj pe ruta: server -> client tcp
typedef struct {
	char ip[20];
	unsigned short port;
	udp_msg udp;
} server_tcp_msg;

// structura pentru un mesaj pe ruta: client tcp -> server
typedef struct {
	uint8_t type; // 0 = subscribe; 1 = unsubscribe
	char topic[TOPIC_LEN];
	uint8_t sf; // 0/1 - variabila pentru store & forward

} tcp_server_msg;

// structura pentru un client
typedef struct {
	char id[10];
	int sockfd;
	list topics;
	list_int sf_values;
} client;

// structura pentru lista de clienti a unui topic
typedef struct {
	char *topic;
	list clienti;
} topic_user_list;

#include "msg_list.h"
// structura pentru retinerea clientilor offline
typedef struct {
	client offline_client;
	msg_list list;
} offline_client_queue;



// Functii pentru subscriber
char * print_int(char payload[]);
char * print_short_real(char payload[]);
char * print_float(char payload[]);
void print_message(server_tcp_msg msg);
int invalid_input(char * buffer);
int topic_taken(char ** topics, int topic_count, char * topic);
char * get_topic(char * buffer);

// --- Functii pentru server ---
int is_tcp_client(int cl, client tcp_client_list[], int tcp_client_count);
int get_sock(char id[], client tcp_client_list[], int tcp_client_count);
int get_topic_index(topic_user_list topics[], int topics_count, char topic[]);
topic_user_list create_topic(char topic[], char id[]);
topic_user_list add_subscriber(topic_user_list list, char id[]);
int is_online(char id[], client * online_users, int online_users_count);
int is_offline(char id[], offline_client_queue offline_users[],
	int offline_users_count);
void send_pending_msg(offline_client_queue user, int sockfd);
msg_list add_to_queue(msg_list list, server_tcp_msg msg);
int has_sf_topics(list_int list);
client remove_unstored_topics(client cl);


#endif
