/* TOADER Sergiu-Cristian - 315CB */

#include "atm.h"

/* --- Functie care adauga card in lista pe o
	pozitie data cu informatia data --- */
int add_card(LC ListaCarduri, int poz, cardInfo info)
{
	int i;
	LC aux = ListaCarduri;

	for(i=0;i<poz;i++)
		aux = aux->urm;

	LSC aux2 = AlocareCard(info);
	if(!aux2)
		return 0;

	aux2->urm = aux->lista;
	aux->lista = aux2;
	return 1;
}

/* --- Functie care elimina cardul din sublista
	data de parametrul poz --- */
int delete_card(LC ListaCarduri, int poz, unsigned long long card_number)
{
	int i;
	LC aux = ListaCarduri;
	LSC ant, p;

	for(i=0;i<poz;i++)
		aux = aux->urm;

  for(p = aux->lista, ant = NULL; p; ant = p, p = p->urm)
    if(p->info.card_number == card_number)
      break;
  
  if(!p)
  {
    fprintf(stderr, "Nu exista cardul in lista\n");
    return 0;
  }

  if(!ant)
    aux->lista = p->urm;
  else
    ant->urm = p->urm;

	delete_history(&(p->info.history));
	free(p);
  return 1;
}

/* --- functie pentru comanda insert_card --- */
void inserare_card(LC ListaCarduri, unsigned long long card_number,
	char pin[6], int poz, FILE * out)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	int i;
	char auxH[MAX];

	/* --- secventa de cod prin care se parcurg lista mare si sublista pentru a
		localiza cardul (secventa va aparea si la alte functii) --- */
	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}
	/* --- --- */

	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}
	if(!strcmp(aux3.status, "LOCKED"))
	{
		fprintf(out,"The card is blocked. Please contact the administrator.\n");
		/* --- variabila auxH este folosita pentru a crea istoricul cu
			datele necesare (la fel si la alte functii)--- */
		sprintf(auxH, "FAIL, insert_card %016llu %s", card_number, pin);
		add_history(&aux2->info.history, auxH);
		return;
	}
	if(strcmp(aux3.pin, pin))
	{
		sprintf(auxH, "FAIL, insert_card %016llu %s", card_number, pin);
		fprintf(out, "Invalid PIN\n");
		aux2->info.wrong_pin++;
		add_history(&aux2->info.history, auxH);

		if(aux2->info.wrong_pin == 3)
		{	
			fprintf(out,"The card is blocked. Please contact the administrator.\n");
			strcpy(aux2->info.status,"LOCKED");
		}
	}
	else
	{
		sprintf(auxH, "SUCCESS, insert_card %016llu %s", card_number, pin);
		/* --- se va afisa mesajul doar daca statusul este NEW--- */
		if(! strcmp(aux2->info.status, "NEW"))
			fprintf(out, "You must change your PIN.\n");
		aux2->info.wrong_pin = 0;
		add_history(&aux2->info.history, auxH);
	}
}

/* --- functie pentru comanda cancel --- */
void cancel(LC ListaCarduri, unsigned long long card_number, int poz)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	int i;
	char auxH[MAX];

	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}
	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}
	sprintf(auxH, "SUCCESS, cancel %016llu", card_number);
	add_history(&aux2->info.history, auxH);
}

/* --- functie pentru comanda unblock_card --- */
void unblock_card(LC ListaCarduri, unsigned long long card_number, int poz)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	int i;

	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}
	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}
	/* --- statusul devine "active" si numarul de introduceri
		gresite ale pin-ului se reseteaza--- */
	strcpy(aux2->info.status, "ACTIVE");
	aux2->info.wrong_pin = 0;
}

/* --- functie pentru comanda pin_change --- */
void pin_change(LC ListaCarduri, unsigned long long card_number,
	char pin[6], int poz, FILE * out)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	char auxH[MAX];
	int i;

	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}
	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}

	if(!valid_pin(pin))
	{
		sprintf(auxH, "FAIL, pin_change %016llu %s", card_number, pin);
		fprintf(out, "Invalid PIN\n");
		aux2->info.wrong_pin++;
		add_history(&aux2->info.history, auxH);
		return;
	}
	else
	{
		sprintf(auxH, "SUCCESS, pin_change %016llu %s", card_number, pin);
		strcpy(aux2->info.pin, pin);
		strcpy(aux2->info.status, "ACTIVE");
		add_history(&aux2->info.history, auxH);
	}
}

