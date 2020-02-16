/* Toader Sergiu-Cristian, 325CB */

/**
 * Class used to determine score for basketball games
 *
 */
public class BasketballScore implements ScoreStrategy {

	
	/**
	 * Score is the average of all players' scores
	 */
	@Override
	public double getScore(Team team) {
		double score = 0;
		
		for(Player p : team.getPlayers()) {
			score += p.getScore();
		}
		return score / team.getNumberOfPlayers();
	}

}
