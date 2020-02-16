/* Toader Sergiu-Cristian, 325CB */

/**
 * Class used for the Factory Design Pattern. Used to create teams. It also uses
 * the Singleton Pattern (only one instance for TeamFactory is required).
 *
 */
public class TeamFactory {

	private static TeamFactory uniqueInstance;

	private TeamFactory() {
	}

	public static TeamFactory getInstance() {
		if (uniqueInstance == null) {
			uniqueInstance = new TeamFactory();
		}
		return uniqueInstance;
	}

	public Team createTeam(String type, String name, String gender, int numberOfPlayers) {
		Team team = null;
		if (type.equals("football")) {
			team = new FootballTeam(name, gender, numberOfPlayers);
		} else if (type.equals("basketball")) {
			team = new BasketballTeam(name, gender, numberOfPlayers);
		} else if (type.equals("handball")) {
			team = new HandballTeam(name, gender, numberOfPlayers);
		}
		return team;
	}

}
