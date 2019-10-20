/* TOADER Sergiu-Cristian - 315CB */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1000


/* --- Structura istoric comenzi --- */
typedef struct history
{
	char info[MAX];
	struct history * urm;
} TCH, * TLH, ** ALH;

/* --- Structura info card --- */ 
typedef struct cardInfo
{
	unsigned long long card_number;
	char pin[6];
	char initial_pin[6];
	char expiry_date[6];
	unsigned int cvv;
	int balance;
	char status[10];
	int wrong_pin;
	TLH history;
} cardInfo;

/* --- Structura sublista de carduri --- */
typedef struct TCard 
{
	cardInfo info;
	struct TCard * urm;
} TCard, * LSC, ** ALSC;

/* --- Structura LC --- */
typedef struct TLista
{
	LSC lista;
	struct TLista * urm;
} TLista, * LC, ** ALC;


/* --- functii specifice prelucrarii cardurilor + alte functii --- */
int sum_of_digits(unsigned long long n);
int valid_pin(char pin[6]);
TLH AlocareHistory(char info[MAX]);
int add_history(TLH *x, char info[MAX]);
void delete_history(ALH aH);
int elimina_history(ALH aL, char aux[MAX]);
LSC AlocareCard(cardInfo x);
cardInfo citire_date_card(FILE * file);
void delete_LSC(ALSC aC);
int verifica_existenta_card(LC ListaCarduri, unsigned long long x, int poz);


/* --- functii specifice prelucrarii listelor --- */
int add_card(LC ListaCarduri, int poz, cardInfo info);
int delete_card(LC ListaCarduri, int poz, unsigned long long card_number);
void cancel(LC ListaCarduri, unsigned long long card_number, int poz);
void inserare_card(LC ListaCarduri, unsigned long long card_number, char pin[6], int poz, FILE * out);
void unblock_card(LC ListaCarduri, unsigned long long card_number, int poz);
void pin_change(LC ListaCarduri, unsigned long long card_number, char pin[6], int poz, FILE * out);
void balance_inquiry(LC ListaCarduri, unsigned long long card_number, int poz, FILE * out);
void recharge(LC ListaCarduri, unsigned long long card_number, int sum, int poz, FILE * out);
void cash_withdrawal(LC ListaCarduri, unsigned long long card_number, int sum, int poz, FILE * out);
void transfer_funds(LC ListaCarduri, unsigned long long card_number_s,
	unsigned long long card_number_d, int sum, int poz1, int poz2, FILE * out);
void reverse_transaction(LC ListaCarduri, unsigned long long card_number_s,
	unsigned long long card_number_d, int sum, int poz1, int poz2, FILE * out);

LC AlocareLista(int nr_max_carduri);
void DistrugeL(ALC aL, int nr_max_carduri);
void afisare_lista_show(LC ListaCarduri, FILE * out, int poz_max);
void afisare_lista_card(LC ListaCarduri, FILE * out, int poz, unsigned long long card_number);
void copiere(LC destinatie, LC sursa, int nr_max_carduri, int * poz_max);