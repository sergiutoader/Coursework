#include "mpi.h"
#include "stringProcessing.h"
#include <unistd.h>
#include <pthread.h>

#define MASTER 0
#define HORROR 1
#define COMEDY 2
#define FANTASY 3
#define SCIENCE_FICTION 4

#define P sysconf(_SC_NPROCESSORS_ONLN)
#define MASTER_THREAD_COUNT 4
#define FILE_NAME_MAX_LEN 100
#define MAX_LINE_LEN 5000
#define MAX_LINE_COUNT 2500

#define DONE_STR "done"

static char * inFileName;
static char * outFileName;

// structura pentru liniile primite de thread-urile care executa procesarea
typedef struct par_lines {
    char **lines;
    int first_line_index;
    int line_count;
    int par_type;
} paragraph_lines;


// construieste o structura paragraph_lines care va fi trimisa la thread-urile care executa procesarea
paragraph_lines buildParagraphLines(char ** par, int first_line_index, int line_count, int par_type) {
    paragraph_lines p;
    p.first_line_index = first_line_index;
    p.line_count = line_count;
    p.par_type = par_type;
    
    p.lines = (char **) calloc(line_count, sizeof(char *));
    if(!p.lines) { fprintf(stderr, "Eroare calloc\n"); exit(EXIT_FAILURE); }

    // alocare + atribuire linii
    for(int i = 0; i < line_count; i++) {
        p.lines[i] = (char *) calloc(MAX_LINE_LEN, sizeof(char));
        if(!p.lines[i]) { fprintf(stderr, "Eroare calloc\n"); exit(EXIT_FAILURE); }

        memcpy(p.lines[i], par[first_line_index + i], MAX_LINE_LEN);
    }

    return p;
}


// intoarce numarul de thread-uri care trebuie create de worker pentru procesarea in paralel a textului
int getThreadCount(int line_count) {
    int count = 1;

    if(line_count % 20) {
         count += line_count / 20;
    }   

    if(count > P - 1) {
        count = P - 1;
    }

    return count;
}

