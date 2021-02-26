#include "stringProcessing.h"

#define MAX_LINE_LEN 5000

/* Intoarce noua dimensiune a paragrafului
	modificat de functia doubleConsonants */
int getNewHorrorLen(char * paragraph) {
	int len = 0;

	for(int i = 0; i < strlen(paragraph) + 1; i++) {
		// Verificare caracter este litera mica
		if(paragraph[i] >= 'a' && paragraph[i] <= 'z') {
			// Verificare caracter nu este vocala 
			if(!strchr("aeiou", paragraph[i])) {
				// Litera se va dubla
				len++;
			}
		} // Verificare daca acest caracter este litera mare 
		else if(paragraph[i] >= 'A' && paragraph[i] <= 'Z') {
			// Verificare caracter nu este vocala 
			if(!strchr("AEIOU", paragraph[i])) {
				// Litera se va dubla
				len++;
			}
		}
		len++;
	}

	return len;
}

/* Dubleaza consoanele cuvintelor unui cuvant (doar in minuscula) */
char * doubleConsonants(char * word) {
	
	// Calculare lungime cuvant nou
	int len = getNewHorrorLen(word);

	// Alocare cuvant nou
	char * newWord = (char *) malloc(len);
	if(!newWord) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); };

	int idx = 0;
	for(int i = 0; i < strlen(word) + 1; i++) {

		newWord[idx++] = word[i];

		// Dublare daca avem un caracter care este consoana
		if(word[i] >= 'a' && word[i] <= 'z') {
			// Verificare caracter nu este vocala 
			if(!strchr("aeiou", word[i])) {
				newWord[idx++] = word[i];
			}
		} else if(word[i] >= 'A' && word[i] <= 'Z') {
			// Verificare caracter nu este vocala 
			if(!strchr("AEIOU", word[i])) {
				// litera se va dubla (in minuscula)
				newWord[idx++] = tolower(word[i]);
			}
		}
	}

	return newWord;
}


/* Functie care face upper pe fiecare litera para din cuvantul dat ca input */
char * upperEvenChars(char * word) {
	char * newWord = (char *) calloc(strlen(word) + 1, sizeof(char));
	if(!newWord) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); };
	// se copiaza continutul vechiului cuvant
	memcpy(newWord, word, strlen(word));

	// se modifica literele
	if(strlen(newWord) > 1) {
		for(int i = 1; i <= strlen(newWord); i += 2) {

			if(newWord[i] >= 'a' && newWord[i] <= 'z') {
				newWord[i] = toupper(newWord[i]);
			}
		}
	}

	return newWord;
}

/* Functie care face upper pe primul caracter
	dintr-un cuvant daca acesta este litera mica */
char * upperFirstChar(char * word) {
	char * newWord = (char *) malloc(strlen(word) + 1);
	if(!newWord) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); };

	memcpy(newWord, word, strlen(word) + 1);

	if(strlen(newWord) > 0) {
		if(newWord[0] >= 'a' && newWord[0] <= 'z') {
			newWord[0] = toupper(newWord[0]);
		}
	}
	
	return newWord;
}

/* Functie care intoarce cuvantul inversat */
char * reverseWord(char * word) {
	char len = strlen(word) + 1;

	char *newWord = (char *)calloc(len, sizeof(char));
	if(!newWord) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); };

	// inversare litere
	for(int i = 0; i < len - 1; i++) {
		newWord[i] = word[len - i - 2];
	}
     
    return newWord;
}


// Functie utilizata in applyDelimiter
// - concateneaza delimitatorul la noul paragraf
void concatDelimiter(char * paragraph, char * token, char * duplicate, char * newParagraph) {
	char delim = duplicate[token - paragraph + strlen(token)];
	char delim_str[2] = "\0";
	delim_str[0] = delim;
	
	memcpy(newParagraph + strlen(newParagraph), delim_str, strlen(delim_str) + 1);
}

/* 	Functie care aplica efectul filter pe cuvintele unui paragraf
	MAX_LINE_LEN este lungimea noului paragraf
	word_frequency reprezinta frecventa cu care filtrul este aplicat
*/
void applyFilter(char ** paragraph, char * (*filter)(char *), int word_frequency) {
	char * newParagraph = (char *)calloc(MAX_LINE_LEN, sizeof(char));
	if(!newParagraph) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); };

	// pointer folosit pentru pastrarea contextului la apeluri succesive
	char *saveptr;
	// duplicat al paragrafului folosit pentru determinarea delimitatorului folosit
	char *duplicate = (char *)calloc(MAX_LINE_LEN, sizeof(char));
	if(!duplicate) { fprintf(stderr, "Eroare malloc\n"); exit(EXIT_FAILURE); };
	memcpy(duplicate, *paragraph, strlen(*paragraph) + 1);

	// split pe cuvintele din paragraful original
	char * token = strtok_r(*paragraph, " \n", &saveptr);
	
	// cuvantul curent din paragraf
	int word_idx = 1;
	
	if(token) {
		// aplicare filtru pe cuvant (daca acesta respecta regula de frecventa)
		// + concatenare cuvant modificat si delimitator
		if(word_idx % word_frequency == 0) {
			char * filtered_token = filter(token);
			memcpy(newParagraph + strlen(newParagraph), filtered_token, strlen(filtered_token) + 1);
			free(filtered_token);
		} else {
			memcpy(newParagraph + strlen(newParagraph), token, strlen(token) + 1);
		}
		concatDelimiter(*paragraph, token, duplicate, newParagraph);
		word_idx++;

		while(1) {
			token = strtok_r(NULL, " \n", &saveptr);

			if(token) {
				// aplicare filtru pe cuvant (daca actualul cuvant respecta
				// regula de frecventa + concatenare cuvant modificat si delimitator
				if(word_idx % word_frequency == 0) {
					char * filtered_token = filter(token);
					memcpy(newParagraph + strlen(newParagraph), filtered_token, strlen(filtered_token) + 1);
					
					free(filtered_token);
				} else {
					memcpy(newParagraph + strlen(newParagraph), token, strlen(token) + 1);
				}
				concatDelimiter(*paragraph, token, duplicate, newParagraph);
				word_idx++;
				
			} else {
				break;
			}
		}
	}

	// adaugare spatiu la finalul liniei
	memcpy(*paragraph, newParagraph, MAX_LINE_LEN);
	if((*paragraph)[strlen(*paragraph) - 1] == '\n') {
		(*paragraph)[strlen(*paragraph) - 1] = ' ';
	}
	
	free(duplicate);
}

/* Functie care aplica efectul de horror pe fiecare cuvant dintr-un paragraf */
void applyHorror(char ** paragraph) {
	applyFilter(paragraph, doubleConsonants, 1);
}

/* Functie care aplica efectul de comedy pe fiecare cuvant dintr-un paragraf */
void applyComedy(char ** paragraph) {
	applyFilter(paragraph, upperEvenChars, 1);
}

/* Functie care aplica efectul de fantasy pe fiecare cuvant dintr-un paragraf */
void applyFantasy(char ** paragraph) {
	applyFilter(paragraph, upperFirstChar, 1);
}

/* Functie care aplica efectul de science-fiction pe fiecare al
	7-lea cuvant dintr-un paragraf */
void applyScienceFiction(char ** paragraph) {
	applyFilter(paragraph, reverseWord, 7);
}
