/* Toader Sergiu-Cristian, 325CB */

#include "helpers.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

// --- Functii pentru subscriber ---

// functie care realizeaza transformarea payload-ului intr-un
// string cu valoarea unui INT
char * print_int(char payload[]) {
	int result = 0;
	if(payload[0] == 0) {
		result = 1;
	} else if(payload[0] == 1){
		result = -1;
	}

	uint32_t number = ntohl(*((uint32_t *) (payload + 1)));
	
	result = (int) (result * number);
	char * result_str = (char *) malloc(20);
	sprintf(result_str, "%d", result);
	return result_str;

}
// functie care realizeaza transformarea payload-ului intr-un
// string cu valoarea unui SHORT_REAL
char * print_short_real(char payload[]) {
	uint16_t number = ntohs(*((uint16_t *) payload));

	double result = (double) number;

	char * result_str = (char *) malloc(20);
	sprintf(result_str, "%.2f", result/100);
	return result_str;
}

// functie care realizeaza transformarea payload-ului intr-un
// string cu valoarea unui FLOAT
char * print_float(char payload[]) {

	int sign = 0;
	if(payload[0] == 0) {
		sign = 1;
	} else if(payload[0] == 1){
		sign = -1;
	}
	// se extrag mantisa si exponentul
	uint32_t mantisa = ntohl(*((uint32_t *) (payload  + 1)));
	uint8_t exponent = payload[5];

	double result = (double) mantisa;
	result *= sign;

	// se realizeaza operatia result = mantisa * 10^(-exponent)
	for(int i = 0; i < exponent; i++) {
		result /= 10;
	}

	char * result_str = (char *) malloc(20);
	sprintf(result_str, "%.6f", result);
	return result_str;
}

// functie care afiseaza mesajul primit de la server
void print_message(server_tcp_msg msg) {
	char output[2000];
	sprintf(output, "%s:%hu - %s - ", msg.ip, ntohs(msg.port), msg.udp.topic);
	switch(msg.udp.data_type) {
		// in functie de data_type, se realizeaza
		// interpretarea si afisarea payload-ului
		case(INT):
			strcat(output, "INT - ");
			strcat(output, print_int(msg.udp.payload));
			break;
		case(SHORT_REAL):
			strcat(output, "SHORT_REAL - ");
			strcat(output, print_short_real(msg.udp.payload));
			break;
		case(FLOAT):
			strcat(output, "FLOAT - ");
			strcat(output, print_float(msg.udp.payload));
			break;
		case(STRING):
			strcat(output, "STRING - ");
			strcat(output, msg.udp.payload);
			break;
		default:
			strcat(output, "INVALID FORMAT");
	}

	printf("%s\n", output);
}

//verifica daca inputul este invalid
int invalid_input(char * buffer) {
	int word_count = 1;
	int i = 0;
	int last_space_poz;
	while(buffer[i] != '\0') {
		if(buffer[i] == ' ') {
			word_count++;
			last_space_poz = i;
		}
		i++;
	}
	// comanda subscribe
	if(word_count == 3)  {
		char command[1000];
		sscanf(buffer, "%s", command);
		if(strlen(command) != 9) {
			return 1;
		}
		// primul cuvant nu este subscribe
		if(strncmp(buffer, "subscribe", 9) != 0) {
			return 1;
		}
		int second_param_length = last_space_poz - 10;
		// al doilea parametru mai mare de 50 de caractere
		if(second_param_length > TOPIC_LEN) {
			return 1;
		}
		// dimensiunea ultimului parametru diferita de 1 si
		// caracterul diferit de 0 sau 1
		int third_param_length = strlen(buffer) - last_space_poz - 2;
		if(third_param_length != 1 || (buffer[last_space_poz + 1] != '0'
			&& buffer[last_space_poz + 1] != '1')) {
			return 1;
		}
		// altfel, comanda este corecta
		return 0;
	}
	// comanda unsubscribe
	if(word_count == 2) {
		// lungimea primului cuvant nu e 11
		char command[1000];
		sscanf(buffer, "%s", command);
		if(strlen(command) != 11) {
			return 1;
		}
		// primul cuvant nu este unsubscribe
		if(strncmp(buffer, "unsubscribe", 11) != 0) {
			return 1;
		}

		int second_param_length = strlen(buffer) - 13;
		// topicul mai mare de 50 de caractere
		if(second_param_length > TOPIC_LEN) {
			return 1;
		}

		return 0;
	}
	// numar incorect de parametri
	return 1;
}

// functie care verifica daca topicul se afla in lista de topicuri
int topic_taken(char ** topics, int topic_count, char * topic) {

	for(int i = 0; i < topic_count; i++) {
		if(strcmp(topics[i], topic) == 0) {
			return i;
		}
	}
	return -1;
}

