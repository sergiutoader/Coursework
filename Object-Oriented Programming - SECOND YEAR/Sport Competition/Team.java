/* Toader Sergiu-Cristian, 325CB */

import java.io.PrintWriter;
import java.util.ArrayList;

/**
 * Class is used as an Observer for the Observer Design Pattern (Teams play the
 * role of observers in the competition and are being notified at the end of
 * it). The class is also used in the Visitor Pattern, where it plays the role
 * of both Visitor (when it is the home team) and Client (when it is the away
 * team). The visit method is overridden in the subclasses and it represents a
 * "game" between 2 teams.
 *
 */
public abstract class Team implements Observer {

	private String teamName;
	private String gender;
	private int numberOfPlayers;
	private ArrayList<Player> players;
	private ScoreStrategy strategy;

	protected Competition competition;

	public Team(String teamName, String gender, int numberOfPlayers) { // constructor for first task
		this.teamName = teamName;
		this.gender = gender;
		this.numberOfPlayers = numberOfPlayers;
		players = new ArrayList<Player>(numberOfPlayers);
	}

	/**
	 * Register this team as observer for competition
	 * 
	 * @param competition The competition the team attends
	 */
	public void assignToCompetition(Competition competition) {
		this.competition = competition;
		competition.registerObserver(this);
	}

	/**
	 * Method used to accept visitor
	 * 
	 * @param visitor
	 */
	public void accept(TeamVisitor visitor) {
		visitor.visit(this);
	}

	public void update(int position, PrintWriter writer) {
		writer.println("Echipa " + getTeamName() + " a ocupat locul " + position);
	}

	/**
	 * Method is overridden in subclasses
	 * 
	 * @param team The team this current team visits
	 */
	public abstract void visit(Team team);

	public void addPlayer(String name, int score) {
		players.add(new Player(name, score));
	}

	/* Getters and setters */

	public String getTeamName() {
		return teamName;
	}

	public void setTeamName(String teamName) {
		this.teamName = teamName;
	}

	public String getGender() {
		return gender;
	}

	public void setGender(String gender) {
		this.gender = gender;
	}

	public int getNumberOfPlayers() {
		return numberOfPlayers;
	}

	public void setNumberOfPlayers(int numberOfPlayers) {
		this.numberOfPlayers = numberOfPlayers;
	}

	public ArrayList<Player> getPlayers() {
		return players;
	}

	public void setPlayers(ArrayList<Player> players) {
		this.players = players;
	}

	public ScoreStrategy getStrategy() {
		return strategy;
	}

	public void setStrategy(ScoreStrategy strategy) {
		this.strategy = strategy;
	}

	public int getMaximumScore() {
		int max = getPlayers().get(0).getScore();
		for (Player p : getPlayers()) {
			if (p.getScore() > max) {
				max = p.getScore();
			}
		}
		return max;
	}

	public int getMinimumScore() {
		int min = getPlayers().get(0).getScore();
		for (Player p : getPlayers()) {
			if (p.getScore() < min) {
				min = p.getScore();
			}
		}
		return min;
	}
}