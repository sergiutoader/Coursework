// TOADER Sergiu-Cristian, 315CB
#include "functii.h"

/* -- functie care aloca un nod nou -- */
Trie* newNode()
{
	int i;
	Trie *node = (Trie *)malloc(sizeof(Trie));
	if(!node) return NULL;

	/* -- setare nod frunza cu 0 -- */
	node->isLeaf = 0;

	/* -- setare vector de caractere pe NULL -- */
	for (i = 0; i < CHAR_SIZE; i++)
		node->character[i] = NULL;

	return node;
}

/* -- functie care primeste ca parametru un sir de caractere si returneza
	indicele corespunzator primului caracter din sir -- */
int getIndex(char *str)
{
	int index;
	if(*str >= 'a' && *str < 'z')
		index = *str - 'a';
	else if(*str >= '0' && *str <= '9')
		index = 26 + *str - '0';
	else if(*str == '.')
		index = 36;
	else if(*str == '/')
		index = 37;
	else index = 38;
	return index;
}

/* -- functie care primeste un intreg si returneaza un caracter corespunzator
	indicelui repezentat de acel intreg -- */
char getChar(int index)
{
	char character;
	if(index >= 0 && index < 26)
		character = 'a' + index;
	else if(index >= 26 && index < 36)
		character = '0' + index - 26;
	else if(index == 36)
		character = '.';
	else if(index == 37)
		character = '/';
	else character = '?';
	return character;
}

/* -- functie care insereaza un sir de caractere in trie -- */
void insert(Trie **head, char *str)
{
	int index;
	Trie *curr = *head;

	/* -- se parcurg toate caracterele din sir -- */
	while (*str)
	{
		/* -- se obtine indicele primului caracter din sir -- */
		index = getIndex(str);

		/* -- se creaza un nod pe pozita corespunzatoare in arbore daca
			nu exista deja-- */
		if (curr->character[index] == NULL)
		{
			curr->character[index] = newNode();
			if(!curr->character[index])
			{ fprintf(stderr, "can't alloc memory\n"); return; }
		}

		/* -- se marcheaza ca frunza nodul care va avea caracterul
			'/' -- */
		if(str[1] == '/')
			curr->character[index]->isLeaf = 1;
		
		/* -- se trece la nivelul urmator in arbore -- */
		curr = curr->character[index];

		/* -- se trece la caracterul urmator un sir -- */
		str++;
	}

	/* -- se marcheaza nodul terminal in arbore -- */
	curr->isLeaf = 1;
}

/* -- functie care cauta un sir in arbore (returneaza 1 pentru
	sir gasit in arbore si 0 in caz contrar) -- */
int search(Trie* head, char *str)
{
	if (head == NULL)
		return 0;

	Trie* curr = head;

	/* -- se parcurg toate caracterele din sir -- */
	while (*str)
	{
		/* -- se trece la nivelul urmator in arbore -- */
		curr = curr->character[getIndex(str)];

		/* -- daca nodul curent este NUL, atunci sirul nu se
			gaseste in arbore -- */
		if (curr == NULL)
			return 0;

		/* -- se trece la caracterul urmator un sir -- */
		str++;
	}

	/* -- returneaza 1 daca ultimul nod la care s-a ajuns este terminal -- */
	return curr->isLeaf;
}

/* -- Functie pentru operatia mkdir -- */
void mkdir(FILE * input, FILE * err, Trie *current_dir, Trie *file_system)
{
	char * parametruOperatie;
	char *buffer = (char *)malloc(MAX);
	if(!buffer) { fprintf(stderr, "can't alloc memory\n"); return; }

	/* -- citire parametrii mkdir -- */
	fgets(buffer, MAX, input);
	buffer[strlen(buffer)-1] = '\0';

	/* -- delimitare sirul de caractere al directoarelor
		de adaugat folosind functia strtok -- */
	parametruOperatie = strtok(buffer, " ");

	while(parametruOperatie != NULL)
	{	

		char *parent_dir = (char *)calloc(1, MAX);
		if(!parent_dir)
		{ fprintf(stderr, "can't alloc memory\n"); return; }

		int i, ok = 0;
		
		/* -- cazul in care se da calea absoluta ca parametru -- */
		if(parametruOperatie[0] == '/')
		{
			/* -- eliminare caracterul / din cale -- */
			memmove(parametruOperatie, parametruOperatie + 1,
				strlen(parametruOperatie));
			/* -- verificare daca directorul exista deja -- */
			if(search(file_system, parametruOperatie))
				fprintf(err, "%s: already exists\n", parametruOperatie);

			else
			{
				/* --- determinare folder parinte --- */
				for(i = strlen(parametruOperatie) - 1; i>0; i--)
					if(parametruOperatie[i]=='/')
						break;
				if(i)
				{
					memcpy(parent_dir, parametruOperatie, i);
					
					/* --- in cazul in care folderul parinte nu exista,
						se afiseaza mesaj de eroare --- */
					if(!search(file_system, parent_dir))
					{
						ok = 1;
						fprintf(err, "/%s: No such file or directory\n",
							parametruOperatie);
					}
				
				}
				/* -- se insereaza folderul in sistemul de fisiere daca fisierul
				nu are director parinte (trebuie inserat in root) sau s-a gasit
				directorul parinte in sistemul de fisiere -- */
				if((!i) || (i && !ok) )
					insert(&file_system, parametruOperatie);
			}
		}
		/* -- cazul in care se da calea relativa ca parametru -- */
		else
		{
			/* -- se verifica daca exista deja directorul -- */
			if(search(current_dir, parametruOperatie))
				fprintf(err, "%s: already exists\n", parametruOperatie);

			/* --- determinare folder parinte --- */
			for(i = strlen(parametruOperatie) - 1; i>0; i--)
				if(parametruOperatie[i]=='/')
					break;
			if(i)
			{
				memcpy(parent_dir, parametruOperatie, i);
				
				/* --- in cazul in care folderul parinte nu exista,
					se afiseaza mesaj de eroare --- */
				if(!search(current_dir, parent_dir))
				{
					fprintf(err, "%s: No such file or directory\n",
						parametruOperatie);
					ok = 1;
				}
			}
			/* -- similar ca la calea absoluta, se verifica conditiile de
				inserare in sistemul de fisiere -- */
			if((!i) || (i && !ok) )
				insert(&current_dir, parametruOperatie);
		}
		parametruOperatie = strtok(NULL, " ");
		free(parent_dir);	
	}
	free(buffer);
}

