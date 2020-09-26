import java.io.FileNotFoundException;
import java.io.PrintWriter;
import java.io.File;

public class Utils {

	public static final int Infinity = 100000;

	// metoda folosita pentru debug - afiseaza in fisierul grid.txt grid-ul
	// daca fisierul e deschis cu sublime, se va actualiza dupa fiecare mutare
	// a engine-ului
	public static void showGrid(Game game) {
		StringBuilder gridb = new StringBuilder();
		for (int i = 7; i >= 0; i--) {
			for (int j = 0; j < 8; j++) {
				if (game.grid[i][j] instanceof Empty) {
					gridb.append("0  ");
				}

				else if (game.grid[i][j].color == false) {
					if (game.grid[i][j] instanceof Pawn) {
						gridb.append("p  ");
					}
					if (game.grid[i][j] instanceof Bishop) {
						gridb.append("b  ");
					}
					if (game.grid[i][j] instanceof Knight) {
						gridb.append("kn ");
					}
					if (game.grid[i][j] instanceof King) {
						gridb.append("ki ");
					}
					if (game.grid[i][j] instanceof Queen) {
						gridb.append("q  ");
					}
					if (game.grid[i][j] instanceof Rook) {
						gridb.append("r  ");
					}

				} else {
					if (game.grid[i][j] instanceof Pawn) {
						gridb.append("P  ");
					}
					if (game.grid[i][j] instanceof Bishop) {
						gridb.append("B  ");
					}
					if (game.grid[i][j] instanceof Knight) {
						gridb.append("KN ");
					}
					if (game.grid[i][j] instanceof King) {
						gridb.append("KI ");
					}
					if (game.grid[i][j] instanceof Queen) {
						gridb.append("Q  ");
					}
					if (game.grid[i][j] instanceof Rook) {
						gridb.append("R  ");
					}

				}

				if (j == 7) {
					gridb.append("\n");
				}
			}
		}

		System.out.println(gridb.toString() + "\n");

	}

	public static void printGrid(Game game) throws FileNotFoundException {
		StringBuilder gridb = new StringBuilder();
		PrintWriter wr = new PrintWriter(new File("grid.txt"));
		for (int i = 7; i >= 0; i--) {
			for (int j = 0; j < 8; j++) {
				if (game.grid[i][j] instanceof Empty) {
					gridb.append("0  ");
				}

				else if (game.grid[i][j].color == false) {
					if (game.grid[i][j] instanceof Pawn) {
						gridb.append("p  ");
					}
					if (game.grid[i][j] instanceof Bishop) {
						gridb.append("b  ");
					}
					if (game.grid[i][j] instanceof Knight) {
						gridb.append("kn ");
					}
					if (game.grid[i][j] instanceof King) {
						gridb.append("ki ");
					}
					if (game.grid[i][j] instanceof Queen) {
						gridb.append("q  ");
					}
					if (game.grid[i][j] instanceof Rook) {
						gridb.append("r  ");
					}

				} else {
					if (game.grid[i][j] instanceof Pawn) {
						gridb.append("P  ");
					}
					if (game.grid[i][j] instanceof Bishop) {
						gridb.append("B  ");
					}
					if (game.grid[i][j] instanceof Knight) {
						gridb.append("KN ");
					}
					if (game.grid[i][j] instanceof King) {
						gridb.append("KI ");
					}
					if (game.grid[i][j] instanceof Queen) {
						gridb.append("Q  ");
					}
					if (game.grid[i][j] instanceof Rook) {
						gridb.append("R  ");
					}

				}

				if (j == 7) {
					gridb.append("\n");
				}
			}
		}
		wr.append(gridb.toString() + "\n");
		wr.close();
	}

	// Alte functii ajutatoare
	// intoarce pozitia sub forma de string
	public static String getPosition(int row, int column) {
		StringBuilder result = new StringBuilder();
		result.append((char) ('a' + column));
		result.append(row + 1);
		return result.toString();
	}

	// metode care intorc indicele si coloana in functie de string

	public static int getRow(String position) {
		return position.charAt(1) - '0' - 1;
	}

	public static int getColumn(String position) {
		return position.charAt(0) - 'a';
	}

	// vad daca in spatiul cautat se afla cumva vreo piesa de-a mea
	public static boolean freeSpace(int row, int column, Game game) {
		if (game.grid[row][column].color == game.side &&
			(game.grid[row][column] instanceof Empty == false)) {
			return false;
		}
		return true;
	}
}