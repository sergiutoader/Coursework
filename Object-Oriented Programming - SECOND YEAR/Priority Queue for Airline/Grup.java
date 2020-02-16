/* TOADER Sergiu-Cristian - 325CB */

/**
 * Clasa copil ce mosteneste clasa Entitate si implementeaza metoda abstracta
 * setPunctajInitial()
 * 
 */
public class Grup extends Entitate {

	public Grup(String id) {
		super(id);
		setPunctajInitial();
	}

	/**
	 * Seteaza valoarea initiala 5 pentru toate grupurile
	 * 
	 */
	@Override
	public void setPunctajInitial() {
		setPunctaj(5);
	}
}
