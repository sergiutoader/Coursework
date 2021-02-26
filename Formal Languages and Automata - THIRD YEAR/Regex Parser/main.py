# Toader Sergiu-Cristian, 335CB - Tema 3 LFA
import sys

### Clase 


# clasa folosita pentru tranzitiile dintr-un DFA
# contine campuri pentru starea sursa, starea destinatie si simbolul 
# pe care se realizeaza tranzitia
class DFA_Transition:
	def __init__(self, source_state, symbol, destination_state):
		self.source_state = source_state
		self.symbol = symbol
		self.destination_state = destination_state

# clasa pentru un DFA - contine stari si tranzitii
# starile si tranzitiile sunt retinute in set-urile states si transitions
class DFA:
	# initializare stari, tranzitii, numar de stari si un dictionar (stateMap)
	# folosit pentru maparea grupurilor de stari din NFA in stari singulare
	# pentru DFA
	def __init__(self):
		self.states = set()
		self.transitions = set()
		self.stateMap = {}
		self.stateCount = 0

	# metoda care adauga o stare noua in DFA si actualizeaza stateMap
	# se foloseste frozenset pentru a putea folosi o stare (un set de stari
	# din NFA) drept cheie
	def addState(self, state):
		self.states.add(frozenset(state))
		self.stateMap[frozenset(state)] = self.stateCount
		self.stateCount += 1

	# metoda care adauga o tranzitie la lista de tranzitii
	def addTransition(self, transition):
		self.transitions.add(transition)

	# intoarce o lista cu starile finale reprezentate prin numere
	# parcurge lista de stari ale DFA si verifica daca ele contin o stare
	# finala din DFA. Adauga la finalStates valoarea din map corespunzatoare
	# starilor gasite
	def getFinalStates(self, nfa_final_states):
		final_states = []
		for state in self.states:
			for nfa_final_state in nfa_final_states:
				if nfa_final_state in state:
					final_states.append(self.stateMap[state])
					break
		return final_states

# functie care intoarce inchiderea-epsilon a unei stari din NFA
def epsilon(state, transitions):
	epsilon = {state}
	for t in transitions:
		if t.initial_state in epsilon and t.symbol == 'eps':
			epsilon = epsilon.union(t.final_states)
	return epsilon


# clase care retin informatii despre expresii regulate

# Expr - interfata
class Expr:
	def __init__(self):
		pass

	def __str__(self) -> str:
		pass

# Concat - retine cele 2 subexpresii concatenate
class Concat(Expr):
	def __init__(self, e1, e2):
		self.e1 = e1
		self.e2 = e2

	def __str__(self):
		return self.e1.__str__() + self.e2.__str__()

# Reunion - retine cele 2 subexpresii in reuniune
class Reunion(Expr):
	def __init__(self, e1, e2):
		self.e1 = e1
		self.e2 = e2

	def __str__(self):
		return self.e1.__str__() + "|" + self.e2.__str__()

# Star - retine subexpresie din Kleene Star
class Star(Expr):
	def __init__(self, e):
		self.e = e

	def __str__(self):
		return self.e.__str__() + "*"

# Par - retine subexpresia din interiorul unei paranteze
class Par(Expr):
	def __init__(self, e):
		self.e = e

	def __str__(self):
		return "(" + self.e.__str__() + ")"


# Symbol - retine un caracter din alfabet
class Symbol(Expr):
	def __init__(self, sym):
		self.sym = sym

	def __str__(self):
		return str(self.sym)