// functia apelata de thread-urile deschise de master
// realizeaza citirea paralelizata din fisiere si trimiterea paragrafelor la workeri
void * master_thread_func(void *arg) {
	int id = *((int *) arg);

	// valoare care retine daca 
	int par_type = 0; // 1 -> horror, 2 -> comedy, 3 -> fantasy, 4 -> sf

	FILE * in;

	in = fopen(inFileName, "r");
	if(!in) { fprintf(stderr, "Eroare deschidere fisier\n"); exit(EXIT_FAILURE); }

	char * line = NULL;
	size_t len = 0;
	ssize_t read;

    // se numara paragrafele folosind variabila paragraph_count
    int * paragraph_count = (int *) calloc(1, sizeof(int));
    if(!paragraph_count) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }

	char ** paragraph = (char **) calloc(MAX_LINE_COUNT, sizeof(char *));
	if(!paragraph) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }
    for(int i = 0; i < MAX_LINE_COUNT; i++) {
        paragraph[i] = (char *) calloc(MAX_LINE_LEN, sizeof(char *));
        if(!paragraph[i]) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }
    }

	// citire linie cu linie
    int line_count = 0;
	while ((read = getline(&line, &len, in)) != -1) {
		if(par_type != 0) {
			// s-a ajuns la sfarsitul paragrafului
			if(!strcmp(line, "\n")) {
                (*paragraph_count)++;

                // trimitere paragraf la workerul corespunzator
                if(par_type == id) {
                    MPI_Send(paragraph_count, 1, MPI_INT, par_type, 1, MPI_COMM_WORLD);
                    MPI_Send(&line_count, 1, MPI_INT, par_type, 1, MPI_COMM_WORLD);
                    for(int i = 0; i < line_count; i++) {
                        MPI_Send(paragraph[i], strlen(paragraph[i]), 
                            MPI_CHAR, par_type, 1, MPI_COMM_WORLD); 
                    }
                }

                // resetare paragrafe
                for(int i = 0; i < line_count; i++) {
                    memset(paragraph[i], '\0', MAX_LINE_LEN); 
                }

                par_type = 0;
                line_count = 0;
			} else {
                // atasare linie la paragraf
				memcpy(paragraph[line_count] + strlen(paragraph[line_count]),
                    line, strlen(line) + 1);
                line_count++;
			}
		}

		// se determina tipul de paragraf citit
        if(!strcmp(line, "horror\n")) {
        	par_type = HORROR;
        } else if(!strcmp(line, "comedy\n")) {
        	par_type = COMEDY;
        } else if(!strcmp(line, "fantasy\n")) {
        	par_type = FANTASY;
        } else if(!strcmp(line, "science-fiction\n")) {
        	par_type = SCIENCE_FICTION;
        }

    }

    if(par_type != 0) {
    // s-a ajuns la sfarsitul paragrafului
        (*paragraph_count)++;


        // trimitere paragraf la workerul corespunzator
        if(par_type == id) {
            MPI_Send(paragraph_count, 1, MPI_INT, par_type, 1, MPI_COMM_WORLD);
            MPI_Send(&line_count, 1, MPI_INT, par_type, 1, MPI_COMM_WORLD);
            for(int i = 0; i < line_count; i++) {
                MPI_Send(paragraph[i], strlen(paragraph[i]), 
                    MPI_CHAR, par_type, 1, MPI_COMM_WORLD);
            }
        }

        // resetare paragrafe
        for(int i = 0; i < line_count; i++) {
            memset(paragraph[i], '\0', MAX_LINE_LEN); 
        }
    }
    


    // am terminat citirea - trimitem "done" pentru a semnala workerilor ca nu se
    // vor mai trimite paragrafe
    (*paragraph_count)++;
    line_count = 1;
    MPI_Send(paragraph_count, 1, MPI_INT, id, 1, MPI_COMM_WORLD);
    MPI_Send(&line_count, 1, MPI_INT, id, 1, MPI_COMM_WORLD);
    MPI_Send(DONE_STR, strlen(DONE_STR), MPI_CHAR, id, 1, MPI_COMM_WORLD);
   

	fclose(in);
	free(line);

    for(int i = 0; i < MAX_LINE_COUNT; i++) {
        free(paragraph[i]);
    }
    free(paragraph);

    // functia intoarce numarul de paragrafe citite
    (*paragraph_count)--;
	return (void *) paragraph_count;
}

// functia executata de thread-urile pornite de workeri
// realizeaza procesarea liniilor
void * process_string_func(void * arg) {
    paragraph_lines *p = ((paragraph_lines *) arg);

    // se verifica tipul paragrafului
    // in functie de tip, se aplica un filtru
    int type = p->par_type;
    if(type == HORROR) {
        for(int i = 0; i < p->line_count; i++) {
            applyHorror(&p->lines[i]);
        }
    } else if(type == COMEDY) {
        for(int i = 0; i < p->line_count; i++) {
            applyComedy(&p->lines[i]);
        }
    } else if(type == FANTASY) {
        for(int i = 0; i < p->line_count; i++) {
            applyFantasy(&p->lines[i]);
        }
    } else if(type == SCIENCE_FICTION) {
        for(int i = 0; i < p->line_count; i++) {
            applyScienceFiction(&p->lines[i]);
        }
    }

    return (void *) p;
}

// printeaza in fisier tipul de filtru in functie de codificarea numerica
void printWorkerType(FILE * file, int rank) {
    if(rank == HORROR) {
        fprintf(file, "horror\n");
    } else if(rank == COMEDY) {
        fprintf(file, "comedy\n");
    } else if(rank == FANTASY) {
        fprintf(file, "fantasy\n");
    } else if(rank == SCIENCE_FICTION) {
        fprintf(file, "science-fiction\n");
    } 
}