// extrage topic-ul din buffer
char * get_topic(char * buffer) {
	char * topic = (char *) malloc(TOPIC_LEN);
	if(!topic){fprintf(stderr, "Alocare esuata\n"); return NULL;}

	// indicele de inceput al topicului
	int idx = 10;

	int count = 0;

	// extrag topicul
	while(buffer[idx] != ' ') {
		topic[count++] = buffer[idx++];
	}
	topic[count] = '\0';
	return topic;
}


// --- Functii pentru server ---

// functie care intoarce indicele clientului din lista de
// clienti sau -1 daca clientul nu se afla in lista
int is_tcp_client(int cl, client tcp_client_list[], int tcp_client_count) {
	for(int i = 0; i < tcp_client_count; i++) {
		if(tcp_client_list[i].sockfd == cl) {
			return i;
		}
	}
	return -1;
}

int get_sock(char id[], client tcp_client_list[], int tcp_client_count) {
	for(int i = 0; i < tcp_client_count; i++) {

		if(strcmp(tcp_client_list[i].id, id) == 0) {
			return tcp_client_list[i].sockfd;
		}
	}
	return -1;
}

// functie care intoarce indicele unui topic din vectorul de topic-uri sau -1
int get_topic_index(topic_user_list topics[], int topics_count, char topic[]) {
	for(int i = 0; i < topics_count; i++) {
		if(strcmp(topic, topics[i].topic) == 0) {
			return i;
		}
	}
	return -1;
}

// functie care creaza un topic nou cu un utilizator abonat
topic_user_list create_topic(char topic[], char id[]) {
	topic_user_list result;
	result.topic = (char *)malloc(50);
	if(!result.topic) {fprintf(stderr, "Insuficienta memorie\n");}
	memcpy(result.topic, topic, strlen(topic));
	result.clienti = cons(id, NULL);

	return result;
}

// functie care adauga un abonat la lista de abonati a unui topic
topic_user_list add_subscriber(topic_user_list list, char id[]) {
	list.clienti = cons(id, list.clienti);
	return list;
}

// verifica daca clientul se afla in lista de utilizatori online
int is_online(char id[], client * online_users, int online_users_count) {
	for(int i = 0; i < online_users_count; i++) {
		if(strcmp(online_users[i].id, id) == 0) {
			return 1;
		}
	}
	return 0;
}

// Functii pentru partea de store and forward

// intoarce indicele utilizatorului din lista de utilizatori offline
// sau -1 daca acesta nu exista in lista
int is_offline(char id[], offline_client_queue offline_users[],
	int offline_users_count) {
	for(int i = 0; i < offline_users_count; i++) {
		if(strcmp(offline_users[i].offline_client.id, id) == 0) {
			return i;
		}
	}
	return -1;
}

// trimite la destinatie toate pachetele in asteptare ale utilizatorului
// nou intrat
void send_pending_msg(offline_client_queue user, int sockfd) {
	msg_list aux = user.list;
	while (aux) {
		int n = send(sockfd, &(aux->element), sizeof(server_tcp_msg), 0);
		DIE(n < 0, "send");
		aux = aux->next;
	}

	list topics = user.offline_client.topics;
	while(topics) {
		char * msg = (char *) calloc(1, sizeof(server_tcp_msg));
		if(!msg){fprintf(stderr, "Alocare esuata\n"); return;}
		
		memcpy(msg, "topic ", 6);
		memcpy(msg + 6, topics->element, TOPIC_LEN);

		int n = send(sockfd, msg, sizeof(server_tcp_msg), 0);
		DIE(n < 0, "send");

		topics = topics->next;
	}
}

// functie care adauga la lista de mesaje in asteptare un mesaj nou 
msg_list add_to_queue(msg_list list, server_tcp_msg msg) {
	list = cons_msg(msg, list);
	return list;
}

// functie care intoarce 1 daca utilizatorul este abonat
// la vreun topic cu SF = 1 sau 0 in caz contrar 
int has_sf_topics(list_int list) {
	list_int aux = list;
	while(aux) {
		if(aux->element == 1) {
			return 1;
		}
		aux = aux->next;
	}
	return 0;
}

// elimina toate topicurile care au SF = 0 din lista de topicuri a clientului
client remove_unstored_topics(client cl) {
	list topics = cl.topics;
	list_int sf_values = cl.sf_values;

	while(topics && sf_values) {
		if(sf_values->element == 0) {
			int idx = remove_el(topics->element, &cl.topics);
			remove_el_int(idx, &cl.sf_values);
		}
		topics = topics->next;
		sf_values = sf_values->next;
	}
	return cl;
}
