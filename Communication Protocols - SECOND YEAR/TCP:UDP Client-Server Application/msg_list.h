/* Toader Sergiu-Cristian, 325CB */

#include "helpers.h"

#ifndef MSG_LIST_H
#define MSG_LIST_H

typedef struct msg_cell *msg_list;

struct msg_cell {
  server_tcp_msg element;
  msg_list next;
};


msg_list cons_msg(server_tcp_msg element, msg_list l);

#endif