int main(int argc, char *argv[])
{
    int  numtasks, rank, len;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    MPI_Status s;

    int provided;

	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if(provided < MPI_THREAD_MULTIPLE) {fprintf(stderr, 
        "MPI does not provide needed threading level\n"); exit(EXIT_FAILURE);}
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks); // Total number of processes.
    MPI_Comm_rank(MPI_COMM_WORLD,&rank); // The current process ID / Rank.
    MPI_Get_processor_name(hostname, &len);

    // cod executat de nodul MASTER
    if(rank == MASTER) {

        // parsare nume fisiere de intrare si iesire
        inFileName = (char *) malloc(FILE_NAME_MAX_LEN);
        if(!inFileName) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }
        outFileName = (char *) malloc(FILE_NAME_MAX_LEN);
        if(!outFileName) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }

        memcpy(inFileName, argv[1], strlen(argv[1]) + 1);
        memcpy(outFileName, argv[1], strlen(argv[1]) + 1);
        strcpy(outFileName + strlen(outFileName) - 3, "out\0");


    	pthread_t master_threads[MASTER_THREAD_COUNT];
    	int i, r;
    	void *status;
    	int arguments[MASTER_THREAD_COUNT];

        
        // initializare thread-uri master care vor executa citirea in paralel
    	for (i = 0; i < MASTER_THREAD_COUNT; i++) {
			arguments[i] = i + 1;
			r = pthread_create(&master_threads[i], NULL, master_thread_func, &arguments[i]);

			if (r) {
				printf("Eroare la crearea thread-ului %d\n", i);
				exit(-1);
			}
		}

        int total_par_count = 1;
        // join la thread-urile de citire si calcul numar de paragrafe
		for (i = 0; i < MASTER_THREAD_COUNT; i++) {
			r = pthread_join(master_threads[i], &status);
            if(i == 0) {
                total_par_count += *((int *) status);
            }
            free(status);
		}

       // alocare buffer pentru primirea paragrafelor
        char * ** recv_paragraphs = (char ***) calloc(total_par_count, sizeof(char **));
        if(!recv_paragraphs) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }

        for(int i = 0; i < total_par_count; i++) {

            recv_paragraphs[i] = (char **) calloc(MAX_LINE_COUNT, sizeof(char *));
            if(!recv_paragraphs[i]) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }

            for(int j = 0; j < MAX_LINE_COUNT; j++) {
                recv_paragraphs[i][j] = (char *) calloc(MAX_LINE_LEN, sizeof(char));
                if(!recv_paragraphs[i][j]) { fprintf(stderr, 
                    "Eroare malloc\n"); exit(EXIT_FAILURE); }
            }
        }

        // alocare vector care retine tipul paragrafului
        int *par_types = (int *) calloc(total_par_count, sizeof(int));
        if(!par_types) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }
        // alocare vector care retine numarul de linii din paragraf
        int *par_sizes = (int *) calloc(total_par_count, sizeof(int));
        if(!par_sizes) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }

        int finished_workers_count = 0;
        // vector care retine workerii care au terminat de procesat paragrafele
        int finished_workers[4];
        for(int j = 0; j < 4; j++) {
            finished_workers[j] = 0;
        }


        int t = 0;
        while(1) {
            int paragraph_idx;
            int line_count;

            // se asteapta date de la fiecare worker
            for(int i = 1; i <= 4; i++) {

                if(finished_workers[i - 1] == 0) {
                    // primire indice paragraf, numar de linii
                    MPI_Recv(&paragraph_idx, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &s);
                    MPI_Recv(&line_count, 1, MPI_INT, i, 1, MPI_COMM_WORLD, &s);


                    // primire linii paragrafe
                    for(int j = 0; j < line_count; j++) {
                         MPI_Recv(recv_paragraphs[paragraph_idx][j], MAX_LINE_LEN, 
                            MPI_CHAR, i, 1, MPI_COMM_WORLD, &s);
                    }

                    // verificare daca procesul a primit toate paragrafele
                    if(!strcmp(recv_paragraphs[paragraph_idx][0], DONE_STR)) {
                        // marcare in vectorul finished_workers faptul ca workerul a terminat de procesat
                        finished_workers_count++;
                        finished_workers[i - 1] = 1;
                        break;
                    }

                    // setare tip paragraf
                    par_types[paragraph_idx] = i;
                    par_sizes[paragraph_idx] = line_count;
                }
            }

            // se incheie bucla de primire paragrafe atunci cand toti workerii au terminat
            if(finished_workers_count == 4) {
                break;
            }
        }

        // Scriere in fisier
        FILE * out = fopen(outFileName, "w");
        if(!out) { fprintf(stderr, "Eroare deschidere fisier\n"); exit(EXIT_FAILURE); }
        
        // se parcurg toate paragrafele  citite, care au fost retinute in recv_paragraphs
        for(int i = 1; i < total_par_count; i++) {
            // se afiseaza tipul paragrafului folosind functia printWorkerType
            printWorkerType(out, par_types[i]);
            
            for(int j = 0; j < par_sizes[i]; j++) {

                if(i == total_par_count - 1 && j == par_sizes[i] - 1) {
                    // se adauga un spatiu la final pentru paragrafele care nu au (valabil
                    // pentru ultimul paragraf)
                    if(recv_paragraphs[i][j][strlen(recv_paragraphs[i][j]) - 1] != ' ') {
                        fprintf(out, "%s \n", recv_paragraphs[i][j]);
                    } else {
                        fprintf(out, "%s\n", recv_paragraphs[i][j]);
                    }   
                } else {
                    fprintf(out, "%s\n", recv_paragraphs[i][j]);
                }   
            }
            fprintf(out, "\n");
        } 

        fclose(out);

        free(par_types);
        free(par_sizes);

        for(int i = 1; i < total_par_count; i++) {
            for(int j = 0; j < MAX_LINE_COUNT; j++) {
                free(recv_paragraphs[i][j]);
            }
            free(recv_paragraphs[i]);
        }
        free(recv_paragraphs);

        free(inFileName);
        free(outFileName);

    } 
    // Cod executat de workeri
    else {
        // alocare buffer pentru primirea paragrafului de la procesul master
        char ** paragraph = (char **) calloc(MAX_LINE_COUNT, sizeof(char *));
        if(!paragraph) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }
        for(int i = 0; i < MAX_LINE_COUNT; i++) {
            paragraph[i] = (char *) calloc(MAX_LINE_LEN, sizeof(char *));
            if(!paragraph[i]) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); }
        }

        // bucla in care se realizeaza primirea unui paragraf, prelucrarea lui si trimitera 
        // paragrafului prelucrat la procesul MASTER
        while(1) {

            int paragraph_idx;
            int line_count;

            // primire indice paragraf, numar de linii ale paragrafului
            MPI_Recv(&paragraph_idx, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &s);
            MPI_Recv(&line_count, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &s);

            // primire linii paragrafe
            for(int i = 0; i < line_count; i++) {
                 MPI_Recv(paragraph[i], MAX_LINE_LEN, MPI_CHAR, MASTER, 1, MPI_COMM_WORLD, &s);
            }

            if(!strcmp(paragraph[0], "done")) {
                // procesul a primit toate paragrafele
                break;
            } 

            // ----- THREAD-URI PROCESARE TEXT


            // calcul numar de thread uri
            int thread_count = getThreadCount(line_count); 

            // threaduri pentru procesare
            pthread_t process_threads[thread_count];

            int r;
            void *status;
            paragraph_lines p[thread_count];

            int lines_left = line_count;
            int current_line = 0;

            // se executa daca numarul de linii este mai mare de 20 * (P - 1)
            if(line_count > thread_count * 20) {
                // fiecare thread primeste pe cate 20 de linii si executa prelucrarea
                for(int i = 0; i < line_count - thread_count * 20; i+= thread_count * 20) {
                    for (int j = 0; j < thread_count; j++) {
                        // se construieste structura care va fi trimisa in thread function
                        p[j] = buildParagraphLines(paragraph, current_line, 20, rank);

                        current_line += 20;
                        lines_left -= 20;

                        // pornesc thread-urile cu cate 20 de linii
                        r = pthread_create(&process_threads[j], NULL, process_string_func, &p[j]);

                        if (r) {
                            printf("Eroare la crearea thread-ului %d\n", j);
                            exit(-1);
                        }
                    }

                    for (int j = 0; j < thread_count; j++) {
                        // thread-ul principal primeste liniile prelucrate
                        // de la thread-urile secundare
                        r = pthread_join(process_threads[j], &status);

                        p[j] = *((paragraph_lines *)status);

                        // actualizare paragraf cu liniile modificate
                        for(int k = 0; k < p[j].line_count; k++) {
                            memcpy(paragraph[p[j].first_line_index + k], 
                                p[j].lines[k], MAX_LINE_LEN);
                        }
                    }
                }
            }
            
            // partea urmatoare se executa atunci cand line_count < 20 * (P - 1)
            // (liniile ramase de la precedenta parcurgere)

            // numarul de thread-uri care primesc 20 de linii
            int max_capacity_thread_count = lines_left / 20;
           
            paragraph_lines last_p[max_capacity_thread_count + 1];

            // executia thread-urilor care primesc 20 de linii
            for(int i = 0; i < max_capacity_thread_count; i++) {
                // constructie structura care va fi trimisa la thread
                last_p[i] = buildParagraphLines(paragraph, current_line, 20, rank);

                current_line += 20;
                lines_left -= 20;
            
                // pornesc thread-urile cu cate 20 de linii
                r = pthread_create(&process_threads[i], NULL, process_string_func, &last_p[i]);

                if (r) {
                    printf("Eroare la crearea thread-ului %d\n", i);
                    exit(-1);
                }
            }

            for(int i = 0; i < max_capacity_thread_count; i++) {
                r = pthread_join(process_threads[i], &status);
                // thread-ul principal primeste liniile prelucrate de la thread-urile secundare
                last_p[i] = *((paragraph_lines *)status);

                // actualizare paragraf cu liniile modificate
                for(int k = 0; k < last_p[i].line_count; k++) {
                    memcpy(paragraph[last_p[i].first_line_index + k], 
                        last_p[i].lines[k], MAX_LINE_LEN);
                }
            }

            // executia ultimului thread cu liniile ramase (sub 20)
            if(lines_left > 0) {
                // construieste structura care va fi trimisa la thread
                last_p[max_capacity_thread_count] = buildParagraphLines(paragraph, 
                    current_line, lines_left, rank);

                // pornire thread
                r = pthread_create(&process_threads[max_capacity_thread_count], NULL, 
                    process_string_func, &last_p[max_capacity_thread_count]);

                if (r) {
                    printf("Eroare la crearea thread-ului %d\n", max_capacity_thread_count);
                    exit(-1);
                }


                r = pthread_join(process_threads[max_capacity_thread_count], &status);
                // preluare rezultat de la thread
                last_p[max_capacity_thread_count] = *((paragraph_lines *)status);

                // actualizare paragraf cu liniile modificate
                for(int k = 0; k < last_p[max_capacity_thread_count].line_count; k++) {
                    memcpy(paragraph[last_p[max_capacity_thread_count].first_line_index + k], 
                        last_p[max_capacity_thread_count].lines[k],
                        MAX_LINE_LEN);
                }

                current_line += lines_left;
                lines_left = 0;
            }
            
            // -------- TRIMITERE DATE LA MASTER --------

            // trimitere numar paragraf la master
            MPI_Send(&paragraph_idx, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
            // trimitere numar de linii la master
            MPI_Send(&line_count, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);

            // trimitere linii prelucrate ale paragrafului curent la master
            for(int i = 0; i < line_count; i++) {
                 MPI_Send(paragraph[i], MAX_LINE_LEN, MPI_CHAR, MASTER, 1, MPI_COMM_WORLD);
            }

            // golire buffer pentru urmatoarea iteratie
            for(int i = 0; i < line_count; i++) {
                memset(paragraph[i], '\0', MAX_LINE_LEN); 
            }
            
        }

        // trimitere mesaj de incheiere a executiei catre thread-ul master
        int p = 0;
        int line_count = 1;
        
        MPI_Send(&p, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
        MPI_Send(&line_count, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD);
        MPI_Send(DONE_STR, strlen(DONE_STR), MPI_CHAR, MASTER, 1, MPI_COMM_WORLD);


        for(int i = 0; i < MAX_LINE_COUNT; i++) {
            free(paragraph[i]);
        }
        free(paragraph);
    }
    
    MPI_Finalize();
}

