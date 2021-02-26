# Toader Sergiu-Cristian, 335CB - Tema 1 LFA
import sys

######### Citire #########

# citire parametri din linia de comanda
inFile = sys.argv[1]	# numele fisierului de input
outFile = sys.argv[2]	# numele fisierului de output

# deschidere fisier de input
f = open(inFile, 'r')

# citire string1 (pattern) si string2 (text)
pattern = f.readline().rstrip()
text = f.readline().rstrip()
# inchidere fisier de input
f.close()


######## Functii #########

# functie care intoarce litera de pe o pozitie din alfabet
def get_letter(idx):
	return chr(idx + ord('A'))

# functie care intoarce pozitia unei litere din alfabet
def get_index(letter):
	return ord(letter) - ord('A')

# intoarce lungimea celui mai lung sufix din string care se potriveste cu un prefix din pattern
def get_longest_suffix(string, pattern):
	
	# nu are rost sa inceapa cautarea de la inceputul string-ului, ci de la len(string) - len(pattern)
	start = 0
	if len(string) > len(pattern):
		start = len(string) - len(pattern)

	# subsirul in care se cauta sufixul
	# practic, ultimele len(pattern) caractere din string se verifica
	search_str = string[start:]

	# verificare daca ultimul caracter se afla in pattern
	# se evita astfel comparatii redundante
	if not search_str[len(search_str) - 1] in pattern:
		return 0

	# prin prelucrarile anterioare, ne asiguram ca realizam maxim len(pattern) cautari
	# se calculeaza cel mai lung sufix din string care face match pe pattern
	for i in range(len(search_str)):
		# comparatie sufixe de dimensiune len(search_str), len(search_str ) - 1, ... din 
		# string cu prefixe de aceeasi lungime din pattern
		if search_str[i:] == pattern[:len(search_str[i:])]:
			# pentru prima potrivire gasita, se intoarce lungimea
			return len(search_str) - i
	return 0


# functie de calculare a matricii delta
def compute_delta():
	# constanta pentru dimensiunea alfabetului A-Z
	ALPHABET_LENGTH = 26

	# delta = matrice cu len(pattern) + 1 linii si ALPHABET_LENGTH coloane
	# delta[i][j] = indicele starii in care se ajunge daca atasam prefixului
	# pattern_prefix de dimensiune i caracterul din alfabet de la pozitia j 
	delta = [[]]
	# prefixul din pattern corespunzator unei linii din matricea delta 
	pattern_prefix = ''

	# setarea primei linii din matricea delta (1 pentru prima litera din pattern, 0 in rest)
	for j in range(ALPHABET_LENGTH):
		delta[0].append(0)
	delta[0][get_index(pattern[0])] = 1

	# setarea celorlalte linii din matricea delta
	for i in range(1, len(pattern) + 1):
		# inserare linie noua in delta
		delta.append([])
		
		# se adauga la pattern_prefix caracterul urmator din pattern
		pattern_prefix += pattern[i - 1]  
		
		# se concateneaza fiecare caracter din alfabet la prefix si se calculeaza 
		# starea urmatoare prin apelul functiei get_longest_suffix pentru toata linia i
		for j in range(ALPHABET_LENGTH):
			string = pattern_prefix + get_letter(j)
			delta[i].append(get_longest_suffix(string, pattern))
	return delta

# functie de construire a sirului de caractere continand output-ul programului
def create_output(delta):
	# sirul de caractere compus din indicii la care se gaseste pattern-ul in text
	output = ''

	# starea curenta
	state = 0

	# se parcurge tot textul si se calculeaza pe baza matricii
	# delta starea la fiecare iteratie prin text
	for index in range(len(text)):
		state = delta[state][get_index(text[index])]
		# daca starea este chiar lungimea pattern-ului, am identificat un pattern
		# se salveaza in output pozitia de start a pattern-ului in text
		if state == len(pattern):
			output += (str(index - len(pattern) + 1) + ' ')

	output += ('\n')
	return output


######### Apelare functii de creare delta si output si afisare rezultat #########

delta = compute_delta()
output = create_output(delta)

# deschidere fisier de output
g = open(outFile, 'a+')

# scrire in fisier rezultat final
g.write(output)
# inchidere fisier de output
g.close()