/* --- functie pentru comanda balance_inquiry --- */
void balance_inquiry(LC ListaCarduri, unsigned long long card_number,
	int poz, FILE * out)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	char auxH[MAX];
	int i;

	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}

	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}

	/* --- se afiseaza in output soldul curent
		si se adauga comanda in istoric --- */
	fprintf(out, "%d\n", aux2->info.balance);
	sprintf(auxH, "SUCCESS, balance_inquiry %016llu", card_number);
	add_history(&aux2->info.history, auxH);
}

/* --- functie pentru comanda recharge --- */
void recharge(LC ListaCarduri, unsigned long long card_number,
	int sum, int poz, FILE * out)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	char auxH[MAX];
	int i;

	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}

	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}

	/* --- se verifica daca suma adaugata este multiplu de 10 --- */
	if(sum%10)
	{
		fprintf(out, "The added amount must be multiple of 10\n");
		sprintf(auxH, "FAIL, recharge %016llu %d", card_number, sum);
		add_history(&aux2->info.history, auxH);
		return;
	}
	/* --- se aduna la sold suma dorita si se adauga comanda in istoric --- */
	else
	{
		aux2->info.balance += sum;
		fprintf(out, "%d\n", aux2->info.balance);
		sprintf(auxH, "SUCCESS, recharge %016llu %d", card_number, sum);
		add_history(&aux2->info.history, auxH);
	}
}

/* --- functie pentru comanda cash_withdrawal --- */
void cash_withdrawal(LC ListaCarduri, unsigned long long card_number,
	int sum, int poz, FILE * out)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	char auxH[MAX];
	int i;

	for(i=0;i<poz;i++)
		aux = aux->urm;

	aux2 = aux->lista;

	while(aux2)
	{
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}

	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}

	if(sum%10)
	{
		fprintf(out, "The requested amount must be multiple of 10\n");
		sprintf(auxH, "FAIL, cash_withdrawal %016llu %d", card_number, sum);
		add_history(&aux2->info.history, auxH);
		return;
	}

	/* --- se verifica daca exista fonduri suficiente --- */
	else if( (aux2->info.balance - sum) < 0)
	{
		fprintf(out, "Insufficient funds\n");
		sprintf(auxH, "FAIL, cash_withdrawal %016llu %d", card_number, sum);
		add_history(&aux2->info.history, auxH);
		return;
	}
	/* --- se scade din sold suma dorita si se adauga operatia in istoric --- */
	else
	{
		aux2->info.balance -= sum;
		fprintf(out, "%d\n", aux2->info.balance);
		sprintf(auxH, "SUCCESS, cash_withdrawal %016llu %d", card_number, sum);
		add_history(&aux2->info.history, auxH);
	}
}

