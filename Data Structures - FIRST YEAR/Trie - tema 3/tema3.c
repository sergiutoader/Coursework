// TOADER Sergiu-Cristian, 315CB
#include "functii.h"

int main()
{
	char operatie[MAX], nume_director[MAX], path[MAX] = "/",
		copii[MAX], src[MAX], dest[MAX];

	/* -- Initializare fisiere input/output/error -- */	
	FILE * input = fopen("input.in", "rt");
	if(!input) { fprintf(stderr, "fisier inexistent\n"); return 0; }

	FILE * output = fopen("output.out", "wt");
	if(!output) { fprintf(stderr, "fisier inexistent\n"); return 0; }

	FILE * err = fopen("error.out", "wt");
	if(!err) { fprintf(stderr, "fisier inexistent\n"); return 0; }
	
	int numarOperatii;
	fscanf(input, "%d\n", &numarOperatii);

	/* -- Initializare Trie pentru sistemul de fisiere + director curent -- */
	Trie * file_system = newNode();
	Trie * current_dir = file_system;
	if(!file_system) { fprintf(stderr, "can't alloc memory\n"); return 0; }

	/* -- Se parcurge fisierul linie cu linie, interpretandu-se fiecare operatie
	in parte si apelandu-se functiile corespunzatoare operatiilor citite -- */
	while(numarOperatii)
	{
		fscanf(input, "%s", operatie);

		if(!strcmp(operatie, "mkdir"))
			mkdir(input, err, current_dir, file_system);

		if(!strcmp(operatie, "cd"))
		{
			fscanf(input, "%s", nume_director);
			cd(nume_director, path, err, file_system, &current_dir);
		}

		if(!strcmp(operatie, "pwd"))
			pwd(path, output);

		if(!strcmp(operatie, "ls"))
		{
			ls(current_dir, output, copii, 0);
			fprintf(output, "\n");
		}

		if(!strcmp(operatie, "mv"))
		{
			fscanf(input, "%s %s", src, dest);
			fprintf(err, "%s: No such file or directory\n", src);
		}

		numarOperatii--;
	}
	
	/* -- eliberare memorie folosita si inchidere fisiere -- */
	destroyTrie(file_system);
	fclose(input);
	fclose(output);
	fclose(err);
	return 0;
}