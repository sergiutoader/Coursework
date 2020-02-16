/* TOADER Sergiu-Cristian - 325CB */

/**
 * Clasa care retine atributele de imbarcare prioritara si de nevoi speciale ale
 * unui bilet
 *
 */
public class Beneficiu {
	private boolean imbarcarePrioritara;
	private boolean nevoiSpeciale;

	public Beneficiu(boolean imbarcarePrioritara, boolean nevoiSpeciale) {
		this.imbarcarePrioritara = imbarcarePrioritara;
		this.nevoiSpeciale = nevoiSpeciale;
	}

	public boolean isImbarcarePrioritara() {
		return imbarcarePrioritara;
	}

	public void setImbarcarePrioritara(boolean imbarcarePrioritara) {
		this.imbarcarePrioritara = imbarcarePrioritara;
	}

	public boolean isNevoiSpeciale() {
		return nevoiSpeciale;
	}

	public void setNevoiSpeciale(boolean nevoiSpeciale) {
		this.nevoiSpeciale = nevoiSpeciale;
	}

	/**
	 * Metoda intoarce numarul de puncte in functie de valorile de adevar ale
	 * campurilor imbarcarePrioritara si nevoiSpeciale
	 * 
	 * @return Punctajul cumulat de cele 2 atribute
	 * 
	 */
	public int getPoints() {
		int points = 0;
		if (isNevoiSpeciale()) {
			points += 100;
		}

		if (isImbarcarePrioritara()) {
			points += 30;
		}
		return points;
	}
}
