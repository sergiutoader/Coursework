/* TOADER Sergiu-Cristian - 325CB */

import java.util.Arrays;

/**
 * Clasa ce implementeaza coada de prioritate
 *
 */
public class MaxHeap {
	private Entitate[] heap;
	private int size;
	private int maxSize;

	public MaxHeap() {
		this.maxSize = 100;
		this.size = 0;
		heap = new Entitate[this.maxSize + 1];
	}

	public Entitate getHeap(int i) {
		return heap[i];
	}

	public void setHeap(int i, Entitate e) {
		heap[i] = e;
	}

	private int parentNode(int pos) {
		return pos / 2;
	}

	private int leftChild(int pos) {
		return 2 * pos;
	}

	private int rightChild(int pos) {
		return 2 * pos + 1;
	}

	/**
	 * @param pos Pozitia curenta din heap
	 * @return Intoarce true daca nodul din heap este nod terminal
	 */
	private boolean isLeaf(int pos) {
		if (pos > (size / 2) && pos <= size) {
			return true;
		}
		return false;
	}

	/**
	 * @param pos Pozitia curenta din heap
	 * @return Intoarce true daca nodul exista in heap
	 */
	private boolean isNode(int pos) {
		if (pos >= 1 && pos <= size) {
			return true;
		}
		return false;
	}

	/**
	 * Interschimba doua noduri din heap
	 * 
	 * @param fpos Prima pozitie din heap
	 * @param spos A doua pozitie din heap
	 */
	private void swap(int fpos, int spos) {
		Entitate tmp = getHeap(fpos);
		setHeap(fpos, getHeap(spos));
		setHeap(spos, tmp);
	}

	/**
	 * Metoda recursiva care realizeaza inteschimbari de noduri pentru a restabili
	 * proprietatea arborelui de maxHeap
	 * 
	 * @param pos Pozitia de la care se realizeaza refacerea Heapului
	 */
	private void maxHeapify(int pos) {
		if (isLeaf(pos)) {
			return;
		}

		if (getHeap(pos).compareTo(getHeap(leftChild(pos))) < 0
				|| getHeap(pos).compareTo(getHeap(rightChild(pos))) < 0) {

			if (getHeap(leftChild(pos)).compareTo(getHeap(rightChild(pos))) >= 0) {
				swap(pos, leftChild(pos));
				maxHeapify(leftChild(pos));
			} else {
				swap(pos, rightChild(pos));
				maxHeapify(rightChild(pos));
			}
		}
	}

	/**
	 * Metoda care insereaza in heap o entitate si o plaseaza prin swap-uri in
	 * pozitia corecta
	 * 
	 * @param e        Entitatea ce trebuie adagurata
	 * @param priority Prioritatea cu care este ea adaugata
	 */
	public void insert(Entitate e, int priority) {

		e.setPunctaj(priority);

		if (size == maxSize - 1) {
			maxSize *= 2;
			heap = Arrays.copyOf(heap, maxSize);
		}
		setHeap(++size, e);

		// Plasare entitate in pozitia corecta
		int current = size;
		while ((getHeap(current).compareTo(getHeap(parentNode(current))) > 0) && (current > 1)) {
			swap(current, parentNode(current));
			current = parentNode(current);
		}
	}

	/**
	 * Metoda list()
	 * 
	 * @return Un sir de caractere cu heap-ul curent in preordine
	 */
	public String list() {
		StringBuilder s = new StringBuilder();
		appendHeap(s, 1);
		s.setLength(s.length() - 1);
		return s.toString();
	}

	/**
	 * Functie recursiva care concateneaza la s parcurgerea in preordine a heap-ului
	 * 
	 * @param s   Builder pentru sirul intors de metoda list()
	 * @param pos Pozitia nodului ai carui copii se concateneaza la s
	 */
	private void appendHeap(StringBuilder s, int pos) {
		if (isNode(pos)) {
			s.append(getHeap(pos).getId() + " ");
			appendHeap(s, leftChild(pos));
			appendHeap(s, rightChild(pos));
		}
	}

	/**
	 * Elimina varful cozii de prioritate
	 * 
	 * @return Elementul eliminat
	 */
	public Entitate embark() {
		Entitate popped = getHeap(1);
		setHeap(1, getHeap(size--));
		maxHeapify(1);
		return popped;
	}

	/**
	 * Elimina entitatea e din heap
	 * 
	 * @param e Entitatea ce trebuie eliminata din heap
	 */
	public void delete(Entitate e) {


		for (int i = 1; i <= size; i++) {
			if (getHeap(i).getPunctaj() == e.getPunctaj()) {

				if(getHeap(i).getDeletePasager()){
					deletePasager(getHeap(i), getHeap(i).getNumePasagerSters());
					return;
				}
				setHeap(i, getHeap(size--));

				// Refacere heap dupa eliminare (doar daca nu s-a eliminat ultimul element din
				// heap)
				if (i <= size) {
					maxHeapify(i);
				}
				return;
			}
		}
	}

	/**
	 * Elimina un pasager dintr-o entitate din heap
	 * 
	 * @param e    Entiatea din care se elimina pasagerul
	 * @param nume Numele pasagerului ce trebuie eliminat
	 */
	public void deletePasager(Entitate e, String nume) {
		for (int i = 1; i <= size; i++) {
			if (getHeap(i).getPunctaj() == e.getPunctaj()) {
				heap[i].eliminaPasager(nume);

				// Daca entitatea nu mai are pasageri este eliminata din heap
				if (getHeap(i).getIdx() == 0) {
					delete(heap[i]);
				}

				if (i <= size) {
					maxHeapify(i);
				}
				return;
			}
		}
	}
}