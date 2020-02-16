/* TOADER Sergiu-Cristian - 325CB */

import java.util.Arrays;

/**
 * Clasa parinte pentru subclasele Familie, Grup, PasagerSingur. Realizeaza
 * manipularea vectorilor de pasageri
 * 
 */
public class Entitate implements Comparable {
	private Pasager[] entitate;
	private int idx;
	private int maxSize;
	private int punctaj;
	private String id;

	public Entitate(String id) {

		this.maxSize = 1;
		this.idx = 0;
		this.id = id;
		entitate = new Pasager[maxSize];

		this.deletePasager = false;
		this.numePasagerSters = null;
	}

	/**
	 * Seteaza punctajul initial pentru fiecare tip de entitate
	 * 
	 */
	public void setPunctajInitial(){};

	/**
	 * Adauga un pasager in vectorul entitate. Realizeaza redimensionarea vectorului
	 * folosind metoda copyOf din clasa Arrays. Actualizeaza punctajul entitatii,
	 * insumandu-l cu cel al pasagerului adaugat.
	 * 
	 * @param p pasagerul ce trebuie adaugat
	 * 
	 */
	public void add(Pasager p) {

		if (idx == maxSize) {
			maxSize *= 2;
			entitate = Arrays.copyOf(entitate, 2 * maxSize);
		}
		setPasager(getIdx(), p);
		setIdx(getIdx() + 1);

		setPunctaj(getPunctaj() + p.getPunctaj());
	}

	/**
	 * Realizeaza eliminarea pasagerului din entitate
	 * 
	 * @param nume Numele pasagerului eliminat
	 * 
	 */
	public void eliminaPasager(String nume) {
		for (int i = 0; i < idx; i++) {
			if (getPasager(i).getNume().equals(nume)) {
				setPunctaj(getPunctaj() - getPasager(i).getPunctaj());

				setPasager(i, getPasager(getIdx() - 1));
				setPasager(getIdx() - 1, null);
				setIdx(getIdx() - 1);
			}
		}
	}

	public int getIdx() {
		return idx;
	}

	public void setIdx(int idx) {
		this.idx = idx;
	}

	public int getPunctaj() {
		return punctaj;
	}

	public void setPunctaj(int punctaj) {
		this.punctaj = punctaj;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public Pasager getPasager(int i) {
		return entitate[i];
	}

	public void setPasager(int i, Pasager p) {
		entitate[i] = p;
	}

	/**
	 * Metoda care suprascrie metoda compareTo din interfata Comparable. Este
	 * folosita pentru a compara doua entitati in functie de punctaj
	 * 
	 */
	@Override
	public int compareTo(Object o) {
		if (o == null) {
			return 1;
		}
		return this.getPunctaj() - ((Entitate) o).getPunctaj();
	}


	private boolean deletePasager;
	private String numePasagerSters;

	public void setDeletePasager(boolean b){
		deletePasager = b;
	}

	public boolean getDeletePasager(){
		return deletePasager;
	}

	public void setNumePasagerSters(String nume){
		numePasagerSters = nume;
	}

	public String getNumePasagerSters(){
		return numePasagerSters;
	}
}