/* Toader Sergiu-Cristian, 325CB */

/**
 * Class used to determine the score for handball games
 *
 */
public class HandballScore implements ScoreStrategy {

	/**
	 * Score is the sum of all players' scores in the team for men and the product
	 * of all players' scores in the team for women
	 */
	@Override
	public double getScore(Team team) {
		double score;
		if (team.getGender().equals("masculin")) {
			score = 0;
			for (Player p : team.getPlayers()) {
				score += p.getScore();
			}
		} else {
			score = 1;
			for (Player p : team.getPlayers()) {
				score *= p.getScore();
			}
		}
		return score;
	}

}
