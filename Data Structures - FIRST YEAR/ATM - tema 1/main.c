/* TOADER Sergiu-Cristian - 315CB */

#include "atm.h"

int main()
{
	char str[MAX], pin[6];
	int nr_max_carduri, nr_carduri = 0, poz_max = 0,
		nr_total_carduri = 0, poz, ok, sum, poz1, poz2;
	unsigned long long card_number, card_number_s, card_number_d;
	cardInfo info;
	LC aux;

	/* --- deschiderea fisierelor de input si output --- */
	FILE * in, * out;
	in = fopen( "input.in" , "rt");
	out = fopen( "output.out", "wt");

	if(!in)
	{
		fprintf(stderr,"fisier inexistent\n");
		return 0;
	}

	/* --- citire nr_max_carduri --- */
	fscanf(in,"%d",&nr_max_carduri);
	
	/* --- alocare memorie pentru nr_max_carduri celule --- */
	LC ListaCarduri = AlocareLista(nr_max_carduri);

	if(!ListaCarduri)
	{
		DistrugeL(&ListaCarduri, nr_max_carduri);
		fprintf(stderr, "Alocare lista esuata\n");
		return 0;
	}
 
	while(fscanf(in, "%s", str)!=EOF)
	{
		/* --- citirea din fisier a operatiilor care trebuie executate
			si apoi, in functie de operatia citita, parametrii*/

		if(! strcmp(str, "add_card"))
		{
			/* --- citirea parametrilor operatiei add_card --- */
			info = citire_date_card(in);

			/* --- calcularea pozitiei pe care trebuie
				introdus cardul + pozitia maxima --- */
			poz = sum_of_digits(info.card_number)%nr_max_carduri;
			if(poz > poz_max)
				poz_max = poz;

			/* --- verific existenta cardului in lista --- */
			if(!nr_total_carduri)
				ok = 0;
			else
				ok = verifica_existenta_card(ListaCarduri, info.card_number, poz);

			if(!ok)
			{
				nr_carduri++;
				nr_total_carduri++;

				/* --- realocarea listei in cazul depasirii nr_max_carduri --- */
				if(nr_carduri>nr_max_carduri)
				{
					nr_max_carduri*=2;

					poz = sum_of_digits(info.card_number)%nr_max_carduri;
					if(poz > poz_max)
						poz_max = poz;

					aux = ListaCarduri;
					ListaCarduri = AlocareLista(nr_max_carduri);
					copiere(ListaCarduri, aux, nr_max_carduri, &poz_max);

				}
				add_card(ListaCarduri, poz, info);
			}

			else
				fprintf(out, "The card already exists\n");   
		}

		if(! strcmp(str, "delete_card"))
		{
			fscanf(in, "%016llu", &card_number);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			delete_card(ListaCarduri, poz, card_number);
			nr_carduri--;
		}

		if(! strcmp(str, "show"))
		{
			/* --- daca nu a fost introdus niciun card in lista,
				show nu va face nimic --- */
			if(!nr_total_carduri)
				continue;
			/* --- daca urmatorul element citit este de tip card_number,
				se va apela functia speciala show <card_number>, in caz
				contrar, se va apela show simplu --- */
			if(fscanf(in, "%016llu", &card_number)!=1)	
				afisare_lista_show(ListaCarduri, out, poz_max);
			else
			{
				poz = sum_of_digits(card_number)%nr_max_carduri;
				afisare_lista_card(ListaCarduri, out, poz, card_number);
			}
		}

		if(! strcmp(str, "insert_card"))
		{
			fscanf(in, "%016llu", &card_number);
			fscanf(in, "%s", pin);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			inserare_card(ListaCarduri, card_number, pin, poz, out);
		}

		if(! strcmp(str, "cancel"))
		{
			fscanf(in, "%016llu", &card_number);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			cancel(ListaCarduri, card_number, poz);
		}

		if(! strcmp(str, "unblock_card"))
		{
			fscanf(in, "%016llu", &card_number);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			unblock_card(ListaCarduri, card_number, poz);
		}

		if(! strcmp(str, "pin_change"))
		{
			fscanf(in, "%016llu", &card_number);
			fscanf(in, "%s", pin);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			pin_change(ListaCarduri, card_number, pin, poz, out);
		}

		if(! strcmp(str, "balance_inquiry"))
		{
			fscanf(in, "%016llu", &card_number);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			balance_inquiry(ListaCarduri, card_number, poz, out);
		}

		if(! strcmp(str, "recharge"))
		{
			fscanf(in, "%016llu", &card_number);
			fscanf(in, "%d", &sum);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			recharge(ListaCarduri, card_number, sum, poz, out);
		}

		if(! strcmp(str, "cash_withdrawal"))
		{
			fscanf(in, "%016llu", &card_number);
			fscanf(in, "%d", &sum);
			poz = sum_of_digits(card_number)%nr_max_carduri;
			cash_withdrawal(ListaCarduri, card_number, sum, poz, out);
		}

		if(! strcmp(str, "transfer_funds"))
		{
			fscanf(in, "%016llu", &card_number_s);
			fscanf(in, "%016llu", &card_number_d);
			fscanf(in, "%d", &sum);
			poz1 = sum_of_digits(card_number_s)%nr_max_carduri;
			poz2 = sum_of_digits(card_number_d)%nr_max_carduri;
			transfer_funds(ListaCarduri, card_number_s, card_number_d, sum, poz1, poz2, out);
		}

		if(! strcmp(str, "reverse_transaction"))
		{
			fscanf(in, "%016llu", &card_number_s);
			fscanf(in, "%016llu", &card_number_d);
			fscanf(in, "%d", &sum);
			poz1 = sum_of_digits(card_number_s)%nr_max_carduri;
			poz2 = sum_of_digits(card_number_d)%nr_max_carduri;
			reverse_transaction(ListaCarduri, card_number_s, card_number_d, sum, poz1, poz2, out);
		}

	}

	/* --- eliberez memoria si inchid fisierele --- */
	fclose(in);
	fclose(out);
	DistrugeL(&ListaCarduri, nr_max_carduri);
	return 0;
}