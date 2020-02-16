/* TOADER Sergiu-Cristian - 325CB */

/**
 * Clasa copil ce mosteneste clasa Entitate si implementeaza metoda abstracta
 * setPunctajInitial()
 * 
 */
public class PasagerSingur extends Entitate {

	public PasagerSingur(String id) {
		super(id);
		setPunctajInitial();
	}

	/**
	 * Seteaza valoarea initiala 0 pentru toti pasagerii cu bilet individual
	 * 
	 */
	@Override
	public void setPunctajInitial() {
		setPunctaj(0);
	}
}
