/* Toader Sergiu-Cristian, 325CB */

import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Collections;

/**
 * Subject class for the Observer Pattern. Contains methods to register, remove
 * and notify observers (the teams attending the competition) and other methods
 * used for the simulation of the competition
 *
 */
public class Competition implements Subject {

	private String type;
	private String gender;
	private PrintWriter writer;

	private ArrayList<Team> teams;
	private ArrayList<Integer> points;

	/**
	 * ArrayList containing the points obtained by each team during the competition
	 */
	private ArrayList<PointsPerTeam> pointsPerTeam;

	public Competition(String type, String gender, PrintWriter writer) {
		this.type = type;
		this.gender = gender;
		this.writer = writer;

		this.teams = new ArrayList<Team>();
		this.points = new ArrayList<Integer>();

		this.pointsPerTeam = new ArrayList<PointsPerTeam>(teams.size());
	}

	/**
	 * Method that simulates all games in the competition. After that, the teams are
	 * notified with their position in the competition standings
	 */
	public void simulateCompetition() {
		for (int i = 0; i < teams.size() - 1; i++) {
			for (int j = i + 1; j < teams.size(); j++) {
				teams.get(i).visit(teams.get(j));
			}
		}
		notifyObservers();
	}

	@Override
	public void registerObserver(Observer o) {
		if (this.type.equals(getType((Team) o)) && this.gender.equals(((Team) o).getGender())) {
			teams.add((Team) o);
			points.add(0);
		}
	}

	@Override
	public void removeObserver(Observer o) {
		teams.remove((Team) o);
	}

	/**
	 * Teams and their points are added into the pointsPerTeam ArrayList, sorted and
	 * then the names of the first 3 teams are printed out. All teams are notified
	 * with their position in the competition standings.
	 */
	@Override
	public void notifyObservers() {
		for (int i = 0; i < teams.size(); i++) {
			pointsPerTeam.add(new PointsPerTeam(teams.get(i), points.get(i)));
		}

		Collections.sort(pointsPerTeam);
		for (int i = 0; i < 3; i++) {
			writer.println(pointsPerTeam.get(i).getTeam().getTeamName());
		}

		for (int i = 0; i < teams.size(); i++) {
			// first parameter is where the team finished in the competition
			teams.get(i).update(getPlaceForTeam(teams.get(i).getTeamName()), writer);
		}
	}

	/**
	 * 
	 * @param homeName			Home team name
	 * @param awayName			Away team name
	 * @param result 			Draw, home win or away win
	 */
	public void updatePoints(String homeName, String awayName, String result) {
		int homeIdx = -1;
		int awayIdx = -1;

		// locate home and away team in teams and keep track of their index value
		for (int i = 0; i < teams.size(); i++) {
			if (teams.get(i).getTeamName().equals(homeName)) {
				homeIdx = i;
			}
			if (teams.get(i).getTeamName().equals(awayName)) {
				awayIdx = i;
			}
		}

		if (result.equals("home win")) { // update points
			points.set(homeIdx, points.get(homeIdx) + 3);
		} else if (result.equals("draw")) {
			points.set(homeIdx, points.get(homeIdx) + 1);
			points.set(awayIdx, points.get(awayIdx) + 1);
		} else {
			points.set(awayIdx, points.get(awayIdx) + 3);
		}
	}

	/**
	 * 
	 * @param team 		The team
	 * @return 			A String corresponding to the team type
	 */
	private String getType(Team team) {
		if (team instanceof FootballTeam) {
			return "football";
		} else if (team instanceof BasketballTeam) {
			return "basketball";
		} else {
			return "handball";
		}
	}

	/**
	 * @param name 		The name of the team
	 * @return 			The team's place in the standings
	 */
	private int getPlaceForTeam(String name) {
		for (int i = 0; i < pointsPerTeam.size(); i++) {
			if (pointsPerTeam.get(i).getTeam().getTeamName().equals(name)) {
				return i + 1;
			}
		}
		return -1;
	}
}
