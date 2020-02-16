/* Toader Sergiu-Cristian, 325CB */

/**
 * Subclass for Team. Is both visitor and client. It uses the Strategy Design
 * Pattern for calculating the score.
 *
 */
public class FootballTeam extends Team implements TeamVisitor {

	public FootballTeam(String teamName, String gender, int numberOfPlayers) {
		super(teamName, gender, numberOfPlayers);
		setStrategy(new FootballScore());
	}

	@Override
	public void visit(Team team) {
		double currentTeamScore = getStrategy().getScore(this);
		double visitedTeamScore = getStrategy().getScore(team);

		if (currentTeamScore > visitedTeamScore) {
			competition.updatePoints(getTeamName(), team.getTeamName(), "home win");
		} else if (currentTeamScore == visitedTeamScore) {
			competition.updatePoints(getTeamName(), team.getTeamName(), "draw");
		} else {
			competition.updatePoints(getTeamName(), team.getTeamName(), "away win");
		}
	}
}
