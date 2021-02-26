/*
 * APD - Tema 1
 * Octombrie 2020
 * 
 * Toader Sergiu-Cristian
 * 335CB
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pthread_barrier_mac.h"

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;

/*
	 Parametrii rularii, latimea, inaltimea, tabloul result si numarul de
	 fire de executie sunt declarate global pentru a fi vizibile in interiorul
	 functiei executate de thread-uri.
*/
params par;
int width, height;
int **result;
int P;

// Bariera utilizata pentru sincronizare
pthread_barrier_t barrier;


// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 5) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	P = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}

// functie de minim
int min (int a, int b) {
	if (a < b) {
		return a;
	} 
	return b;
} 

// Functia apelata de thread
void *thread_function(void *arg) {

	// id thread
	int id = *(int *)arg;

	int w, h, i;

	/*
		Se calculeaza pe baza id-ului indici de start si de end pentru calcul
		paralel. Se imparte inaltimea intervalului pe care se va calcula functia
		multimii Julia in parti egale astfel incat fiecare thread sa execute 
		una din parti.
	*/
	int start = id * (double)height / P;
 	int end = min((id + 1) * (double)height / P, height);

	for (w = 0; w < width; w++) {
		for (h = start; h < end; h++) {
			int step = 0;
			complex z = { .a = w * par.resolution + par.x_min,
							.b = h * par.resolution + par.y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + par.c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + par.c_julia.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	/*
	 	Se introduce o bariera pentru a garanta ca toate thread-urile au
	 	incheiat calculul valorilor inainte de modificarea pozitiei valorilor
	 	din multimea result
	 */
	pthread_barrier_wait(&barrier);

	/* 
		Trecerea de la coordonate matematice la coordonate ecran. Se realizeaza
		de asemenea paralelizat. Fiecarui thread i se atribuie o sectiune din
		jumatatea de inceput a matricei result si realizeaza interschimbarea
		cu valorile "diametral opuse" din matrice.
	*/
	for (i = start / 2; i < end / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}

	/* 	
		Bariera pentru a asigura ca result este complet asignat inainte sa se
	 	construiasca imaginea pe baza valorilor.
	 */
	pthread_barrier_wait(&barrier);

	/*
		Scrierea in fisier, eliberarea memoriei matricei result, citirea
		datelor din fisierul de intrare pentru multimea Mandelbrot, calculul
		latimii si inaltimii pentru noul interval si realocarea matricei result
		sunt realizate doar de thread-ul cu id-ul 0 (deoarece nu pot fi
		paralelizate). Celelalte thread-uri vor trece peste acesti pasi si vor
		astepta pana cand thread-ul 0 termina. 
	*/
	if(id == 0) {
		write_output_file(out_filename_julia, result, width, height);
		free_memory(result, height);

		read_input_file(in_filename_mandelbrot, &par);

		width = (par.x_max - par.x_min) / par.resolution;
		height = (par.y_max - par.y_min) / par.resolution;

		result = allocate_memory(width, height);
	}

	/*
		Bariera introdusa pentru a opri executia celorlalte thread-uri pana cand
		thread-ul 0 termina de realizat sarcinile mentionate anterior.
	*/
	pthread_barrier_wait(&barrier);


	/*
		Similar algoritmului pentru multimea Julia, intervalul va fi impartit
		in mod egal pe inaltime, fiecare thread executand algoritmul pe portiunea
		corespunzatoare id-ului sau
	*/
	start = id * (double)height / P;
 	end = min((id + 1) * (double)height / P, height);

 	// Algoritm Mandelbrot paralelizat
 	for (w = 0; w < width; w++) {
		for (h = start; h < end; h++) {
			complex c = { .a = w * par.resolution + par.x_min,
							.b = h * par.resolution + par.y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < par.iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			result[h][w] = step % 256;
		}
	}

	/*
		Bariera pentru a asigura ca trecerea in coordonate ale ecranului se
		realizeaza dupa ce matricea result are toate valorile calculate.
	*/
	pthread_barrier_wait(&barrier);

	/*
		Trecere in coordonate ale ecranului - paralelizata identic ca 
		la multimea Julia.
	*/
	for (i = start / 2; i < end / 2; i++) {
		int *aux = result[i];
		result[i] = result[height - i - 1];
		result[height - i - 1] = aux;
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	// se citesc argumentele programului
	get_args(argc, argv);

	int i;
	// vectori thread-uri + indici
	int thread_id[P];
	pthread_t tid[P];

	// Julia:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	read_input_file(in_filename_julia, &par);
	width = (par.x_max - par.x_min) / par.resolution;
	height = (par.y_max - par.y_min) / par.resolution;
	result = allocate_memory(width, height);
	
	// initializare bariera
	int r;
	r = pthread_barrier_init(&barrier, NULL, P);
	if(r) {
		printf("Eroare la crearea barierei\n");
		exit(-1);
	}

	/*  
		- se creeaza thread-urile, pentru apelul functiei thread_function
		- se trimite ca parametru un id unic pentru fiecare thread
	*/
	for (i = 0; i < P; i++) {
		thread_id[i] = i;
		pthread_create(&tid[i], NULL, thread_function, &thread_id[i]);

		if (r) {
  	  		printf("Eroare la crearea thread-ului %d\n", i);
  	  		exit(-1);
  		}
	}

	// se asteapta thread-urile sa isi incheie executia
	for (i = 0; i < P; i++) {
		pthread_join(tid[i], NULL);

		if (r) {
  	  		printf("Eroare la asteptarea thread-ului %d\n", i);
  	  		exit(-1);
  		}
	}

	// distrugere bariera
	r = pthread_barrier_destroy(&barrier);
	if(r) {
		printf("Eroare la distrugerea barierei\n");
		exit(-1);
	}

	
	// scriere in fisier rezultat pentru multimea mandelbrot + eliberare result
	write_output_file(out_filename_mandelbrot, result, width, height);
	free_memory(result, height);

	return 0;
}
