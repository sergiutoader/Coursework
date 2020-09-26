Nume: 	Toader Sergiu-Cristian
Grupa: 	325CB
				
				Tema 2 - Protocoale de Comunicatii


Observatii:

1) Aplicatia este implementata sa suporte un un numar oricat de mare de clienti,
topiuri si abonamente, in limita memoriei calculatorului. Toate structurile de
date folosite sunt fie liste simple inlantuite, fie vectori de structuri care
se realoca la capacitate dubla atunci cand capacitatea maxima a fost atinsa.

2) Alte fisiere sursa utilizate: 	-helpers.c - contine functiile folosite atat
in client, cat si in server
									-list.c - contine implementarea listelor si
operatiilor pe liste de tip char* si int
									-msg_list.c - contine implementarea listelor
si operatiilor pe liste ce structuri de tipul server_tcp_msg

3) Structuri definite in helpers.h:
- udp_msg - structura pentru un mesaj pe ruta: client udp -> server. Contine
campuri pentru topic, data_type si payload
- server_tcp_msg - structura pentru un mesaj pe ruta server -> client tcp.
Contine ip-ul sursa al mesajului, portul sursa si un element de tip udp_msg,
continand datele mentionate anterior.
- tcp_server_msg - structura pentru un mesaj pe ruta client tcp -> server.
Contine type (o codificare pentru subscribe - 0 / unsubscribe - 1), topic-ul
mesajului si un camp pentru SF
client - structura folosita in server pentru retinerea informatiilor despre un
client (id-ul, socketul activ, lista de topicuri la care este abonat si o lista
de valori pentru SF care corespund tipului de abonament).
-topic_user_list - structura pentru lista de clienti a unui topic. Contine
numele topicului si o lista de clienti abonati.
-offline_client_queue - structura utilizata pentru retinerea informatiilor
despre clientii offline. Primul camp este un client (cel deconectat). Al doilea
camp este o lista de mesaje (la care se adauga mesajele ce trebuie trimise
trimise catre client la reconectare).

================================================================================

Tratarea cazurilor de eroare:

Atat in client, cat si in server, dupa orice functie care este apel de sistem,
se apeleaza functia DIE cu care se verifica succesul executiei acestui apel.

Serverul - accepta de la tastatura doar comanda exit, orice alt input de la
tastatura este ignorat.

Clientul 	- accepta doar urmatoarele comenzi: exit, subscribe <topic> <SF>,
unsubscribe <topic>. Pentru orice alta comanda, cat si pentru comanda subscribe
cu SF diferit de 0 sau 1, se afiseaza "Invalid input."
			- conectarea cu id deja inregistrat in server nu va fi permisa.
Serverul va verifica existenta id-ului in baza de date si va transmite inapoi un
mesaj cu continutul "taken" daca acest id exista deja. Clientul verifica daca a
primit acest mesaj si in acel caz inchide conexiunea.
			- nu vor fi permise abonari la topic-uri la care clientul este deja
abonat sau dezabonari la topicuri la care clientul nu este abonat. Acesta retine
in vectorul topic_count topic-urile la care este abonat. Atunci cand clientul se
aboneaza la un topic cu succes, se adauga in vector numele topicului. In cazul
unei dezabonari realizate cu succes, se elimina din vector acea valoare. De
asemenea, in cazul in care un client se aboneaza la anumite topic-uri cu SF = 1
si se deconecteaza de la server, serverul va retine in continuare topicurile la
care este abonat si, la reconectare, va trimite clientului pachete continand
aceste topicuri pentru a le adauga topic_list. Deoarece dimensiunea unui pachet
va fi doar sizeof(char *), TCP in multe situatii uneste aceste mesajele. De
aceea, se va verifica rezultatul intors de functia recv si va realiza separarea
acestor mesaje.

================================================================================

Detalii implementare client:

Se deschide socket-ul si se realizeaza conectarea pe socket-ul deschis. Se
adauga in multimea de file-descriptori cei doi FD cu care clientul va
interactiona (STDIN si socket-ul creat). Dupa conectare, se trimite catre server
un mesaj continand id-ul serverului. Mai departe, actiunile clientului vor fi
decise intr-ul loop infinit in care se face select pe multumea de file descript-
ori, verificand permanent daca a primit date de la vreun FD.
In momentul in care primiste date de la STDIN, functia FD_ISSET intoarce un
rezultat diferit de 0. Se realizeaza citirea in buffer. Daca s-a introdus
comanda "exit", se inchide iese din while si se inchide programul. Se verifica
mai departe validitatea inputului (vezi: Tratarea cazurilor de eroare). Se
verifica apoi daca topicul este deja in lista de abonamente. Daca comanda este
"subscribe", se trimite la server un mesaj pentru a se solicita abonarea la
topic-ul respectiv si se adauga la topicul la lista de topicuri. Daca comanda
este "unsubscribe", se verifica daca topicul exista in lista de abonamente. Daca
nu exista, se afiseaza un mesaj de eroare. In caz contrar, se elimina
abonamentul din lista de topicuri si se trimite la server un mesaj pentru a-l
informa ca nu mai este abonat la topic.
Daca primeste date de la server (file descriptor-ul pentru socket este setat),
se face receive la mesaj. Daca apelul functiei recv a intors 0, se afiseaza
mesajul "Lost connection to server" si se iese din while. Daca mesajul este
"taken", id-ul este folosit, deci se va afisa un mesaj de eroare si se iese din
while. Daca mesajul incepe cu "topic", se reface lista de topicuri la care
clientul era abonat (cazul SF = 1). Altfel, este un mesaj venit de la un client
UDP, deci acesta va fi afisat in formatul corespunzator folosind functia
print_message. La iesirea din while, se inchide socket-ul si se iese din
program.
Functia print_message: se construieste prima parte din mesaj folosind sprintf cu
parametrii ip, port si topic. Apoi, in functie de tipul de date, se ataseaza
si numele tipului si continutul, folosind o functie de afisare specifica pentru
intepretarea continutului: 	- pentru INT - se extrage semnul (byte-ul 1), se
extrage valoarea in modul (octetii 2-5 cu ntohl) si se intoarce rezultatul intr-
un char *
							- pentru SHORT_REAL - se extrage valoarea cu ntohs
