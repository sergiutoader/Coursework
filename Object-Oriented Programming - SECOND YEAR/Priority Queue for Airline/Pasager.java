/* TOADER Sergiu-Cristian - 325CB */

/**
 * Clasa care inglobeaza atributele unui pasager (nume, varsta, tip de bilet,
 * beneficii) si ii atribuie un punctaj
 *
 */
public class Pasager {
	private int punctaj;
	
	private String nume;
	private int varsta;
	private Bilet bilet;
	private Beneficiu beneficii;

	public Pasager() {
	}

	public Pasager(String nume, int varsta, Bilet bilet, Beneficiu beneficii) {
		this.nume = nume;
		this.varsta = varsta;
		this.bilet = bilet;
		this.beneficii = beneficii;
		this.setPunctajFinal();
	}

	public int getPunctaj() {
		return punctaj;
	}

	/**
	 * Metoda care seteaza punctajul final al calatorului
	 * 
	 */
	public void setPunctajFinal() {
		punctaj = bilet.getPoints() + beneficii.getPoints();

		if (getVarsta() < 2) {
			setPunctaj(getPunctaj() + 20);
		} else if ((getVarsta() >= 2) && (getVarsta() < 5)) {
			setPunctaj(getPunctaj() + 10);
		} else if ((getVarsta() >= 5) && (getVarsta() < 10)) {
			setPunctaj(getPunctaj() + 5);
		} else if (getVarsta() >= 60) {
			setPunctaj(getPunctaj() + 15);
		}
	}

	public int getVarsta() {
		return varsta;
	}

	public void setVarsta(int varsta) {
		this.varsta = varsta;
	}

	public String getNume() {
		return nume;
	}

	public void setPunctaj(int punctaj) {
		this.punctaj = punctaj;
	}
}
