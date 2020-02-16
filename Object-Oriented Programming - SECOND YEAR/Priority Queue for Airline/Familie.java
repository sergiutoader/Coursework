/* TOADER Sergiu-Cristian - 325CB */
/**
 * Clasa copil ce mosteneste clasa Entitate si implementeaza metoda abstracta
 * setPunctajInitial()
 * 
 */
public class Familie extends Entitate {

	public Familie(String id) {
		super(id);
		setPunctajInitial();
	}

	/**
	 * Seteaza valoarea initiala 10 pentru toate familiile
	 * 
	 */
	@Override
	public void setPunctajInitial() {
		setPunctaj(10);
	}
}
