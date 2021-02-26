# Toader Sergiu-Cristian, 335CB - Tema 2 LFA
import sys

### Clase 

# clasa folosita pentru tranzitiile dintr-un NFA
# retine starea initiala, simbolul si starile finale
class NFA_Transition:
	def __init__(self, initial_state, symbol, final_states):
		self.initial_state = initial_state
		self.symbol = symbol
		self.final_states = final_states


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


if __name__ == "__main__":

	######### Citire #########

	# citire parametri din linia de comanda
	inFile = sys.argv[1]	# numele fisierului de input
	outFile = sys.argv[2]	# numele fisierului de output

	# deschidere fisier de input
	f = open(inFile, 'r')

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
	g = open(outFile, 'w+')

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
	