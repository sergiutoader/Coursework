/* TOADER Sergiu-Cristian - 325CB */

import java.util.Scanner;

/**
 * Clasa care prelucreaza datele de intrare si retine entitatile inainte ca
 * acestea sa fie inserate in heap
 *
 */
public class DateIntrare {

	private PasagerSingur[] pasageriSinguri;
	private Grup[] grupuri;
	private Familie[] familii;

	private int nrPasageriSinguri;
	private int nrGrupuri;
	private int nrFamilii;

	private String[] id;
	private String[] nume;
	private int[] varsta;
	private Bilet[] bilet;
	private Beneficiu[] beneficii;

	public DateIntrare(int nrPasageri) {
		this.id = new String[nrPasageri];
		this.nume = new String[nrPasageri];
		this.varsta = new int[nrPasageri];
		this.bilet = new Bilet[nrPasageri];
		this.beneficii = new Beneficiu[nrPasageri];

		this.pasageriSinguri = new PasagerSingur[nrPasageri];
		this.grupuri = new Grup[nrPasageri];
		this.familii = new Familie[nrPasageri];

		this.nrPasageriSinguri = 0;
		this.nrGrupuri = 0;
		this.nrFamilii = 0;
	}

	/**
	 * Realizeaza citirea unei linii din fisierul de input
	 * 
	 * @param input Scannerul care citeste din fisier
	 * @param idx   Linia citita
	 */
	public void citireDate(Scanner input, int idx) {
		id[idx] = input.next();
		nume[idx] = input.next();
		varsta[idx] = input.nextInt();
		bilet[idx] = new Bilet(input.next().charAt(0));
		beneficii[idx] = new Beneficiu(input.nextBoolean(), input.nextBoolean());
	}

	/**
	 * Determina tipul de entitate din care face parte pasagerul si il insereaza in
	 * entitatea corespunzatoare
	 * 
	 * @param idx Linia curenta din fisierul de input
	 */
	public void adaugaPasager(int idx) {
		// Interpretare id (determinarea tipului de entitate)
		switch (getId(idx).charAt(0)) {
		case 'g':
			this.nrGrupuri = getId(idx).charAt(1) - 48;
			if (grupuri[nrGrupuri - 1] == null) {
				grupuri[nrGrupuri - 1] = new Grup(getId(idx));
			}
			// Adaugare pasager la grupul lui
			grupuri[nrGrupuri - 1].add(new Pasager(getNume(idx), getVarsta(idx), getBilet(idx), getBeneficiu(idx)));
			break;
		case 'f':
			this.nrFamilii = getId(idx).charAt(1) - 48;
			if (familii[nrFamilii - 1] == null) {
				familii[nrFamilii - 1] = new Familie(getId(idx));
			}
			// Adaugare pasager la familia lui
			familii[nrFamilii - 1].add(new Pasager(getNume(idx), getVarsta(idx), getBilet(idx), getBeneficiu(idx)));
			break;
		default:
			nrPasageriSinguri = Integer.parseInt(getId(idx).substring(1));
			pasageriSinguri[nrPasageriSinguri - 1] = new PasagerSingur(getId(idx));
			// Adaugare pasageri individuali
			pasageriSinguri[nrPasageriSinguri - 1]
					.add(new Pasager(getNume(idx), getVarsta(idx), getBilet(idx), getBeneficiu(idx)));
		}
	}

	public String getId(int i) {
		return id[i];
	}

	public String getNume(int i) {
		return nume[i];
	}

	public int getVarsta(int i) {
		return varsta[i];
	}

	public Bilet getBilet(int i) {
		return bilet[i];
	}

	public Beneficiu getBeneficiu(int i) {
		return beneficii[i];
	}

	public PasagerSingur getPasagerSingur(int i) {
		return pasageriSinguri[i];
	}

	public Familie getFamilie(int i) {
		return familii[i];
	}

	public Grup getGrup(int i) {
		return grupuri[i];
	}
}
