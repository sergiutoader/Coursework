/* TOADER Sergiu-Cristian - 315CB */

#include "atm.h"

/* --- Functie care calculeaza suma cifrelor unui numar --- */
int sum_of_digits(unsigned long long n)
{
	int sum = 0;
	while(n)
	{
		sum+=n%10;
		n/=10;
	}
	return sum;
}

/* --- functie care verifica daca pinul este valid
	(daca are 4 caractere strict numerice) --- */
int valid_pin(char pin[6])
{
	unsigned int i = 0;
	if(strlen(pin)!=4)
		return 0;
	for(;i < strlen(pin);i++)
		if(pin[i] < '0' || pin[i] > '9')
			return 0;
	return 1;
}

/* --- Alocare celula istoric --- */
TLH AlocareHistory(char info[MAX])
{
	TLH aux = (TLH)malloc(sizeof(TCH));
	if(!aux) return NULL;
	strcpy(aux->info, info);
	aux->urm = NULL;
	return aux;
}

/* --- Functie care adauga o celula in istoric --- */
int add_history(TLH *x, char info[MAX])
{
	TLH aux = AlocareHistory(info);
	if(!aux)
		return 0;
	aux->urm = *x;
	*x = aux;
	return 1;
}

/* --- functie de stergere istoric --- */
void delete_history(ALH aH)
{
	TLH aux;
  while(*aH)
  {
    aux = *aH;
    *aH = aux->urm;
    free(aux);
 	}
}

/* --- Eliminare celula istoric (pentru reverse_transaction) --- */
int elimina_history(ALH aL, char aux[MAX])
{
	TLH ant, p;
  for(p = *aL,ant = NULL; p; ant = p, p = p->urm)
    if(!strcmp(p->info, aux))
      break;
  
  if(!p)
    return 0;

  if(!ant)
    *aL=p->urm;
  else
    ant->urm = p->urm;
  free(p);
  return 1;
}

/* --- Alocare camp LSC --- */
LSC AlocareCard(cardInfo x)
{
	LSC aux = (LSC)malloc(sizeof(TCard));
	if(!aux) return NULL;
	aux->info = x;
	aux->urm = NULL;
	return aux;
}

/* --- Functie care citeste datele de pe card --- */
cardInfo citire_date_card(FILE * file)
{
	cardInfo info;
	fscanf(file, "%016llu", &info.card_number);
	fscanf(file, "%s", info.pin);
	fscanf(file, "%s", info.expiry_date);
	fscanf(file, "%03d", &info.cvv);
	info.balance = 0;
	info.history = NULL;
	strcpy(info.initial_pin, info.pin);
	strcpy(info.status, "NEW");
	info.wrong_pin = 0;
	return info;
}

/* --- functie care sterge o lista LSC din lista LC --- */
void delete_LSC(ALSC aC)
{
	LSC aux;
  while(*aC)
  {
    aux = *aC;
    delete_history(&(aux->info.history));
    *aC = aux->urm;
    free(aux);
  }
}

/* ---functie care verifica daca exista cardul in lista --- */
int verifica_existenta_card(LC ListaCarduri, unsigned long long x, int poz)
{
  int i = 0;
  LC aux = ListaCarduri;
  LSC aux2 = NULL;

  /* --- parcurgere lista pana la pozitia poz --- */
  for(;i<poz;i++)
    aux = aux->urm;

	if(!aux)
	{
		fprintf(stderr, "aux = NULL\n");
		return -1;
	}

  if(!(aux->lista))
		return 0;

	 aux2 = aux->lista;

	/* --- cautare card cu acelasi card_number in lista --- */
  while(aux2)
  {
    if(aux2->info.card_number == x)
      return 1;
    aux2 = aux2->urm;
  }
  return 0;
}