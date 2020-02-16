/* TOADER Sergiu-Cristian - 325CB */

/**
 * Clasa care retine tipul de bilet, codificat cu un caracter
 *
 */
public class Bilet {
	private char tipBilet;

	public Bilet(char tipBilet) {
		this.tipBilet = tipBilet;
	}

	public char getTipBilet() {
		return tipBilet;
	}

	public void setTipBilet(char tipBilet) {
		this.tipBilet = tipBilet;
	}

	/**
	 * @return Numarul de puncte in functie de tipul de bilet al pasagerului
	 */
	public int getPoints() {
		switch (getTipBilet()) {
		case 'e':
			return 0;
		case 'p':
			return 20;
		case 'b':
			return 35;
		}
		return -1;
	}
}
