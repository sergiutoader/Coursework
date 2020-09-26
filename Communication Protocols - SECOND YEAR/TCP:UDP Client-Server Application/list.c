/* Toader Sergiu-Cristian, 325CB */

#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

list cons(char *element, list l) {
	list temp = malloc(sizeof(struct cell));
    if(!temp) {printf("Alocare esuata\n");return NULL;}
    temp->element = (char *) malloc(60);
    if(!temp->element) {printf("Alocare esuata\n");return NULL;}
	memcpy(temp->element, element, strlen(element));
	temp->next = l;
	return temp;
}

list cdr_and_free(list l) {
	list temp = l->next; 
	free(l->element);
    free(l);
	return temp;
}

// Functie care elimina din lista un element
int remove_el(char * el, list *l) {
	list temp = *l;
	list prev;
	int nr = 0;

    // Daca elementul este primul
    if (temp != NULL && strcmp(temp->element, el) == 0) {
        *l = temp->next; 
        free(temp); 
        return nr; 
    } 
  
    // Se parcurge lista si se verifica egalitatea
    while (temp != NULL && strcmp(temp->element, el) != 0) { 
        prev = temp; 
        temp = temp->next; 
        nr++;
    } 
  
    // Nu s-a gasit elementul in lista 
    if (temp == NULL) return -1; 
  
    // se rupe legatura listei de element
    prev->next = temp->next; 
  
    free(temp);  // eliberarea memoriei
    return nr;
}


// Functii pentru liste de int-uri

// adaugare element in lista
list_int cons_int(int element, list_int l) {
	list_int temp = malloc(sizeof(struct cell_int));
    if(!temp) {printf("Alocare esuata\n");return NULL;}
    temp->element = element;
	temp->next = l;
	return temp;
}

list_int cdr_and_free_int(list_int l) {
	list_int temp = l->next; 
    free(l);
	return temp;
}


// elimina din lista un element de pe o pozitie data
void remove_el_int(int index, list_int *l) {
	list_int temp = *l;
	list_int prev;

	// Eliminare primul element
    if (temp != NULL && index == 0) { 
        *l = temp->next;  
        free(temp); 
        return; 
    } 
  
    // Se parcurge lista si se verifica egalitatea
    while (temp != NULL && index != 0) { 
        prev = temp; 
        temp = temp->next; 
        index--;
    } 
  
    // Nu s-a gasit elementul in lista 
    if (temp == NULL) return; 
  
    // se rupe legatura listei de element
    prev->next = temp->next; 
  
    free(temp);
}