se imparte la 100, se retine rezultatul cu 2 zecimale intr-un char *
							- pentru FLOAT - se extrage semnul, se extrage
mantisa (octetii 2-5) si exponentul (octetul 6), se inmulteste mantisa cu semnul
si se imparte la 10 de <valoarea exponentului> ori. Se retine rezultatul cu 6
zecimale intr-un char *
							- pentru STRING - nu se prelucreaza payload-ul


================================================================================

Detalii implementare server:

Structuri de date folosite: - online_users - retine clientii care sunt conectati
la server
							- offline_users - retine clientii deconectati care
sunt abonati la cel putin un topic cu optiunea store & forward
							- topics - retine informatii despre topicurile care
au cel putin un client abonat

Se deschide socket-ul pasiv pentru conexiunea cu clientii TCP. Se dezactiveaza
algoritmul Neagle folosind setsockopt cu parametrul TCP_NODELAY. Se apeleaza
bind si listen pe acest socket. Se deschide un socket si pentru comunicarea cu
clientii UDP. Se face bind la socket. Se adauga in multimea de file descriptori
STDIN, resprectiv cei doi socketi si se retine valoarea maxima.
Actiunile serverului sunt decise, ca si in cazul clientului, intr-un loop
infinit in care se face select pe multimea de socketi existenta. Se verifica
cu un for de la 0 la fdmax de la ce file descriptor au fost receptionate date.

Daca s-a primit o cerere de conectare pe socket-ul pasiv TCP, se creaza un
socket activ nou (new_client) folosind accept si se dezactiveaza in mod similar
ca la cel pasiv algoritmul Neagle. Apoi este receptionat mesajul de la client
care contine id-ul clientului folosind recv. Se verifica daca id-ul este
utilizat deja, folosind functia is_online care parcurge lista de clienti deja
conectati. Daca id-ul este utilizat, se trimite la client un mesaj cu continutul
"taken", se inchide socketul creat si se asteapta alt mesaj. Daca id-ul este
nou, verific daca userul a mai fost conectat inainte folosind functia is_offline
care parcurge lista de clienti offline. Daca a fost gasita o valoare diferita
de -1, se trimit folosind functia send_pending_msg toate mesajele catre acel
client care erau in asteptare si se elimina clientul din lista de clienti
offline. Se adauga utilizatorul in lista de clienti online, se afiseaza un mesaj
de notificare in server si se actualizeaza (daca e cazul) fd_max.

Functia send_pending_msg: - se parcurge lista de mesaje in asteptare pentru
client si se trimite fiecare mesaj pe rand. Apoi, se parcurge lista de topic-uri
si se transmit pe rand mesaje de tipul "topic <topic>" catre client, pentru a-i
reface lista de topic-uri la care fusese abonat inainte de deconectare.

Daca s-au primit informatii pe socket-ul udp, se face recvfrom intr-un buffer
de tip udp_msg in care se va vor retine informatiile ce trebuie trimise mai
departe la clientul tcp si cu parametrul cli_addr in care se vor retine ip-ul si
portul sursa. Daca se verifica daca topicul mesajului exista deja folosind
functia get_topic index care parcurge lista de topicuri si intoarce indicele
corespunzator pozitiei din lista sau -1 daca acesta nu exista. Mesajele care
au un topic la care nu este niciun client abonat, sunt ignorate. Daca topicul
exista, se construieste mesajul care va fi transmis la client cu ip-ul si portul
sursa si cu mesajul primit de la clientul udp, se parcurge lista de clienti
abonati la topic si se trimite mesajul la toti clientii online, iar pentru
clientii offline, se muta mesajul in coada de asteptare a acestuia, urmand sa
fie trimis la reconectare.

Daca s-au primit date de la STDIN, se citesc in buffer, si se inchide progamul
in cazul comenzii "exit", nu inainte sa se inchida toti socketii din multimea
read_fds. Orice alta comanda este ignorata.

Daca a ajuns un mesaj pe unul din socketii activi TCP, se face citirea folosind
recv. Daca rezultatul intors este 0 (client deconectat), se afiseaza un mesaj
de notificare si se inchide socket-ul. Se parcurg toate topicurile la care era
abonat. Se elimina clientul din lista tuturor topicurilor la care utilizatorul
era abonat cu SF = 0. Se adauga utilizatorul cu topicurile ramase ca intrare
noua in coada de asteptare pentru a stoca mesajele primite cat timp este
offline. Se elimina utilizatorul din lista de clienti online si se scoated din
multimea de file descriptori socket-ul sau.
Daca s-a primit un mesaj de tip "subscribe", se verifica daca topicul exista
deja. Daca nu exista, se adauga topicul la lista de topicuri cu utilizatorul
curent ca unic abonat, altfel, se localizeaza topicul in lista de topicuri si se
adauga utilizatorul care a dat subscribe in lista. In plus, se adauga topicul
in lista de topicuri a utilizatorului.
Daca s-a primit un mesaj de tip "unsubscribe", se verifica daca topicul exista
deja. Daca exista, se elimina abonatul din lista topicului, se elimina topicul
din lista abonatului, se elimina valoarea sf a acelui topic din lista
subscriberului, iar daca topic-ul nu mai are abonati, se elimina din lista de
topicuri.