# contine o stiva, implementeza operatiile de reducere a stivei, de preprocesare a cuvantului si de parsare a cuvantului
class PDA:
	def __init__(self):
		self.stack = []
		pass

	# elimina toate star-urile consecutive din cuvant
	def preprocess(self, word):
		new_word = ""
		for i in range(len(word)):
			if not(word[i] == "*" and word[i-1] == "*"):
				new_word += word[i]
		return new_word 

	# realizeaza operatia de push pe stiva
	def push(self, el):
		self.stack.append(el)

	# realizeaza operatia de pop pe stiva
	def pop(self):
		return self.stack.pop()

	# realizeaza reducerea unei expresii de tip Concat de pe stiva
	def reduceConcat(self):
		# extragere ultimele 2 expresii de pe stiva
		e2 = self.pop()
		e1 = self.pop()
		# se adauga pe stiva un obiect de tip Concat intre cele 2 expresii
		self.push(Concat(e1, e2))
		
	# realizeaza reducerea unei expresii de tip Reunion de pe stiva
	def reduceReunion(self):
		# extragere ultimele 2 expresii de pe stiva
		e2 = self.pop()
		# extragere simbol | de pe stiva
		self.pop() 
		e1 = self.pop()

		# se adauga pe stiva un obiect de tip Reunion intre cele 2 expresii
		self.push(Reunion(e1, e2))

	
	# realizeaza reducerea unei expresii de tip Star de pe stiva
	def reduceStar(self):
		# extragere expresie de pe stiva
		expr = self.pop()
		# adauga pe stiva un obiect de tip par de expresia extrasa
		self.push(Star(expr))

	# realizeaza reducerea unei expresii de tip Par de pe stiva
	def reducePar(self):
		# extragere expresie de pe stiva
		expr = self.pop()

		# extragere paranteza deschisa de pe stiva
		self.pop()

		# adauga pe stiva un obiect de tip par de expresia extrasa
		self.push(Par(expr))


	# functia care transforma cuvantul intr-o expresie regulata (Un obiect de tip Expr)
	def parse(self, word):

		# se parcurg caracterele din cuvant
		for i in range(len(word)):

			# deschidere paranteza - doar se adauga pe stiva
			if word[i] == "(":
				self.push(word[i])

			# am citit paranteza inchisa
			elif word[i] == ")":
				# realizez reducerea parantezei
				self.reducePar()

				# verifcare daca se poate realiza reducere de tip concatenare sau reuniune
				# este necesar ca urmatorul caracter sa nu fie star
				if (i < len(word) - 1 and not word[i+1] == "*") or i == len(word) - 1:

					# concatenare
					# extragere penultimul element de pe stiva (se verifica si daca acesta exista)
					if len(self.stack) > 1:
						secondToLastValue = self.stack[-2]
						# daca penultimul element din stiva este o expresie, se executa concatenare intre cele doua expresii
						if isinstance(secondToLastValue, Expr):
							self.reduceConcat()

					# reuniune
					# se realizeaza daca am ajuns la finalul expresiei sau la o paranteza inchisa (deoarece reuniunea se
					# realizeaza ultima - dupa star si concatenare)
					if i == len(word) - 1 or word[i+1] == ")":
						# se realizeaza toate reuniunile posibile de pe stiva (intre primele 2 subexpresii de pe stiva)
						while len(self.stack) > 2:
							secondToLastValue = self.stack[-2]
							thirdToLastValue = self.stack[-3]
							if secondToLastValue == "|" and isinstance(thirdToLastValue, Expr):
								self.reduceReunion()
							else:
								# daca nu se mai pot realiza reduceri, se realizeaza iesirea din bucla
								break


			# am citit "*"
			elif word[i] == "*":
				# realizam reducere star
				self.reduceStar()

				# verificam daca putem realiza reduceri de tip concat sau reuniune
				# este necesar ca urmatorul caracter sa nu fie star
				if (i < len(word) - 1 and not word[i+1] == "*") or i == len(word) - 1:
			
					# daca simbolul urmator din cuvant nu este * iar penultimul element de pe stiva este o expresie, executam concatenare
					if len(self.stack) > 1:
						secondToLastValue = self.stack[-2]
						if isinstance(secondToLastValue, Expr):
							self.reduceConcat()

					# se realizeaza daca am ajuns la finalul expresiei sau la o paranteza inchisa (deoarece reuniunea se
					# realizeaza ultima - dupa star si concatenare)
					if i == len(word) - 1 or word[i+1] == ")":
						while len(self.stack) > 2:
							# se realizeaza toate reuniunile posibile de pe stiva (intre primele 2 subexpresii de pe stiva)
							secondToLastValue = self.stack[-2]
							thirdToLastValue = self.stack[-3]
							if secondToLastValue == "|" and isinstance(thirdToLastValue, Expr):
								self.reduceReunion()
							else:
								# daca nu se mai pot realiza reduceri, se realizeaza iesirea din bucla
								break

			# am citit simbolul "|" - doar se pune pe stiva
			elif word[i] == "|":
				self.push(word[i])

			# am citit un caracter din alfabet
			elif word[i] in "abcd":
				# adauga simbol pe stiva
				self.push(Symbol(word[i]))

				# verificam daca putem realiza reduceri de tip concat sau reuniune
				# este necesar ca urmatorul caracter sa nu fie star
				if (i < len(word) - 1 and not word[i+1] == "*") or i == len(word) - 1:
					# daca simbolul urmator din cuvant nu este * iar penultimul element de pe stiva este o expresie, executam concatenare
					if len(self.stack) > 1:
						secondToLastValue = self.stack[-2]
						if isinstance(secondToLastValue, Expr):
							self.reduceConcat()
					# se realizeaza daca am ajuns la finalul expresiei sau la o paranteza inchisa (deoarece reuniunea se
					# realizeaza ultima - dupa star si concatenare)
					if i == len(word) - 1 or word[i+1] == ")":
						while len(self.stack) > 2:
							# se realizeaza toate reuniunile posibile de pe stiva (intre primele 2 subexpresii de pe stiva)
							secondToLastValue = self.stack[-2]
							thirdToLastValue = self.stack[-3]
							if secondToLastValue == "|" and isinstance(thirdToLastValue, Expr):
								self.reduceReunion()
							else:
								# daca nu se mai pot realiza reduceri, se realizeaza iesirea din bucla
								break
			
		return self.pop()


