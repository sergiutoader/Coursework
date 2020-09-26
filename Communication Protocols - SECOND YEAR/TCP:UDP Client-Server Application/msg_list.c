/* Toader Sergiu-Cristian, 325CB */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "msg_list.h"

// Functie pentru construirea listelor de mesaje
msg_list cons_msg(server_tcp_msg element, msg_list l) {
	msg_list temp = malloc(sizeof(struct msg_cell));
    if(!temp) {printf("Alocare esuata\n");return NULL;}
    temp->element = element;
	temp->next = l;
	return temp;
}
