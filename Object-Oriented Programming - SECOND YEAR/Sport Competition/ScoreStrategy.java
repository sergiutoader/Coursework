/* Toader Sergiu-Cristian, 325CB */

/**
 *  Interface used to implement the Strategy design pattern. 
 *  Used to separate score calculation from the team classes
 *
 */
public interface ScoreStrategy {
	public double getScore(Team team);
}
