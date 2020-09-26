/* Toader Sergiu-Cristian, 325CB */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "helpers.h"

// functie care realizeaza abonarea unui client la un topic
void subscribe(char buffer[], int sockfd, char * **topics,
	int * topic_count, int * topic_max_count) {

	char command[9];
	tcp_server_msg msg;
	msg.type = 0;
	sscanf(buffer, "%s %s %hhu", command, msg.topic, &(msg.sf));

	// se trimite mesaj la server
	int n = send(sockfd, &msg, sizeof(tcp_server_msg), 0);
	DIE(n < 0, "send");
				
	printf("subscribed %s\n", msg.topic);

	for(int i = 0; i < *topic_count; i++) {
		printf("%s ", (*topics)[i]);
	}
	printf("\n");

	// se adauga topicul in lista de topicuri a clientului
	(*topics)[*topic_count] = (char *) malloc(TOPIC_LEN);
	if(!((*topics)[*topic_count])) {fprintf(stderr, "Alocare esuata\n");}

	memcpy((*topics)[*topic_count], msg.topic, TOPIC_LEN);
	(*topic_count)++;
	if((*topic_count) == (*topic_max_count)) {
		(*topic_max_count) *= 2;
		*topics = (char **) realloc(*topics, *topic_max_count * sizeof(char *));
		if(!(*topics)) {fprintf(stderr, "Alocare esuata\n");}
	}
}

// functie care realizeaza dezabonarea unui client de la un topic
void unsubscribe(char buffer[], int sockfd, char * **topics, int * topic_count) {
	char command[11];
	tcp_server_msg msg;
	msg.type = 1;
	sscanf(buffer, "%s %s", command, msg.topic);

	// Daca clientul nu este abonat la topic, se afiseaza un
	// mesaj de eroare
	int topic_idx = topic_taken(*topics, *topic_count, msg.topic);
	if(topic_idx == -1) {
		printf("You are not subscribed to this topic.\n");
		return;
	}
	// altfel, se elimina din lista de clienti acel abonat
	else {
		for(int k = topic_idx + 1; k < *topic_count; k++) {
			memcpy((*topics)[k-1], (*topics)[k], TOPIC_LEN);
		}
		free((*topics)[*topic_count - 1]);
		(*topic_count)--;
	}
	// se trimite mesaj la server
	int n = send(sockfd, &msg, sizeof(tcp_server_msg), 0);
	DIE(n < 0, "send");

	printf("unsubscribed %s\n", msg.topic);
}

// refacerea listei de topicuri la care clientul este abonat
void restore_topics(server_tcp_msg msg, char * **topics,
	int *topic_count, int *topic_max_count) {

	(*topics)[*topic_count] = (char *) malloc(TOPIC_LEN);
	if((*topics)[*topic_count] == NULL) {
		fprintf(stderr, "Alocare esuata\n");
		return;
	}
					
	// adaugare topic nou
	memcpy((*topics)[*topic_count], msg.ip + 6, TOPIC_LEN);
	(*topic_count)++;

	if(*topic_count == *topic_max_count) {
		(*topic_max_count) *= 2;
		*topics = (char **) realloc(*topics, (*topic_max_count) * sizeof(char *));
		if(*topics == NULL) {
			fprintf(stderr, "Alocare esuata\n");
			return;
		}
	}
}

int main(int argc, char *argv[]) {
	DIE(argc < 3, "argc");

	// structura de date care retine topicurile la care clientul este abonat
	int topic_count = 0, topic_max_count = 1;
	char ** topics = (char **) calloc(topic_max_count, sizeof(char *));
	if(!topics) {fprintf(stderr, "Insufficient memory\n"); return 0;}

	int sockfd, ret, fdmax;
	struct sockaddr_in serv_addr;

	fd_set read_fds;
	fd_set tmp_fds;

	// se goleste multimea de descriptori de citire (read_fds)
	// si multimea temporara (tmp_fds)
	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	// creare socket nou
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(sockfd < 0, "socket");
	fdmax = sockfd;

	// se adauga 0 si sockfd in multimea de descriptori
	FD_SET(sockfd, &read_fds);
	FD_SET(STDIN_FILENO, &read_fds);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(atoi(argv[3]));
	ret = inet_aton(argv[2], &serv_addr.sin_addr);
	DIE(ret == 0, "inet_aton");

	// se realizeaza conexiunea pe socket
	ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	DIE(ret < 0, "connect");
	
	// se trimite la server dupa conectare ID-ul
	int id = send(sockfd, argv[1], sizeof(argv[1]), 0);
	DIE (id < 0, "ID");

	while (1) {
		tmp_fds = read_fds; 
		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		// citire informatii de la STDIN
		if (FD_ISSET(STDIN_FILENO, &tmp_fds)) {

			char buffer[BUFLEN];
			memset(buffer, 0, BUFLEN);
			fgets(buffer, BUFLEN - 1, stdin);

			// daca s-a introdus "exit", se inchide clientul
			if (strcmp(buffer, "exit\n") == 0) {
				break;
			}

			// verificare validitate input
			if(invalid_input(buffer)) {
				printf("Invalid input.\n");
				continue;
			}

			if(topic_taken(topics, topic_count, get_topic(buffer)) != -1) {
				printf("Already subscribed to this topic.\n");
				continue;			
			}

			// abonare la topic
			if(strncmp(buffer, "subscribe", 9) == 0) {
				subscribe(buffer, sockfd, &topics, &topic_count, &topic_max_count);
			}

			// dezabonare la topic
			else if(strncmp(buffer, "unsubscribe", 11) == 0) {
				unsubscribe(buffer, sockfd, &topics, &topic_count);
			}
		}
		// primire informatii de la server
  		if (FD_ISSET(sockfd, &tmp_fds)) {

  			server_tcp_msg msg;
  			memset(&msg, 0, sizeof(server_tcp_msg));

   			int m = recv(sockfd, &msg, sizeof(msg), 0);
			DIE(m < 0, "recv");


			if(m == 0) {
				printf("Lost connection to server.\n");
				break;
			}

			if(strcmp(msg.ip, "taken") == 0) {
				printf("Error: User with this ID is currently online.\n");
				break;
			}

			if(strncmp(msg.ip, "topic ", 6) == 0) {
				// refacerea listei de topicuri la care clientul este abonat//
				restore_topics(msg, &topics, &topic_count, &topic_max_count);
			} else {
				// afisare mesaj primit
				print_message(msg);
			}
  		}	
	}
	// se goleste multimea de descriptori de citire (read_fds)
    // si multimea temporara (tmp_fds)
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);
	
	int c = close(sockfd);
	DIE(c < 0, "close");
	return 0;
}