# clasa folosita pentru tranzitiile dintr-un NFA
# retine starea initiala, simbolul si starile finale
class NFA_Transition:
	def __init__(self, initial_state, symbol, final_states):
		self.initial_state = initial_state
		self.symbol = symbol
		self.final_states = final_states


# clasa care retine date despre un automat nedeterminist
# are campuri pentru numarul de stari, starea initiala, starea finala si tranzitii
class NFA:
	def __init__(self, state_count, initial_state, final_state, transitions):
		self.state_count = state_count
		self.initial_state = initial_state
		self.final_state = final_state
		self.transitions = transitions

# clasa care realizeaza conversia intre un obiect expresie regulata si un automat finit nedeterminist
class RegexNFAConverter:
	def __init__(self, reg_expr):
		self.reg_expr = reg_expr;

	# realizeaza conversia expresiei intr-un NFA, apeland in functie de tipul expresiei metoda de conversie corespunzatoare
	# conversia se va realiza recursiv, pe fiecare subexpresie din interiorul expresiei principale din care vor rezulta NFA-uri
	# mai mici, care vor fi combinate conform regulilor de baza pentru concatenare, reuniune si Kleene Star
	def convertExpr(self):
		if isinstance(self.reg_expr, Symbol):
			nfa = self.convertSymbol(self.reg_expr)
		elif isinstance(self.reg_expr, Concat):
			nfa = self.convertConcat(self.reg_expr)
		elif isinstance(self.reg_expr, Reunion):
			nfa = self.convertReunion(self.reg_expr)
		elif isinstance(self.reg_expr, Star):
			nfa = self.convertStar(self.reg_expr)
		elif isinstance(self.reg_expr, Par):
			nfa = self.convertPar(self.reg_expr)

		return nfa

	# functii pentru conversie simpla de la regex la nfa

	# de la o expresie regulata ce contine doar un simbol, construieste un nfa simplu, cu 2 stari si o tranzitie
	def convertSymbol(self, expr):
		state_count = 2		# 2 stari - initiala si finala
		initial_state = 0	# starea initiala 0
		final_state = 1		# starea finala - 1
		 # tranzitie de la starea initiala la starea finala pe simbolul expresiei
		transitions = [NFA_Transition(initial_state, expr.sym, [final_state])]

		nfa = NFA(state_count, initial_state, final_state, transitions)
		return nfa

	# de la o expresie de tip Concatenare, construieste un NFA ce leaga cele doua NFA-uri mai mici ale
	# subexpresiilor printr-o tranzitie
	def convertConcat(self, expr):
		# se extrag cele doua nfa-uri din expresiile componente

		# se verifica tipul de expresie, extragand nfa-ul corespunzator
		if isinstance(expr.e1, Symbol):
			nfa1 = self.convertSymbol(expr.e1)
		elif isinstance(expr.e1, Concat):
			nfa1 = self.convertConcat(expr.e1)
		elif isinstance(expr.e1, Reunion):
			nfa1 = self.convertReunion(expr.e1)
		elif isinstance(expr.e1, Star):
			nfa1 = self.convertStar(expr.e1)
		elif isinstance(expr.e1, Par):
			nfa1 = self.convertPar(expr.e1)

		if isinstance(expr.e2, Symbol):
			nfa2 = self.convertSymbol(expr.e2)
		elif isinstance(expr.e2, Concat):
			nfa2 = self.convertConcat(expr.e2)
		elif isinstance(expr.e2, Reunion):
			nfa2 = self.convertReunion(expr.e2)
		elif isinstance(expr.e2, Star):
			nfa2 = self.convertStar(expr.e2)
		elif isinstance(expr.e2, Par):
			nfa2 = self.convertPar(expr.e2)
	

		# numarul starilor va fi suma starilor din cele doua expresii - 1
		state_count = nfa1.state_count + nfa2.state_count - 1
		# stare initiala 0, stare finala - numarul starilor - 1
		initial_state = 0
		final_state = state_count - 1

		# tranzitiile din prima expresie se pastreaza
		transitions = nfa1.transitions

		# expresia a doua va avea starile initiale si finale ale tranzitiilor modificate - se adauga numarul de stari din prima expresie - 1
		for i in range(len(nfa2.transitions)):
			nfa2.transitions[i].initial_state += nfa1.state_count - 1
			for j in range(len(nfa2.transitions[i].final_states)):
				nfa2.transitions[i].final_states[j] += nfa1.state_count - 1
		nfa2.initial_state += nfa1.state_count - 1

		# adauga tranzitiile din nfa2
		transitions = transitions + nfa2.transitions

		# creare instanta nfa
		nfa = NFA(state_count, initial_state, final_state, transitions)

		return nfa

	# de la o expresie de tip Reuniune, construieste un NFA ce leaga cele doua NFA-uri mai mici ale
	# subexpresiilor prin doua stari suplimentare si 4 tranzitii-epsilon
	def convertReunion(self, expr):
		# se verifica tipul de expresie, extragand nfa-ul corespunzator
		if isinstance(expr.e1, Symbol):
			nfa1 = self.convertSymbol(expr.e1)
		elif isinstance(expr.e1, Concat):
			nfa1 = self.convertConcat(expr.e1)
		elif isinstance(expr.e1, Reunion):
			nfa1 = self.convertReunion(expr.e1)
		elif isinstance(expr.e1, Star):
			nfa1 = self.convertStar(expr.e1)
		elif isinstance(expr.e1, Par):
			nfa1 = self.convertPar(expr.e1)

		if isinstance(expr.e2, Symbol):
			nfa2 = self.convertSymbol(expr.e2)
		elif isinstance(expr.e2, Concat):
			nfa2 = self.convertConcat(expr.e2)
		elif isinstance(expr.e2, Reunion):
			nfa2 = self.convertReunion(expr.e2)
		elif isinstance(expr.e2, Star):
			nfa2 = self.convertStar(expr.e2)
		elif isinstance(expr.e2, Par):
			nfa2 = self.convertPar(expr.e2)

		# se vor adauga 2 stari noi,una initiala (din care pleaca 2 tranzitii epsilon catre starile initiale ale nfa1 si nfa2) si
		# una finala (in care ajung 2 tranzitii epsilon din starile finale ale nfa1 si nfa2)
		state_count = nfa1.state_count + nfa2.state_count + 2

		initial_state = 0
		final_state = state_count - 1

		# se adauga 2 eps-tranzitii de la starea initiala la vechile stari initiale ale celor 2 nfa-uri
		transitions = [NFA_Transition(initial_state, "eps", [initial_state + 1, initial_state + nfa1.state_count + 1])]

		# vechiul nfa1 va avea starile initiale si finale ale tranzitiilor modificate - se adauga 1 la numarul starii
		for i in range(len(nfa1.transitions)):
			nfa1.transitions[i].initial_state += 1
			for j in range(len(nfa1.transitions[i].final_states)):
				nfa1.transitions[i].final_states[j] += 1
		nfa1.initial_state += 1
		nfa1.final_state += 1

		# se adauga tranzitiile din nfa1 si o eps-tranzitie de la starea finala din nfa1 la starea finala din noul nfa
		transitions = transitions + nfa1.transitions
		transitions.append(NFA_Transition(nfa1.final_state, "eps", [final_state]))

		# vechiul nfa2 va avea starile initiale si finale ale tranzitiilor modificate - se adauga 1 + nfa1.state_count la numarul starii
		for i in range(len(nfa2.transitions)):
			nfa2.transitions[i].initial_state += 1 + nfa1.state_count
			for j in range(len(nfa2.transitions[i].final_states)):
				nfa2.transitions[i].final_states[j] += 1 + nfa1.state_count
		nfa2.initial_state += 1 + nfa1.state_count
		nfa2.final_state += 1 + nfa1.state_count

		# se adauga tranzitiile din nfa2 si o eps-tranzitie de la starea finala din nfa1 la starea finala din noul nfa
		transitions = transitions + nfa2.transitions
		transitions.append(NFA_Transition(nfa2.final_state, "eps", [final_state]))

		new_nfa = NFA(state_count, initial_state, final_state, transitions)
		return new_nfa

	# de la o expresie de tip Star, construieste un NFA ce repeta de 0 sau mai multe ori
	# cuvantul generat de subexpresie
	def convertStar(self, expr):
		# se verifica tipul expresiei continute in star si se intoarce un nfa construit recursiv
		if isinstance(expr.e, Par):
			old_nfa = self.convertPar(expr.e)
		elif isinstance(expr.e, Symbol):
			old_nfa = self.convertSymbol(expr.e)
		elif isinstance(expr.e, Concat):
			old_nfa =  self.convertConcat(expr.e)
		elif isinstance(expr.e, Reunion):
			old_nfa = self.convertReunion(expr.e)
		elif isinstance(expr.e, Star):
			old_nfa = self.convertStar(expr.e)

		# vor exista 2 stari noi
		state_count = 2 + old_nfa.state_count
		# starea initiala - are o eps-tranzitie in starea initiala a old_nfa si o eps-tranzitie in noua stare finala
		initial_state = 0
		# starea finala - primeste eps-tranzitii de la noua stare initiala si vechea stare finala
		final_state = state_count  - 1

		# initializare cu eps-tranzitii 0->1, 0->final state
		transitions = [NFA_Transition(initial_state, "eps", [1, final_state])]

		# nfa-ul al doilea va avea starile initiale si finale ale tranzitiilor modificate - se adauga 1 la numarul starii
		for i in range(len(old_nfa.transitions)):
			old_nfa.transitions[i].initial_state += 1
			for j in range(len(old_nfa.transitions[i].final_states)):
				old_nfa.transitions[i].final_states[j] += 1

		old_nfa.initial_state += 1

		# se adauga tranzitiile vechi la nfa
		transitions = transitions + old_nfa.transitions

		# se adauga tranzitiile-eps de la penultima stare la a doua, respectiv la ultima stare
		transitions.append(NFA_Transition(final_state - 1, "eps", [initial_state + 1, final_state]))

		# se creeaza noua instanta de nfa
		new_nfa = NFA(state_count, initial_state, final_state, transitions)
		return new_nfa


	# se verifica tipul de expresie continuta in paranteza si se converteste recursiv
	def convertPar(self, expr):
		if isinstance(expr.e, Par):
			return self.convertPar(expr.e)
		elif isinstance(expr.e, Symbol):
			return self.convertSymbol(expr.e)
		elif isinstance(expr.e, Concat):
			return self.convertConcat(expr.e)
		elif isinstance(expr.e, Reunion):
			return self.convertReunion(expr.e)
		elif isinstance(expr.e, Star):
			return self.convertStar(expr.e)


