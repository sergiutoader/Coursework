/* Toader Sergiu-Cristian, 325CB */

/**
 * Class that holds fields for a team and its points. Used to sort teams in the
 * standings by comparing their points.
 *
 */
public class PointsPerTeam implements Comparable<PointsPerTeam> {

	private Team team;
	private int points;

	public PointsPerTeam(Team team, int points) {
		this.team = team;
		this.points = points;
	}

	public Team getTeam() {
		return team;
	}

	public void setTeam(Team team) {
		this.team = team;
	}

	public int getPoints() {
		return points;
	}

	public void setPoints(int points) {
		this.points = points;
	}

	@Override
	public int compareTo(PointsPerTeam o) {
		return o.getPoints() - this.getPoints();
	}

}