/* -- functie care modifica calea absoluta si realizeaza avansul in arbore -- */
Trie * changeDir (char path[MAX], Trie *head, char *str)
{
	Trie * curr = head;
	strcat(path, str);
	/* -- se parcurg caracterele sirului -- */
	while (*str)
	{
		/* -- se realizeaza parcurgerea in arbore -- */
		curr = curr->character[getIndex(str)];
		/* -- se trece la caracterul urmator -- */
		str++;
	}

	/* -- se returneaza nodul curent -- */
	return curr;
}

/* -- functie pentru operatia cd -- */
void cd(char nume_director[MAX], char path[MAX], FILE *err,
	Trie *file_system, Trie ** current_dir)
{
	/* -- cazul comenzilor de forma 'cd ..' -- */
	if(nume_director[0] == '.' && nume_director[1] == '.')
	{
		/* -- daca directorul curent este root se afiseaza mesaj de eroare -- */
		if(strlen(path) == 1)
			fprintf(err, "%s: No such file or directory\n", nume_director);
		else
		{
			int index = strlen(path) - 2;
			char new_path[MAX];
			/* -- determinare director parinte -- */
			while(path[index]!='/')
			{
				path[index] = '\0';
				index--;
			}
			strcpy(new_path, path + 1);
			strcpy(path, "/");
			/* -- actualizare director curent cu parintele lui -- */
			*current_dir = changeDir(path, file_system, new_path);
		}
				
	}
	/* -- cazul in care se da o cale relativa ca parametru -- */
	else if(nume_director[0]!='/')
	{	
		Trie * auxDir = (*current_dir)->character[CHAR_SIZE - 1];
		if(auxDir && search(auxDir, nume_director))
			*current_dir = auxDir;
		/* -- daca nu se gaseste in directorul curent
			calea, se afiseaza mesaj de eroare -- */
		if(!search(*current_dir, nume_director))
			fprintf(err, "%s: No such file or directory\n", nume_director);
		else
		{
			/* -- daca exista calea, se modifica directorul curent,
				actualizandu-se calea catre acesta -- */
			*current_dir = changeDir(path, *current_dir, nume_director);
			strcat(path, "/");
		}
	}
	/* -- cazul in care se da o cale absoluta ca parametru -- */
	else if(nume_director[0]=='/')
	{
		/* -- stergerea caracterului '/' din cale -- */
		memmove(nume_director, nume_director + 1, strlen(nume_director));
		/* -- introducere director in sistemul de fisiere (daca exista) -- */
		if(search(file_system, nume_director))
		{
			path[0]='/';
			path[1]='\0';
			*current_dir = changeDir(path, file_system, nume_director);
			strcat(path, "/");		
		}
		else
			fprintf(err, "/%s: No such file or directory\n", nume_director);
	}
}

/* -- functie pentru comanda pwd -- */
void pwd(char path[200], FILE * output)
{
	char printPath[MAX];
	strcpy(printPath, path);
	/* -- eliminare ultimul caracter('/') -- */
	if(strlen(path)>1)
		printPath[strlen(path) - 1] = '\0';
	/* -- afisare cale absoluta -- */
	fprintf(output, "%s\n", printPath);
}

/* -- functie recursiva pentru operatia ls -- */
void ls(Trie * head, FILE *output, char *copii, int level)
{
	int i;
	/* -- daca s-a gasit un nod frunza se afiseaza directorul -- */
	if(head->isLeaf)
	{	
		copii[level] = '\0';
		if(strlen(copii))
			fprintf(output, "%s ", copii);	
	}
	/* -- cautare noduri alocate -- */
	for(i = 0; i < CHAR_SIZE; i++)
	{
		if(head->character[i])
		{
			/* -- oprire in momentul in care s-a gasit caracterul '/' -- */
			if(i == CHAR_SIZE - 1 && strlen(copii))
				return;
			/* -- se pune in sir caracterul corespunzator
				pe nivelul curent -- */
			copii[level] = getChar(i);
			/* -- daca primul caracter din sir este '/' se elimina si se
			apeleaza recursiv functia ls pe acelasi nivel -- */
			if(copii[0] == '/')
			{
				memmove(copii, copii+1, strlen(copii));
				ls(head->character[i], output, copii, level);
			}
			/* -- altfel, functia se apeleaza recursiv cu
				nivelul urmator in arbore -- */
			else
				ls(head->character[i], output, copii, level+1);
		}
	}
}

/* -- functie care elibereaza memoria recursiv pentru Trie -- */
void destroyTrie(Trie * head)
{
	int i;
	for (i = 0; i < CHAR_SIZE; i++)
	{
		/* -- se cauta toate nodurile alocate -- */
    	if(head->character[i])
   			destroyTrie(head->character[i]);
	}
	/* -- eliberare memorie nod -- */
	free(head);
}