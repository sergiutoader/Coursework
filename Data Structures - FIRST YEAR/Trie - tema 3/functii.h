// TOADER Sergiu-Cristian, 315CB
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define CHAR_SIZE 38 // 26 litere, 10 cifre, '.'' si '/'
#define MAX 200

typedef struct Trie
{
	int isLeaf;
	struct Trie* character[CHAR_SIZE];
} Trie;

Trie* newNode();
void destroyTrie(Trie * head);
int getIndex(char *str);
char getChar(int index);
void insert(Trie **head, char *str);
int search(Trie* head, char *str);

void mkdir(FILE * input, FILE * err, Trie *current_dir, Trie *file_system);
Trie * changeDir (char *path, Trie *head, char *str);
void cd(char nume_director[MAX], char path[MAX], FILE *err,
	Trie *file_system, Trie ** current_dir);
void pwd(char path[200], FILE * output);
void ls(Trie * current_dir, FILE *output, char *copii, int level);