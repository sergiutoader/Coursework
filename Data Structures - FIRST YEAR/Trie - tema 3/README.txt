Nume: Toader Sergiu-Cristian
Grupa: 315CB


Sistemul de fisiere este implementat cu ajutorul unei structuri de trip Trie.
Strucura contine 2 campuri: isLeaf (intreg care ia valoarea 1 atunci cand litera
este frunza si 0 in caz contrar) si character, vector de pointeri cu maximum 38
de elemente alocate (pentru litere, cifre si caracterele '.' si '/').

================================================================================

Sunt declarate si initializate doua variabile de tip Trie * : file_system, care
reprezinta arborele complet al sistemului de fisiere si current_dir, care repre-
zinta un subarbore al sistemului de fisiere si care retine pozitia in care se
afla utilizatorul in sistem (initial, directorul current_dir coincide cu
file_system). O alta variabila importanta este path: un sir de caractere care
retine calea absoluta pana la directorul curent care este modificat in urma
operatiei 'cd' si care este afisat in fisierul de output la apelarea comenzii
'pwd'. Variabila path este initial "/", deoarece utilizatorul se afla la
inceputul executiei programului in root.

================================================================================

Este citit din fisierul de input numarul de operatii, dupa care se citeste fie-
care linie in parte, apelandu-se functia corespunzatoare operatiei de pe linia
curenta cu parametrii dati. La final este eliberata memoria alocata sistemului
de fisiere si inchise fisierele de output, input si pentru erori.

================================================================================

Functii folosite:

- getIndex si getChar:
	Prima primeste un sir de caractere si returneaza indicele din vectorul de
pointerii corespunzator primului caracter din sir; a doua realizeaza procesul
invers: primeste indicele si returneaza caracterul.

- insert si search:
	Ambele primesc ca parametru sistemul de fisiere si un sir de caractere.
Prima adauga sirul de caractere in sistemul de fisiere prin parcurgerea
caracterelor din sir si alocarea unui nod pe pozitia corespunzatoare caracteru-
lui curent. A doua parcurge iterativ arborele, caracter cu caracter, pana la
cand a ajuns la ultimul caracter din sir. Daca dupa ce realizat parcurgerea s-a
ajuns la un nod terminal (cu campul isLeaf = 1), functia returneaza 1 (s-a gasit
caracterul in arbore) sau 0 in caz contrar.

- mkdir:
	Functia realizeaza delimitarea parametrilor operatiei mkdir folosind strtok,
dupa care realizeaza inserarea sirului de caractere delimitat in arbore daca
aceasta se poate realiza (directorul nu trebuie sa existe deja iar directorul
parinte trebuie sa fie deja in sistemul de fisiere).

- changeDir:
	Functia este folosita in functia cd si are rolul de a actualiza variabila
path in care este retinuta calea absoluta si de a realiza avansul in arbore,
modificand directorul curent. 

- cd:
	Functia modifica calea absoluta si directorul curent, folosindu-se de calea
absoluta pentru a realiza revenirea in directorul parinte pentru operatii de
forma 'cd ..', sau de functia changeDir pentru avans in sistemul de fisiere.
Va afisa mesaje de eroare in cazul in care nu gaseste in sistem directoarele
primite ca parametru.

- ls:
	Se parcurge recursiv arborele retinandu-se in sirul de caractere 'copii'
fiecare copil al nodului curent. Parcurgerea recursiva se incheie atunci cand
se ajunge la finalul arborelui sau la caracterul '/'. Atunci cand in nodul
curent campul isLeaf are valoarea 1, se realizeaza afiseara sirului 'copii'.