/* --- functie pentru comanda transfer_funds --- */
void transfer_funds(LC ListaCarduri, unsigned long long card_number_s,
	unsigned long long card_number_d, int sum, int poz1, int poz2, FILE * out)
{
	LC aux_s = ListaCarduri;
	LSC aux2_s;
	cardInfo aux3_s;
	char auxH[MAX];
	int i;

	/* --- se determina pozitia pentru ambele carduri --- */
	for(i=0;i<poz1;i++)
		aux_s = aux_s->urm;
	aux2_s = aux_s->lista;
	while(aux2_s)
	{
		aux3_s = aux2_s->info;
		if(aux3_s.card_number == card_number_s)
			break;
		aux2_s = aux2_s->urm;
	}
	if(!aux2_s)
	{
		fprintf(stderr, "Nu exista cardul sursa\n");
		return; 
	}

	LC aux_d = ListaCarduri;
	LSC aux2_d;
	cardInfo aux3_d;

	for(i=0;i<poz2;i++)
		aux_d = aux_d->urm;
	aux2_d = aux_d->lista;
	while(aux2_d)
	{
		aux3_d = aux2_d->info;
		if(aux3_d.card_number == card_number_d)
			break;
		aux2_d = aux2_d->urm;
	}
	if(!aux2_d)
	{
		fprintf(stderr, "Nu exista cardul destinatie\n");
		return; 
	}

	if(sum%10)
	{
		fprintf(out, "The transferred amount must be multiple of 10\n");
		sprintf(auxH, "FAIL, transfer_funds %016llu %016llu %d",
			card_number_s, card_number_d, sum);
		add_history(&aux2_s->info.history, auxH);
		return;
	}

	else if( (aux2_s->info.balance - sum) < 0)
	{
		fprintf(out, "Insufficient funds\n");
		sprintf(auxH, "FAIL, transfer_funds %016llu %016llu %d",
			card_number_s, card_number_d, sum);
		add_history(&aux2_s->info.history, auxH);
		return;
	}

	/* --- se ia suma de la cardul sursa, se adauga la suma cardului destinatie
		si se adauga comanda in istoricul ambelor carduri --- */
	else
	{
		aux2_s->info.balance -= sum;
		fprintf(out, "%d\n", aux2_s->info.balance);
		aux2_d->info.balance += sum;
		sprintf(auxH, "SUCCESS, transfer_funds %016llu %016llu %d",
			card_number_s, card_number_d, sum);
		add_history(&aux2_s->info.history, auxH);
		add_history(&aux2_d->info.history, auxH);
	}
}

/* --- functie pentru comanda reverse_transaction --- */
void reverse_transaction(LC ListaCarduri, unsigned long long card_number_s,
	unsigned long long card_number_d, int sum, int poz1, int poz2, FILE * out)
{
	LC aux_s = ListaCarduri;
	LSC aux2_s;
	cardInfo aux3_s;
	char auxH[MAX];
	int i;

	/* --- se determina pozitia pentru ambele carduri --- */
	for(i=0;i<poz1;i++)
		aux_s = aux_s->urm;
	aux2_s = aux_s->lista;
	while(aux2_s)
	{
		aux3_s = aux2_s->info;
		if(aux3_s.card_number == card_number_s)
			break;
		aux2_s = aux2_s->urm;
	}
	if(!aux2_s)
	{
		fprintf(stderr, "Nu exista cardul sursa\n");
		return; 
	}

	LC aux_d = ListaCarduri;
	LSC aux2_d;
	cardInfo aux3_d;

	for(i=0;i<poz2;i++)
		aux_d = aux_d->urm;
	aux2_d = aux_d->lista;
	while(aux2_d)
	{
		aux3_d = aux2_d->info;
		if(aux3_d.card_number == card_number_d)
			break;
		aux2_d = aux2_d->urm;
	}
	if(!aux2_d)
	{
		fprintf(stderr, "Nu exista cardul destinatie\n");
		return; 
	}

	if(sum%10)
	{
		fprintf(out, "The transferred amount must be multiple of 10\n");
		sprintf(auxH, "FAIL, reverse_transaction %llu %llu %d",
			card_number_s, card_number_d, sum);
		add_history(&aux2_s->info.history, auxH);
		return;
	}

	else if( (aux2_d->info.balance - sum) < 0)
	{
		fprintf(out, "The transaction cannot be reversed\n");
		return;
	}

	else
	{
		aux2_s->info.balance += sum;
		aux2_d->info.balance -= sum;
		sprintf(auxH, "SUCCESS, reverse_transaction %016llu %016llu %d",
			card_number_s, card_number_d, sum);
		add_history(&aux2_s->info.history, auxH);
		sprintf(auxH, "SUCCESS, transfer_funds %016llu %016llu %d",
			card_number_s, card_number_d, sum);
		/* --- se sterge din istoricul cardului
			destinatie comanda transfer_funds --- */
		elimina_history(&aux2_d->info.history, auxH);
	}
}

/* --- Functie de alocare lista --- */
LC AlocareLista(int nr_max_carduri)
{
	LC aux, u, ListaCarduri = NULL;
	do
	{
		aux = (LC)calloc(nr_max_carduri, sizeof(cardInfo));

		if(!aux){ fprintf(stderr, "alocare aux esuata\n"); break; }

		if(!ListaCarduri) ListaCarduri = aux;
		else u->urm = aux;

		u = aux;

		nr_max_carduri --;
	} while(nr_max_carduri>=0);

	return ListaCarduri;
}

