Nume: Toader Sergiu-Cristian
Grupa: 315CB

Structura bazei de date in care sunt stocate si prelucreate cardurile este
urmatoarea: LC (lista principala) este o lista simplu inlantuita care are ca
informatie elemente de tip LSC. LSC (sublista) este o lista simplu inlantuita
care are ca informatie elemente de tip cardInfo. cardInfo este o structura care
inglobeaza informatiile unui card (card_number, pin, etc.), precum si un
element de tip TCH. TCH (istoricul) este o lista simplu inlantuita care are ca
informatie siruri de caractere si stocheaza informatii despre tranzactiile
efectuate cu cardul respectiv.

===============================================================================

Alocarea / dezalocarea / stergearea celulelor / distrugerea listelor, precum si
completarea si prelucrarea informatiilor din liste se realizeaza prin
intermediul functiilor existente in fisierele functii_lista.c si
functii_card.c.

===============================================================================

Modul de implementare al functiei main este urmatorul: Se citeste din fisierul
de input numarul maxim de carduri si este alocata o lista de tip LC cu
dimensiunea data de acest numar. Se citesc dupa siruri de caractere
reprezentand operatia care trebuie efectuata pana la citirea caracterului EOF.
In functie de operatia citita, sunt cititi parametrii aditionali si se
apeleaza una din functiile de prelucrare a listei LC. La final sunt inchise
fisierele de input/output si este eliberata memoria prin apelarea functiei
DistrugeL. 

===============================================================================

Operatiile din main:

Operatia add_card - se citesc parametrii (functia citire_date_card), se
determina pozitia pe care trebuie introdus cardul si pozitia maxima si se
verifica existenta cardului in baza de date. Daca exista cardul in baza, se
afiseaza mesaj de eroare si se trece la urmatoarea operatie. In caz contrar,
se verifica daca prin adaugarea cardului se depaseste numarul maxim de carduri,
se realoca o noua lista (copiindu-se cardurile anterioare in ea si dublandu-se
nr_max_carduri) daca este cazul si se adauga cardul curent in baza de date.


Operatia delete_card - se citeste card_number-ul, se determina pozitia pe
care poate fi in lista, se cauta in lista celula care contine acelasi card_
number si se elimina acea celula.

Operatia show - poate face 3 lucruri diferite:
-nimic, daca nu a mai fost introdus un card in baza de date
- afiseaza pozitia si informatiile despre toate cardurile daca nu este insotit
de niciun parametru si exista carduri in baza de date
- afiseaza pozitia si informatiile despre un singur card daca este insotit de
un card_number.

Operatia insert_card - se citeste card_number-ul si pin-ul, se localizeaza
cardul in lista. Daca pin-ul este cel corect si cardul nu este blocat, operatia
se executa cu succes, iar numarul de introduceri gresite se reseteaza la 0.
Pentru introduceri gersite ale pin-ului, se afeseaza mesajul de eroare si se
creste variabila wrong_pin. Cand ea ajunge la 3, cardul este blocat.

Operatia cancel - se citeste card_number-ul, se localizeaza cardul in lista si
se adauga comanda in istoric.

Operatia unblock_card - se citeste card_number_ul, se localizeaza cardul in
lista, se reseteaza variabila wrong_pin la 0 si se actualizeaza statusul la
"ACTIVE".

Operatia pin_change - se citeste card_number-ul si pin-ul, se localizeaza
cardul in lista, se verifica daca pinul nou este valid si daca este, se
actualizeaza variabila pin, statusul la "ACTIVE" si se trece comanda in
istoric.

Operatia balance_inquiry - se citeste card_number-ul, se localizeaza cardul in
lista, se afiseaza variabila balance si se trece comanda in istoric.

Operatiile recharge si cash_withdrawal - se citeste card_number-ul si suma, se
localizeaza card-ul in lista, se modifica variabila balance, si se actualizeaza
istoricul. In cazul cash_withdrawal se verifica inainte daca exista fonduri
suficiente si operatia se executa cu succes doar in caz afirmativ.

Operatiile transfer_funds si reverse_transaction - se citesc card_number-ele
pentru cardul sursa si cardul destinatie, si citeste suma, se localizeaza
cardurile in lista, se actualizeaza variabilele balance si istoricul pentru
ambele card-uri. Se verifica inainte daca soldul cardului sursa (la tranfer_
funds) respectiv soldul cardului destinatie (la reverse_transaction) este
suficitent si operatiile se executa cu succes doar in caz afirmativ.

===============================================================================

Punctaj obtinut local: 102/85

