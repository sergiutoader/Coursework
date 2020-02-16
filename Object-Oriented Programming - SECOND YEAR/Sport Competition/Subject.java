/* Toader Sergiu-Cristian, 325CB */

/**
 * Interface used in the Observer Design Pattern
 *
 */
public interface Subject {
	
	public void registerObserver(Observer o);
	public void removeObserver(Observer o);
	public void notifyObservers();
}
