/* Toader Sergiu-Cristian, 325CB */

#ifndef LIST_H
#define LIST_H

// lista de char
typedef struct cell *list;

struct cell {
  char * element;
  list next;
};

list cons(char *element, list l);
list cdr_and_free(list l);
int remove_el(char *el, list *l);

//lista de int-uri
typedef struct cell_int *list_int;

struct cell_int {
	int element;
	list_int next;
};

list_int cons_int(int element, list_int l);
list_int cdr_and_free_int(list_int l);
void remove_el_int(int index, list_int *l);


#endif
