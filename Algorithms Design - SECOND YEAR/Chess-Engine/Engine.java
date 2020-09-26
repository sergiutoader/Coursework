
import java.util.Scanner;
import java.io.IOException;
import java.io.BufferedOutputStream;

public class Engine {
	public static void main(String args[]) throws IOException {

		// se creaza un joc nou cu engine-ul jucand cu negru
		Game game = new Game(false);

		int newGame = 1;
		int pause = 0;

		// partea pe care va juca engine-ul dupa comanda "force"
		boolean sideAfterForce = true;

		Scanner input = new Scanner(System.in);
		String prevSide = "";
		BufferedOutputStream bout = new BufferedOutputStream(System.out);

		while (true) {
			// daca s-a dat comanda 'quit', newGame = 0 si se iese din program
			// daca s-a dat comanda 'new', newGame = 1 si se intra inapoi in while
			if (newGame == 0) {
				break;
			}

			while (true) {
				String command = input.nextLine();

				if (pause == 0) {

					if (command.equals("xboard")) {
					}

					if (command.equals("protover 2")) {
						bout.write("feature sigint=0 san=0\n".getBytes());
						bout.flush();
					}

					if (command.equals("new")) {
						game = new Game(false);
						break;
					}

					if (command.equals("black")) {
						game.side = false;
					}

					if (command.equals("white")) {
						game.side = true;
					}

					if (command.equals("quit")) {
						newGame = 0;
						break;
					}

					if (command.equals("force")) {
						pause = 1;
					}

					if(command.equals("go")) {
						game.makeMove(bout);
					}

					if (command.length() == 4 || command.length() == 5) {
						if (Character.isLetter(command.charAt(0)) && Character.isLetter(command.charAt(2))
								&& Character.isDigit(command.charAt(1)) && Character.isDigit(command.charAt(3))) {
							OpponentMove.opponentMove(game, command);
							game.makeMove(bout);
						}
					}

				} else if (pause == 1) {

					// daca primeste o mutare cat timp e in pauza,
					// updateaza grid-ul
					if (command.length() == 4) {
						if (Character.isLetter(command.charAt(0)) && Character.isLetter(command.charAt(2))
								&& Character.isDigit(command.charAt(1)) && Character.isDigit(command.charAt(3))) {
							OpponentMove.opponentMove(game, command);
						}
					}

					if (command.equals("white")) {
						sideAfterForce = true;
					}

					if (command.equals("black")) {
						sideAfterForce = false;
					}

					// dupa comanda "go" se revine in joc pe partea sideAfterForce si se neaga
					// aceasta variabila pentru a intra in joc
					// pe partea opusa la urmatoarea comanda force
					if (command.equals("go")) {

						pause = 0;
						game.side = sideAfterForce;
						sideAfterForce = !sideAfterForce;
						game.makeMove(bout);

					// in cazul comenzii "new", engine-ul joaca cu negru
					} else if (command.equals("new")) {
						pause = 0;
						sideAfterForce = true;
						game = new Game(false);
						// daca se da alta comanda in afara de "go" cat timp engine-ul este in pauza,
						// (deci o mutare de la celalalt player), va trebui ca partea pe care intra in
						// joc
						// engine-ul sa fie cea de pe care a iesit
					}

				}
			}

		}
		input.close();
		bout.close();
	}
}