if __name__ == "__main__":
	
	# instanta pda
	pda = PDA()
	
	######### Citire #########

	# citire parametri din linia de comanda
	inFile = sys.argv[1]	# numele fisierului de input
	outFile1 = sys.argv[2]	# numele primului fisier de output
	outFile2 = sys.argv[3]	# numele celui de-al doilea fisier de output

	# deschidere fisier de input
	f = open(inFile, 'r')

	# citire cuvant
	word = f.readline()

	f.close()

	##### Parsare cuvant intr-un arbore #####

	# preprocesare - eliminare "*" redundante
	processed_word = pda.preprocess(word)

	# construire expresie regulata
	regex_tree = pda.parse(processed_word)

	# instantiere converter
	converter = RegexNFAConverter(regex_tree)

	# construire nfa
	nfa = converter.convertExpr()
	

	### SCRIERE ###
	# deschidere fisier de output
	g = open(outFile1, 'w+')

	# scriere numar de stari + stare finala
	g.write(str(nfa.state_count) + "\n")
	g.write(str(nfa.final_state) + "\n")

	# scriere tranzitii (stare initiala, simbol, stari finale)
	for tr in nfa.transitions:
		transition_line = ""
		transition_line += str(tr.initial_state) + " " + tr.symbol
		for final_state in tr.final_states:
			transition_line += " " + str(final_state)
		transition_line += "\n"
		g.write(transition_line)

	# inchidere fisier de output
	g.close()


	### NFA -> DFA ###

	# deschidere fisier de input
	f = open(outFile1, 'r')

	# citire linii fisier
	lines = f.readlines()

	# setare informatiii despre NFA
	sigma = set()									# alfabet

	nfa_state_count = int(lines[0]) 				# numar de stari

	nfa_final_states = lines[1].split() 			# stari finale
	for i in range(len(nfa_final_states)):
		nfa_final_states[i] = int(nfa_final_states[i])


	nfa_transitions = [] 							# tranzitii
	for i in range(2, len(lines)):
		split_line = lines[i].split()
		
		initial_state = int(split_line[0])
		symbol = split_line[1]
		if not symbol == "eps":
			sigma.add(symbol)						# adaugare simbol la alfabet

		final_states = set()
		for j in range(2, len(split_line)):
			final_states.add(int(split_line[j]))	# adaugare stari finale

		nfa_transitions.append(NFA_Transition(initial_state, symbol, final_states)) 

	f.close()										# inchidere fisier


	# DFA-ul construit pe baza NFA-ului
	dfa = DFA()

	# coada cu care se realizeaza parcurgerea tranzitiilor automatului nedeterminist
	# se initializeaza cu inchiderea lui 0 (starea initiala)
	queue = [epsilon(0, nfa_transitions)]
	# variabila care retine daca avem sinkstate
	hasSinkState = False

	# extragem din coada pana cand nu mai sunt stari in queue
	while len(queue) > 0:

		# se extrage urmatoarea stare / inchidere de stari
		source_states = queue.pop(0)
		# adaugare stare la dfa
		dfa.addState(source_states)

		# parcurgere fiecare litera din alfabet
		for sym in sigma:
			# starile destinatie pentru sursa source_states si simbolul sym
			# daca acest set ramane vid dupa parcurgerea tuturor tranzitiilor
			# pe simbolul curent, inseamna ca nu exista tranzitie pe simbol
			# din starea extrasa, deci trebuie adaugat un sinkstate 
			destination_states = set()

			# parcurgere fiecare stare din starea sursa:
			for source_state in source_states:
				# identificare tranzitii din starea source_state pe simbolul sym
				for nfa_transition in nfa_transitions:
					if source_state == nfa_transition.initial_state:

						if nfa_transition.symbol == sym:
							# calculul tuturor starilor destinatie din inchiderea curenta
							for final_state in nfa_transition.final_states:
								# reuniune intre inchiderea gasita si starile gasite anterior
								destination_states = destination_states.union(epsilon(final_state, nfa_transitions))

			# adaugam in coada grupul de stari gasite daca destinatia nu se
			# gaseste deja in coada sau in starile deja adaugate in dfa
			# set-ul vid semnifica sinkstate - a fost extras un set vid de stari 
			if not destination_states in queue and destination_states not in dfa.states and not source_states == set():
				queue.append(destination_states)

			# setare existenta sinkstate
			if len(destination_states) == 0:
				hasSinkState = True

			# daca starea initiala nu este sinkstate
			if not source_states == set():
				new_transition = DFA_Transition(source_states, sym, destination_states)
				dfa.addTransition(new_transition)

	# setare tranzitii (sinkstate, sym) -> sinkstate
	if hasSinkState:
		for sym in sigma:
			new_transition = DFA_Transition(set(), sym, set())
			dfa.addTransition(new_transition)

	# calcul stari finale si numar total de stari
	dfa_state_count = len(dfa.states)
	dfa_final_states = dfa.getFinalStates(nfa_final_states)

	### SCRIERE ###
	# deschidere fisier de output
	g = open(outFile2, 'w+')

	# numar de stari
	g.write(str(dfa_state_count) + '\n')
	# stari finale
	for state in dfa_final_states:
		g.write(str(state) + " ")
	g.write('\n')

	# afisare tranzitii
	# se extrag starile tranzitiilor folosind metoda stateMap
	for transition in dfa.transitions:
		transition_str = ""
		transition_str += (str(dfa.stateMap[frozenset(transition.source_state)]) + " ")
		transition_str += (transition.symbol + " ")
		transition_str += (str(dfa.stateMap[frozenset(transition.destination_state)]) + "\n")
		g.write(transition_str)

	# inchidere fisier de output
	g.close()
	