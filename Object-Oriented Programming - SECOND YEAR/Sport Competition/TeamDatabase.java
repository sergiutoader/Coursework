/* Toader Sergiu-Cristian, 325CB */

import java.io.PrintWriter;
import java.util.ArrayList;

/**
 * Class representing a database for teams in the competition. It contains
 * methods for adding and listing teams. It uses the Singleton Pattern (only one
 * instance for a database is required).
 *
 */
public class TeamDatabase {

	private static TeamDatabase uniqueInstance;

	private TeamDatabase() {
		teamList = new ArrayList<Team>();
	}

	public static TeamDatabase getInstance() {
		if (uniqueInstance == null) {
			uniqueInstance = new TeamDatabase();
		}
		return uniqueInstance;
	}

	private ArrayList<Team> teamList;

	public ArrayList<Team> getTeamList() {
		return teamList;
	}

	public void addTeam(Team t) {
		teamList.add(t);
	}

	public void listTeams(PrintWriter writer) {
		String outString = "";
		for (Team t : teamList) {
			String aux = String.format("{teamName: %s, gender: %s, numberOfPlayers: %d, players: [", t.getTeamName(),
					t.getGender(), t.getNumberOfPlayers());
			outString = outString + aux;
			for (Player p : t.getPlayers()) {
				String aux2 = String.format("{name: %s, score: %d}, ", p.getName(), p.getScore());
				outString = outString + aux2;
			}
			outString = outString.substring(0, outString.length() - 2);
			outString = outString + "]}\n";
		}
		writer.append(outString);
		writer.close();
	}

}
