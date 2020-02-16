/* Toader Sergiu-Cristian, 325CB */

/**
 * Class used to determine score for football games
 *
 */
public class FootballScore implements ScoreStrategy {

	/**
	 * Score is the sum of all players' scores added to the highest score of one
	 * player in the team for men and the sum of all players' score added to the
	 * lowest score of one player in the team for women
	 */
	@Override
	public double getScore(Team team) {
		double score;
		if (team.getGender().equals("masculin")) {
			score = team.getMaximumScore();
		} else {
			score = team.getMinimumScore();
		}

		for (Player p : team.getPlayers()) {
			score += p.getScore();
		}
		return score;
	}

}
