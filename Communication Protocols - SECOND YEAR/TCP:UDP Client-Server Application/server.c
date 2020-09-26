/* Toader Sergiu-Cristian, 325CB */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include "helpers.h"

int main(int argc, char *argv[]) {

// Structuri de date folosite:

    // lista de clienti online
    int online_users_count = 0;
    long unsigned int online_users_max_count = 2;
    client * online_users = (client *) calloc(online_users_max_count,
        sizeof(client));
    if(!online_users){fprintf(stderr, "Insuficienta memorie\n"); return 0;}
    

    //lista de clienti offline
    int offline_users_count = 0;
    long unsigned int offline_users_max_count = 2;
    offline_client_queue * offline_users = (offline_client_queue *)
        calloc(offline_users_max_count, sizeof(offline_client_queue));
    if(!offline_users){fprintf(stderr, "Insuficienta memorie\n"); return 0;}
    

    // lista de topic-uri
    int topics_count = 0;
    long unsigned int topics_max_count = 2;
    topic_user_list * topics = (topic_user_list *) calloc(topics_max_count,
        sizeof(offline_client_queue));
    if(!topics){fprintf(stderr, "Insuficienta memorie\n"); return 0;}

    // socket_tcp, socket_udp, si port number
    int sockfd_tcp, sockfd_udp, portno;
    int ret;

    // structuri sockaddr_in
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    fd_set read_fds;    // multimea de citire folosita in select()
    fd_set tmp_fds;     // multime folosita temporar
    int fdmax;          // valoare maxima fd din multimea read_fds

    DIE(argc < 2, "argc");

    // se goleste multimea de descriptori de citire (read_fds)
    // si multimea temporara (tmp_fds)
    FD_ZERO(&read_fds);
    FD_ZERO(&tmp_fds);

    // se deschide socket-ul pentru conectarea cu clientii TCP
    sockfd_tcp = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd_tcp < 0, "socket tcp");

    // dezactivare algoritm neagle pentru socket-ul TCP
    int flag = 1;
    int result = setsockopt(sockfd_tcp, IPPROTO_TCP, TCP_NODELAY,
        (char *) &flag, sizeof(int));
    DIE(result < 0, "setsockopt");

    portno = atoi(argv[1]);
    DIE(portno == 0, "atoi");

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // bind si listen pentru socket tcp
    ret = bind(sockfd_tcp, (struct sockaddr *) &serv_addr,
        sizeof(struct sockaddr));
    DIE(ret < 0, "bind");

    ret = listen(sockfd_tcp, MAX_CLIENTS);
    DIE(ret < 0, "listen");

    // se deschide socket-ul pentru comunicarea cu clientii UDP
    sockfd_udp = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(sockfd_udp < 0, "socket udp");

    // bind socket udp
    ret = bind(sockfd_udp, (struct sockaddr *) &serv_addr,
        sizeof(struct sockaddr));
    DIE(ret < 0, "bind");

    // se adauga STDIN_FILENO la multime
    FD_SET(STDIN_FILENO, &read_fds);
    // se adauga noul file descriptor (socketul pe care se asculta
    // conexiuni tcp) in multimea read_fds
    FD_SET(sockfd_tcp, &read_fds);
    fdmax = sockfd_tcp;
    // se adauga noul file descriptor (socketul pe care se ascult
    // conexiuni udp) in multimea read_fds
    FD_SET(sockfd_udp, &read_fds);
    if(sockfd_udp > fdmax) {
        fdmax = sockfd_udp;
    }

    while (1) {
        tmp_fds = read_fds; 
        
        ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
        DIE(ret < 0, "select");

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &tmp_fds)) {

                // s-a primit o cerere de conectare pe socket-ul
                // TCP (cel care face listen)
                if(i == sockfd_tcp) {
                    clilen = sizeof(cli_addr);
                    int new_client = accept(sockfd_tcp, (struct sockaddr *)
                        &cli_addr, &clilen);
                    DIE(new_client < 0, "accept");

                    // dezactivare algoritm neagle pentru socket-ul TCP
                    int flag = 1;
                    result = setsockopt(new_client, IPPROTO_TCP, TCP_NODELAY,
                        (char *) &flag, sizeof(int));
                    DIE(result < 0, "setsockopt");

                    // se adauga noul id in baza de date
                    char new_id[10];

                    int r = recv(new_client, new_id, sizeof(new_id), 0);
                    DIE(r < 0, "recv");

                    // se verifica daca id_ul este utilizat deja
                    if(is_online(new_id, online_users, online_users_count)) {
                        // se trimite mesajul de eroare "taken"
                        server_tcp_msg error_msg;
                        strcpy(error_msg.ip, "taken");

                        int s = send(new_client, &error_msg, sizeof(error_msg) , 0);
                        DIE(s < 0, "send");
                        // inchidere socket
                        int c = close(new_client);
                        DIE(c < 0, "close");
                        continue;
                    }

                    // verific daca userul a mai fost conectat inainte
                    int w = is_offline(new_id, offline_users, offline_users_count);
                    if(w != -1) {

                        // trimit toate mesajele in asteptare; pe portul nou
                        send_pending_msg(offline_users[w], new_client);

                        for(int j = w + 1; j < offline_users_count; j++) {
                            offline_users[j-1] = offline_users[j];
                        }
                        memset(&(offline_users[offline_users_count - 1]), 0,
                            sizeof(offline_users[offline_users_count - 1]));
                        offline_users_count--;
                    }

                    // se adauga utilizatorul in lista de clienti online
                    strcpy(online_users[online_users_count].id, new_id);
                    online_users[online_users_count].sockfd = new_client;
                    online_users_count++;

                    // daca s-a atins capacitatea maxima a listei, aceasta se
                    // realoca cu capacitate dubla
                    if(online_users_count == online_users_max_count) {
                        online_users_max_count *= 2;

                        online_users = (client *) realloc(online_users,
                            online_users_max_count * sizeof(client));
                        if(!online_users){fprintf(stderr,
                            "Insuficienta memorie\n"); return 0;}
                    }

                    // Afisare conectare client
                    printf("New client (%s) connected from %s:%u.\n", new_id,
                        inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
                    FD_SET(new_client, &read_fds);
                    if (new_client > fdmax) { 
                        fdmax = new_client;
                    }
                }
                // s-au primit datagrame pe socket-ul UDP
                else if(i == sockfd_udp) {
                    udp_msg msg;
                    memset(&msg, 0, sizeof(udp_msg));

                    clilen = sizeof(cli_addr);
                    int r = recvfrom(sockfd_udp, &msg, sizeof(udp_msg), 0,
                        (struct sockaddr *) &cli_addr, &clilen);
                    DIE(r < 0, "recvfrom");

                    // se verifica daca topic-ul exista deja
                    int idx = get_topic_index(topics, topics_count, msg.topic);

                    // topicul exista -> se construieste pachetul cu ip, port, mesaj
                    // se trimite pachetul la toti clientii din lista topic-ului
                    if(idx != -1) {

                        server_tcp_msg tcp_msg;
                        strcpy(tcp_msg.ip, inet_ntoa(cli_addr.sin_addr));
                        tcp_msg.port = cli_addr.sin_port;   
                        tcp_msg.udp = msg;
                        
                        list aux = topics[idx].clienti;
                        // se trimite mesajul la toti clientii abonati
                        while(aux) {
                            // se obtine socket-ul corespunzator id-ului
                            int sock = get_sock(aux->element, online_users,
                                online_users_count);

                            // clientul este online
                            if(sock != -1) {
                                // se trimite mesaj la clientul tcp
                                int n = send(sock, &tcp_msg,
                                    sizeof(server_tcp_msg), 0);
                                DIE(n < 0, "send");
                            }
                            // clientul offline
                            else {
                                // se adauga pachetul in coada de mesaje a clientului offline
                                int idx = is_offline(aux->element,
                                    offline_users, offline_users_count);
                                offline_users[idx].list = add_to_queue(
                                    offline_users[idx].list, tcp_msg);

                            }
                            aux = aux->next;
                        }       
                    }
                }
                // daca s-au primit date de la STDIN - se ignora cu exceptia comenzii exit
                else if(i == STDIN_FILENO) {
                    char buffer[BUFLEN];
                    memset(buffer, 0, BUFLEN);
                    fgets(buffer, BUFLEN - 1, stdin);
                    // daca s-a primit "exit", inchid toti socketii si ies din program
                    if(strcmp(buffer, "exit\n") == 0) {
                        for(int j = 1; j <= fdmax; j++) {
                            if(FD_ISSET(j, &read_fds)) {
                                int c = close(j);
                                DIE(c < 0, "close");
                            }
                        }
                        return 0;
                    }
                }
                else {

                    // mesaj de pe un socket deschis al tcp
                    // daca a venit de la un client tcp, se realizeaza subscribe
                    // / unsubscribe (se adauga in lista topic-ului)
                    int cl = is_tcp_client(i, online_users, online_users_count);
                    if(cl != -1) {
                        // recv mesaj
                        tcp_server_msg msg;
                        memset(&msg, 0, sizeof(msg));

                        int n = recv(i, &msg, sizeof(msg), 0);
                        DIE(n < 0, "recv");

                        // clientul s-a deconectat
                        if(n == 0) {
                            printf("Client (%s) disconnected.\n",
                                online_users[cl].id);
                            close(i);

                            // Daca utilizatorul are pe topicul respectiv SF = 0
                            list aux = online_users[cl].topics;
                            list_int aux_sf = online_users[cl].sf_values;
                            // se elimina subscriberul din lista tuturor topicurilor
                            // atunci cand se deconecteaza (pentru SF = 0)
                            while(aux && aux_sf) {
                                // daca valoarea sf a utilizatorului pentru topicul
                                // respectiv este 1, nu se realizeaza eliminarea
                                if(aux_sf->element == 0) {
                                    for(int j = 0; j < topics_count; j++) {

                                        if(strcmp(topics[j].topic,
                                            aux->element) == 0) {

                                            remove_el(online_users[cl].id,
                                                &(topics[j].clienti));
                                            // daca topic-ul nu mai are abonati, se elimina din lista de topic-uri
                                            if(topics[j].clienti == NULL) {
                                            
                                                for(int k = j + 1; k <
                                                    topics_count; k++) {
                                                    topics[k-1] = topics[k];
                                                }
                                                topics_count--;
                                                break;
                                            }
                                        }
                                    }
                                }
                                aux = aux->next;
                                aux_sf = aux_sf->next;
                            }

                            // se adauga utilizatorul cu toate topicurile sale in coada
                            if(has_sf_topics(online_users[cl].sf_values)) {
                                // i se elimina toate topicurile cu sf - 0
                                online_users[cl] = remove_unstored_topics(
                                    online_users[cl]);

                                // se seteaza campurile
                                offline_users[offline_users_count].offline_client
                                    = online_users[cl];

                                offline_users[offline_users_count].list = NULL;
                                offline_users_count++;
                                // realocare cu capacitate dubla daca s-a atins capacitatea maxima
                                if(offline_users_count == offline_users_max_count) {
                                    offline_users_max_count *= 2;

                                    offline_users = (offline_client_queue *) realloc(offline_users,
                                        offline_users_max_count * sizeof(offline_client_queue));
                                    if(!offline_users){fprintf(stderr,
                                        "Insuficienta memorie\n"); return 0;}
                                }
                            }

                            for(int j = cl + 1; j < online_users_count; j++) {
                                online_users[j-1] = online_users[j];
                            }
                            online_users_count--;
                        
                            // se scoate din multimea de citire socketul inchis 
                            FD_CLR(i, &read_fds);
                        }
                        // subscribe client la topic
                        else if(msg.type == 0){

                            // se verifica daca topic-ul exista deja
                            int idx = get_topic_index(topics, topics_count,
                                msg.topic);
                            if(idx == -1) {
                                
                                // adauga topic-ul la lista de topic-uri
                                // realocare cu capacitate dubla daca s-a atins capacitatea maxima
                                topics[topics_count++] = create_topic(msg.topic,
                                    online_users[cl].id);
                                if(topics_count == topics_max_count) {
                                    topics_max_count *= 2;
                                    topics = (topic_user_list *) realloc(topics,
                                        topics_max_count * sizeof(topic_user_list));
                                    if(!topics){fprintf(stderr, "Insuficienta memorie\n"); return 0;}
                                }
                    
                            } else {
                                // adauga subscriber la topicul deja existent
                                topics[idx] = add_subscriber(topics[idx],
                                    online_users[cl].id);
                            }
                            // adauga topic-ul la lista de topic-uri ale utilizatorului
                            online_users[cl].topics = cons(msg.topic,
                                online_users[cl].topics);
                            // adauga valoarea sf la lista de valori sf ale utilizatorului
                            online_users[cl].sf_values = cons_int(msg.sf,
                                online_users[cl].sf_values);
                        } 
                        // unsubscribe client la topic
                        else if(msg.type == 1) {
                            int idx = get_topic_index(topics, topics_count,
                                msg.topic);
                            if(idx != -1) {
                                // se elimina subscriberul din lista topicului
                                remove_el(online_users[cl].id,
                                    &(topics[idx].clienti));
                                // se elimina topicul din lista subscriberului
                                int sf_idx = remove_el(msg.topic,
                                    &(online_users[cl].topics));
                                // se elimina valorarea SF a acelui topic din lista subscriberului
                                remove_el_int(sf_idx, &(online_users[cl].sf_values));
                                // daca topic-ul nu mai are abonati, se elimina din lista de topic-uri
                                if(topics[idx].clienti == NULL) {
                                    for(int k = idx + 1; k < topics_count; k++) {
                                        topics[k-1] = topics[k];
                                    }
                                    topics_count--;
                                }
                            }
                        }
                    } 
                }
            }
        }
    }
    return 0;
}