/* --- functie care distruge lista --- */
void DistrugeL(ALC aL, int nr_max_carduri)
{
	LC aux;
	LSC aux2;
	while(nr_max_carduri>=0)
	{
		nr_max_carduri--;
		aux = *aL;

		if(nr_max_carduri>=0)
			aux2 = aux->lista;

		delete_LSC(&aux2);
		
		if(nr_max_carduri>=0)
			*aL = aux->urm;

		free(aux);


	}
}

/* --- functie pentru functia show simpla --- */
void afisare_lista_show(LC ListaCarduri, FILE * out, int poz_max)
{
	int pos = 0;
	LC aux = ListaCarduri;
	LSC aux2 = aux->lista;
	cardInfo aux3;
	TLH aux4;
	/* --- parcurgerea listei de tip LC --- */
	for(;pos<=poz_max;pos++)
	{
		aux2 = aux->lista;
		if(!aux2)
			fprintf(out, "pos%d: []\n",pos);
		else
		{
			fprintf(out, "pos%d: [",pos);
			/* --- parcurgerea listelor de tip LSC --- */
			while(aux2)
			{
				aux3 = aux2->info;
				/* --- afisarea --- */
				fprintf(out, "\n(card number: %016llu, PIN: %s, expiry date: %s, CVV:",
					aux3.card_number, aux3.pin, aux3. expiry_date);
				fprintf(out, " %03d, balance: %d, status: %s, history: [",
					 aux3.cvv, aux3.balance, aux3.status);
				aux4 = aux3.history;

				/* --- parcurgerea si afisarea istoricului --- */
				while(aux4 && aux4->urm)
				{
					fprintf(out, "(%s), ", aux4->info);
					aux4 = aux4->urm;
				}
				if(aux4)
					fprintf(out, "(%s)", aux4->info);
				fprintf(out, "])");

				aux2 = aux2->urm;
			}
			fprintf(out, "\n]\n");
		}
		aux = aux->urm;
	}
}

/* --- functie pentru functia show <card_number> --- */
void afisare_lista_card(LC ListaCarduri, FILE * out, int poz,
 unsigned long long card_number)
{
	LC aux = ListaCarduri;
	LSC aux2;
	cardInfo aux3;
	TLH aux4;
	int i;

	for(i=0;i<poz;i++)
  	aux = aux->urm;
	aux2 = aux->lista;
	while(aux2)
	{	
		aux3 = aux2->info;
		if(aux3.card_number == card_number)
			break;
		aux2 = aux2->urm;
	}
	if(!aux2)
	{
		fprintf(stderr, "Nu exista cardul\n");
		return; 
	}

	/* --- afisarea --- */
	fprintf(out, "(card number: %016llu, PIN: %s, expiry date: %s, CVV:",
		aux3.card_number, aux3.pin, aux3. expiry_date);
	fprintf(out, " %03d, balance: %d, status: %s, history: [",
		aux3.cvv, aux3.balance, aux3.status);
	aux4 = aux3.history;

	/* --- parcurgerea si afisarea istoricului --- */
	while(aux4 && aux4->urm)
	{
		fprintf(out, "(%s), ", aux4->info);
		aux4 = aux4->urm;
	}
	if(aux4)
		fprintf(out, "(%s)", aux4->info);
	fprintf(out, "])\n");
}

/* --- functie care copiaza o lista de tip LC in alta lista ---*/
void copiere(LC destinatie, LC sursa, int nr_max_carduri, int * poz_max)
{
	LSC aux2;
	cardInfo aux3;
	int nr = 0, poz;
	/* --- parcurgerea listei sursa --- */
	while(sursa)
	{
		aux2 = sursa->lista;
		while(aux2)
		{
			aux3 = aux2->info;
			poz = sum_of_digits(aux3.card_number)%nr_max_carduri;
			/* --- recalularea pozitiei maxime si transmiterea
				sa prin efect lateral --- */
			if(*poz_max < poz)
				*poz_max = poz;

			/* --- adaugarea cardului in lista copie --- */
			add_card(destinatie, poz, aux3);
			aux2 = aux2->urm;
			nr++;
		}
		sursa = sursa->urm;
	}
}