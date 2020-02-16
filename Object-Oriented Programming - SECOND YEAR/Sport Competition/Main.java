/* Toader Sergiu-Cristian, 325CB */

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.util.Scanner;

/**
 * Class containing main method. Used to instantiate input and output objects,
 * Database and Competition objects, scan input data and decide whether to run
 * first or second task. Used to list Teams for first task and simulate the
 * competition for the second. It uses the Factory Design Pattern to create
 * teams before being added to the database.
 *
 */
public class Main {

	public static void main(String[] args) {

		File inFileTeams = new File(args[1]);
		File inFileCompetition = null;
		File outFile = new File(args[3]);

		Scanner inputTeams = null;
		Scanner inputCompetition = null;
		PrintWriter writer = null;

		try {
			inputTeams = new Scanner(inFileTeams);
			writer = new PrintWriter(outFile);
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		if (args[0].equals("inscriere")) { // Part 1 (team entry)
			TeamDatabase database = TeamDatabase.getInstance();
			TeamFactory factory = TeamFactory.getInstance();

			while (inputTeams.hasNextLine()) {

				String[] lineParams = inputTeams.nextLine().split(",");
				for (int i = 0; i < lineParams.length; i++) {
					lineParams[i] = lineParams[i].trim();
				}
				String teamType = lineParams[0];
				String teamName = lineParams[1];
				String gender = lineParams[2];
				int numberOfPlayers = Integer.parseInt(lineParams[3]);
				// Create team using team factory
				Team team = factory.createTeam(teamType, teamName, gender, numberOfPlayers);
				for (int i = 0; i < numberOfPlayers; i++) {
					String[] playerLine = inputTeams.nextLine().split(",");
					String playerName = playerLine[0];
					int playerScore = Integer.parseInt(playerLine[1].trim());
					// Add player to team
					team.addPlayer(playerName, playerScore);
				}
				// Add team to database
				database.addTeam(team);
			}
			// List teams
			database.listTeams(writer);
			inputTeams.close();
		} else if (args[0].equals("competitie")) { // Task 2 (competition)
			inFileCompetition = new File(args[2]);

			try {
				inputCompetition = new Scanner(inFileCompetition);
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}

			TeamDatabase database = TeamDatabase.getInstance();
			TeamFactory factory = TeamFactory.getInstance();

			while (inputTeams.hasNextLine()) {

				String[] lineParams = inputTeams.nextLine().split(",");
				for (int i = 0; i < lineParams.length; i++) {
					lineParams[i] = lineParams[i].trim();
				}
				String teamType = lineParams[0];
				String teamName = lineParams[1];
				String gender = lineParams[2];
				int numberOfPlayers = Integer.parseInt(lineParams[3]);
				// Create team using team factory
				Team team = factory.createTeam(teamType, teamName, gender, numberOfPlayers);
				for (int i = 0; i < numberOfPlayers; i++) {
					String[] playerLine = inputTeams.nextLine().split(",");
					String playerName = playerLine[0];
					int playerScore = Integer.parseInt(playerLine[1].trim());
					// Add player to team
					team.addPlayer(playerName, playerScore);
				}
				// Add player to database
				database.addTeam(team);
			}

			String[] competitionLine = inputCompetition.nextLine().split(",");
			String competitionType = competitionLine[0].trim();
			String competitionGender = competitionLine[1].trim();

			Competition competition = new Competition(competitionType, competitionGender, writer);

			while (inputCompetition.hasNext()) {
				String teamName = inputCompetition.next();

				for (Team team : database.getTeamList()) {
					if (team.getTeamName().equals(teamName)) {
						// Register team as observer for the competition
						team.assignToCompetition(competition);
						break;
					}
				}
			}
			// Simulate competition
			competition.simulateCompetition();
			
			inputTeams.close();
			inputCompetition.close();
			writer.close();
		}

	}
}