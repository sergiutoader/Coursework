
public class OpponentMove {

	// metoda folosita pentru a interpreta si inregistra pe grid o mutare a oponentului
	public static void opponentMove(Game game, String command) {
		// calculare indicii pentru pozitia anterioara
		// si curenta in functie de stringul command
		int prevRow = Utils.getRow(command.substring(0, 2));
		int prevColumn = Utils.getColumn(command.substring(0, 2));
		int currRow = Utils.getRow(command.substring(2));
		int currColumn = Utils.getColumn(command.substring(2));

		// daca comanda are dimensiune 5, oponentul a ajuns cu pionul in baza
		// engine-ului si a schimbat pionul in alta piesa
		// se inregistreaza pe gridboard aceasta modificare
		// se actualizeaza vectorii pieceCount
		if (command.length() == 5) {
			game.grid[currRow][currColumn] = null;
			if(game.side == false) game.whitePieceCount[0]--;
			else game.blackPieceCount[0]--;
			switch (command.charAt(4)) {
			case 'q':
				game.grid[currRow][currColumn] = new Queen(Utils.getPosition(
					currRow, currColumn), !(game.side), game);
				if(game.side == false) game.whitePieceCount[4]++;
				else game.blackPieceCount[4]++;
				break;
			case 'r':
				game.grid[currRow][currColumn] = new Rook(Utils.getPosition(
					currRow, currColumn), !(game.side), game);
				if(game.side == false) game.whitePieceCount[3]++;
				else game.blackPieceCount[3]++;
				break;
			case 'b':
				game.grid[currRow][currColumn] = new Bishop(Utils.getPosition(
					currRow, currColumn), !(game.side), game);
				if(game.side == false) game.whitePieceCount[2]++;
				else game.blackPieceCount[2]++;
				break;
			case 'n':
				game.grid[currRow][currColumn] = new Knight(Utils.getPosition(
					currRow, currColumn), !(game.side), game);
				if(game.side == false) game.whitePieceCount[1]++;
				else game.blackPieceCount[1]++;
				break;
			}
		} else {
			// se pune piesa de pe pozitia anterioara pe pozitia actuala
			game.updatePieceCount(game, currRow, currColumn);
			game.grid[currRow][currColumn] = game.grid[prevRow][prevColumn];
			game.grid[currRow][currColumn].position = command.substring(2);
			game.grid[currRow][currColumn].hasMoved = true;
		}
		// se actualizeaza pozitia veche cu empty
		game.grid[prevRow][prevColumn] = new Empty(Utils.getPosition(
			prevRow, prevColumn), false, game);

		engineEnPassant(game, prevRow, prevColumn, currRow, currColumn);
		opponentEnPassant(game, prevRow, prevColumn, currRow, currColumn);
		opponentCastling(game, prevRow, prevColumn, currRow, currColumn);
	}

	// verifica daca engine-ul poate efectua o mutare de tip en passant si seteaza
	// campul enPassant al pionului care poate efectua acea mutare cu pozitia pe
	// care se poate muta
	private static void engineEnPassant(Game game, int prevRow, int prevColumn,
		int currRow, int currColumn) {

		if (game.side == false && prevRow == 1 && currRow == 3) {
			if (currColumn > 0 && game.grid[currRow][currColumn - 1] instanceof Pawn
					&& game.grid[currRow][currColumn - 1].color == false) {
				game.grid[currRow][currColumn - 1].enPassant = 
					Utils.getPosition(currRow - 1, currColumn);
			}
			if (currColumn < 7 && game.grid[currRow][currColumn + 1] instanceof Pawn
					&& game.grid[currRow][currColumn + 1].color == false) {
				game.grid[currRow][currColumn + 1].enPassant = 
					Utils.getPosition(currRow - 1, currColumn);
			}
		} else if (game.side == true && prevRow == 6 && currRow == 4) {
			if (currColumn > 0 && game.grid[currRow][currColumn - 1] instanceof Pawn
					&& game.grid[currRow][currColumn - 1].color == true) {
				game.grid[currRow][currColumn - 1].enPassant = 
					Utils.getPosition(currRow + 1, currColumn);
			}
			if (currColumn < 7 && game.grid[currRow][currColumn + 1] instanceof Pawn
					&& game.grid[currRow][currColumn + 1].color == true) {
				game.grid[currRow][currColumn + 1].enPassant = 
					Utils.getPosition(currRow + 1, currColumn);
			}
		}
	}

	// verifica daca oponentul a efectuat o mutare de tip en passant si actualizeaza
	// grid-ul
	private static void opponentEnPassant(Game game, int prevRow, int prevColumn,
		int currRow, int currColumn) {

		if (game.grid[currRow][currColumn] instanceof Pawn) {
			// conditii (negru): linia curenta 5, mutare pion alb pe diagonala stanga sau
			// dreapta, pion negru aflat cu o pozitie in spate fata de pozitia curenta
			if (game.side == false) {
				if (prevRow == 4 && currRow == 5) {
					if (prevColumn == currColumn + 1) {
						if ((game.grid[currRow - 1][currColumn] instanceof Pawn)
								&& (game.grid[currRow - 1][currColumn].color == false)) {
							game.grid[currRow - 1][currColumn] = new Empty(
								Utils.getPosition(currRow - 1, currColumn), false, game);
						}
					} else if (prevColumn == currColumn - 1) {
						if ((game.grid[currRow - 1][currColumn] instanceof Pawn)
								&& (game.grid[currRow - 1][currColumn].color == false)) {
							game.grid[currRow - 1][currColumn] = new Empty(
								Utils.getPosition(currRow - 1, currColumn), false, game);
						}
					}
				}
			}
			// conditii (alb): linia curenta 2, mutare pion negru pe diagonala stanga sau
			// dreapta, pion alb aflat cu o pozitie in spate fata de pozitia curenta
			else {
				if (prevRow == 3 && currRow == 2) {
					if (prevColumn == currColumn + 1) {
						if ((game.grid[currRow + 1][currColumn] instanceof Pawn)
								&& (game.grid[currRow + 1][currColumn].color == true)) {
							game.grid[currRow + 1][currColumn] = new Empty(Utils.getPosition(
								currRow + 1, currColumn), false, game);
						}
					} else if (prevColumn == currColumn - 1) {
						if ((game.grid[currRow + 1][currColumn] instanceof Pawn)
								&& (game.grid[currRow + 1][currColumn].color == true)) {
							game.grid[currRow + 1][currColumn] = new Empty(Utils.getPosition(
								currRow + 1, currColumn), false, game);
						}
					}
				}
			}
		}
	}

	// verifica daca oponentul a efectuat o mutare de tip rocada si actualizeaza
	// grid-ul
	private static void opponentCastling(Game game, int prevRow, int prevColumn,
		int currRow, int currColumn) {
		if (game.grid[currRow][currColumn] instanceof King) {
			// rocada de 2
			if (currRow == prevRow && currColumn == prevColumn + 2) {
				game.grid[currRow][currColumn - 1] = game.grid[currRow][7];
				game.grid[currRow][currColumn - 1].position = Utils.getPosition(
					currRow, currColumn - 1);

				game.grid[currRow][7] = new Empty(Utils.getPosition(
					currRow, 7), false, game);
			}
			// rocada de 3
			else if (currRow == prevRow && currColumn == prevColumn - 2) {
				game.grid[currRow][currColumn + 1] = game.grid[currRow][0];
				game.grid[currRow][currColumn + 1].position =
					Utils.getPosition(currRow, currColumn + 1);

				game.grid[currRow][0] = new Empty(
					Utils.getPosition(currRow, 0), false, game);
			}
		}

	}
}