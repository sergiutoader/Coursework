import java.util.ArrayList;
import java.io.BufferedOutputStream;
import java.io.IOException;


class Pair<F, S> {
    public F first;
    public S second;

    public Pair(F first, S second) {
        this.first = first;
        this.second = second;
    }
}

public class Game {
	public Piece[][] grid;

	public boolean side; // true = white, false = black;
	public ArrayList<String> allMoves;

	public int[] whitePieceCount;
	public int[] blackPieceCount;

	// variabila care retine daca engine-ul a efectuat rocadas
	public boolean castling;
	// variabila care retine numarul de mutari care au fost
	// efectuate in jocul curent (de un jucator)
	public int moveCount;

	// getter pentru o piesa
	public Piece getPiece(int row, int column) {
		return this.grid[row][column];
	}

	public Game(boolean side) {
		this.side = side;
		this.grid = new Piece[8][8];

		this.castling = false;
		this.moveCount = 0;

		// Spatii goale
		for (int i = 2; i < 6; i++) {
			for (int j = 0; j <= 7; j++) {
				grid[i][j] = new Empty(Utils.getPosition(i, j), false, this);
			}
		}
		// Pioni
		for (int j = 0; j <= 7; j++) {
			grid[1][j] = new Pawn(Utils.getPosition(1, j), true, this);
			grid[6][j] = new Pawn(Utils.getPosition(6, j), false, this);
		}

		// Cai
		grid[0][1] = new Knight(Utils.getPosition(0, 1), true, this);
		grid[0][6] = new Knight(Utils.getPosition(0, 6), true, this);
		grid[7][1] = new Knight(Utils.getPosition(7, 1), false, this);
		grid[7][6] = new Knight(Utils.getPosition(7, 6), false, this);

		// Nebuni
		grid[0][2] = new Bishop(Utils.getPosition(0, 2), true, this);
		grid[0][5] = new Bishop(Utils.getPosition(0, 5), true, this);
		grid[7][2] = new Bishop(Utils.getPosition(7, 2), false, this);
		grid[7][5] = new Bishop(Utils.getPosition(7, 5), false, this);

		// Ture
		grid[0][0] = new Rook(Utils.getPosition(0, 0), true, this);
		grid[0][7] = new Rook(Utils.getPosition(0, 7), true, this);
		grid[7][0] = new Rook(Utils.getPosition(7, 0), false, this);
		grid[7][7] = new Rook(Utils.getPosition(7, 7), false, this);

		// Regine
		grid[0][3] = new Queen(Utils.getPosition(0, 3), true, this);
		grid[7][3] = new Queen(Utils.getPosition(7, 3), false, this);

		// Regii
		grid[0][4] = new King(Utils.getPosition(0, 4), true, this);
		grid[7][4] = new King(Utils.getPosition(7, 4), false, this);

		/*
			pion - 0 | cal - 1 | nebun - 2 | tura - 3 | regina - 4 | rege - 5
		*/
		whitePieceCount = new int[6];
		blackPieceCount = new int[6];

		whitePieceCount[0] = 8;
		whitePieceCount[1] = 2;
		whitePieceCount[2] = 2;
		whitePieceCount[3] = 2;
		whitePieceCount[4] = 1;
		whitePieceCount[5] = 1;

		blackPieceCount[0] = 8;
		blackPieceCount[1] = 2;
		blackPieceCount[2] = 2;
		blackPieceCount[3] = 2;
		blackPieceCount[4] = 1;
		blackPieceCount[5] = 1;

	}

	public Game(Boolean side, Piece[][] grid, int[] whitePieceCount,
		int[] blackPieceCount, boolean castling, int moveCount) {
		this.side = side;
		this.grid = grid;
		this.castling = castling;
		this.whitePieceCount = new int[6];
		this.blackPieceCount = new int[6];
		this.moveCount = moveCount;

		for(int i = 0; i < 6; i++) {
			this.whitePieceCount[i] = whitePieceCount[i];
			this.blackPieceCount[i] = blackPieceCount[i];
		}
	}

	// intoarce o clona a piesei p
	public Piece clonePiece(Piece p) {
		if(p instanceof Empty) {
			return new Empty(p.position, p.color, p.game);
		}
		if(p instanceof Pawn) {
			return new Pawn(p.position, p.color, p.game);
		}
		if(p instanceof Knight) {
			return new Knight(p.position, p.color, p.game);
		}
		if(p instanceof Bishop) {
			return new Bishop(p.position, p.color, p.game);
		}
		if(p instanceof Rook) {
			return new Rook(p.position, p.color, p.game, p.hasMoved);
		}
		if(p instanceof Queen) {
			return new Queen(p.position, p.color, p.game);
		}
		if(p instanceof King) {
			return new King(p.position, p.color, p.game, p.hasMoved);
		}
		return new Empty(p.position, p.color, p.game);
	}

	// intoarce o clona a grid-ului
	public Piece[][] cloneGrid(Piece [][] grid) {
		Piece[][] newGrid = new Piece[8][8];
		for(int i = 0; i <= 7; i++) {
			for(int j = 0; j <= 7; j++) {
				newGrid[i][j] = clonePiece(grid[i][j]);
			}
		}
		return newGrid;
	}

	// creaza o copie a jocului cu culoarea opusa celei curente
	public Game cloneGame(Game game) {
		return new Game(game.side, game.cloneGrid(game.grid),
			game.whitePieceCount, game.blackPieceCount, game.castling, game.moveCount);
	}

	// intorce o lista de mutari complete (care contin atat sursa cat si destinatia)
	// se apeleaza metoda possibleMoves pentru fiecare piesa a engine-ului
	// se adauga toate mutarile legale in lista allMoves
	public void updateAllPossibleMoves(Game game) {
		int i, j;
		game.allMoves = new ArrayList<String>();
		for(i = 0; i <= 7; i++) {
			for(j = 0; j <= 7; j++) {
				if (!(game.grid[i][j] instanceof Empty) && (game.grid[i][j].color == game.side)) {
					game.grid[i][j].updatePossibleMoves(game.side, game);
					for(String s : game.grid[i][j].possibleMoves) {
						game.allMoves.add(s);
					}
				}
			}
		}
	}

	// Algoritm negamax cu taieturi alfa-beta
	public Pair <Integer, String> minimax(Game game, int depth, int alfa, int beta) {
		// daca adancimea este 0, sau jocul s-a terminat, se intoarce evaluarea
		// curenta a tablei de sah
		if(depth == 0 || Eval.ended(game)) {
			return new Pair<Integer, String>(Eval.eval(game), "");
		}

		// se actualizeaza toate mutarile jucatorului curent
		game.updateAllPossibleMoves(game);		

		// daca nu exista mutari, se intoarce -Infinit ca scor
		if(game.allMoves.isEmpty()) {
			return new Pair<Integer, String>(-Utils.Infinity, "");
		}

		int bestScore = -Utils.Infinity;
		String bestMove = "";
		// se aplica toate mutarile pe rand si se calculeaza mutarea cea mai buna
		for(String move : game.allMoves) {
			Game copy = game.cloneGame(game);
			copy.applyMove(copy, move);
			copy.side = !(copy.side);
			// apel recursiv negamax
			int score = -minimax(copy, depth - 1, -beta, -alfa).first;

			if(score > bestScore) {
				bestScore = score;
				bestMove = move; 
			}

			if(bestScore > alfa) {
				alfa = bestScore;
			}
			if(alfa >= beta) {
				break;
			}
		}
		
		return new Pair <Integer, String> (alfa, bestMove);
	}

	// metoda care actualizeaza grid-ul in urma unei mutari
	public void applyMove(Game game, String move) {

		int i = Utils.getRow(move.substring(0, 2));
		int j = Utils.getColumn(move.substring(0, 2));
		String nextPosition = move.substring(2);
		int nextRow = Utils.getRow(nextPosition);
		int nextColumn = Utils.getColumn(nextPosition);

		// se actualizeaza vectorii de pieceCount
		updatePieceCount(game, nextRow, nextColumn);

		// se aplica mutari speciale pentru pion /
		// rege (daca este cazul) sau o mutare obisnuita 
		if (game.grid[i][j] instanceof Pawn && game.grid[i][j].color == game.side) {
			if(game.side == false) {
				game.pawnBlackMove(game, move);
					
			} else {
				game.pawnWhiteMove(game, move);
			}
		} else if(game.grid[i][j] instanceof King && game.grid[i][j].color == game.side) {
			game.kingMove(game, move);
		} else {
			// actualizare grid + Empty pe pozitia veche
			game.grid[nextRow][nextColumn] = game.grid[i][j];	
			game.grid[nextRow][nextColumn].position = Utils.getPosition(nextRow, nextColumn);

			game.grid[i][j] = new Empty(Utils.getPosition(i, j), false, game);
		}	

	}

	// Functie care trimite o comanda la xboard in functie de ce intoarce algoritmul minimax
	public void makeMove(BufferedOutputStream bout) throws IOException {
		Pair <Integer, String> pair = minimax(this, 5, -Utils.Infinity, Utils.Infinity);

		String move = pair.second;

		if(move.equals("")) {
			bout.write(String.format("resign\n").getBytes());
			bout.flush();
			return;
		}
		
		this.applyMove(this, move);
		
		// piesa a fost mutata
		String nextPosition = move.substring(2);
		int nextRow = Utils.getRow(nextPosition);
		int nextColumn = Utils.getColumn(nextPosition);
		this.grid[nextRow][nextColumn].hasMoved = true;

		// daca a dat o mutare ilegala, dau resign
		// asta se intampla in cazul in care este pus in sah, iar singuele
		// mutari legale pe care le poate face va da posibilitatea oponentului sa dea sah mat
		// (practic toate evaluarile din minimax conduc la un joc pierdut) 
		if(this.isCheck(this)) {
			bout.write(String.format("resign\n").getBytes());
			bout.flush();
			return;
		}

		// se incrementeaza numarul de mutari
		this.moveCount++;

		// afisare mutare
		bout.write(String.format("move " + move + "\n").getBytes());
		bout.flush();
	}

	// metoda folosita in cazul in care engine-ul joaca pe alb si are mutare legala pentru un pion
	private void pawnWhiteMove(Game game, String move) {

		int i = Utils.getRow(move.substring(0, 2));
		int j = Utils.getColumn(move.substring(0, 2));

		String nextPosition = move.substring(2);
		int nextRow = Utils.getRow(nextPosition);
		int nextColumn = Utils.getColumn(nextPosition);

		// daca este o mutare de tip en passant, se elimina pionul din spatele
		// destinatiei
		if (game.grid[i][j].enPassant != null) {
			int row = Utils.getRow(nextPosition) - 1;
			int column = Utils.getColumn(nextPosition);
			game.grid[row][column] = new Empty(Utils.getPosition(row, column), false, game);
			// se reseteaza campul pentru pionul mutat
			game.grid[i][j].enPassant = null;
		}

		// daca pionul ajunge in baza oponentului, se transforma in regina
		if (game.grid[i][j] instanceof Pawn && nextRow == 7) {
			game.grid[nextRow][nextColumn] = new Queen(
					nextPosition, true, game);
			// se actualizeaza vectorii pieceCount
			game.whitePieceCount[0]--;
			game.whitePieceCount[4]++;
		} else {
			// actualizare grid
			game.grid[nextRow][nextColumn] = game.grid[i][j];
			// actualizare camp pentru pozitie
			game.grid[nextRow][nextColumn].position = nextPosition;
		}

		// setare camp gol pe pozitia anterioara
		game.grid[i][j] = new Empty(Utils.getPosition(i, j), false, game);

	}

		// metoda folosita in cazul in care engine-ul joaca pe alb si are mutare legala pentru un pion
	private void pawnBlackMove(Game game, String move) {
		
		int i = Utils.getRow(move.substring(0, 2));
		int j = Utils.getColumn(move.substring(0, 2));

		String nextPosition = move.substring(2);
		int nextRow = Utils.getRow(nextPosition);
		int nextColumn = Utils.getColumn(nextPosition);

		// daca este o mutare de tip en passant, se elimina pionul din spatele
		// destinatiei
		if (game.grid[i][j].enPassant != null) {
			int row = Utils.getRow(nextPosition) + 1;
			int column = Utils.getColumn(nextPosition);
			game.grid[row][column] = new Empty(Utils.getPosition(row, column), false, game);
			// se reseteaza campul pentru pionul mutat
			game.grid[i][j].enPassant = null;
		}

		// daca pionul ajunge in baza oponentului, se transforma in regina
		if (Utils.getRow(nextPosition) == 0) {
			game.grid[nextRow][nextColumn] = new Queen(nextPosition, false, game);
			// se actualizeaza vectorii pieceCount
			game.whitePieceCount[0]--;
			game.whitePieceCount[4]++;
		} else {

			// actualizare grid
			game.grid[nextRow][nextColumn] = game.grid[i][j];
			// actualizare camp pentru pozitie
			game.grid[nextRow][nextColumn].position = nextPosition;
		}
		// setare camp gol pe pozitia anterioara
		game.grid[i][j] = new Empty(Utils.getPosition(i, j), false, game);
	}

	// metoda care realizeaza mutarea regelui
	public void kingMove(Game game, String move) {
		String nextPosition = move.substring(2);
		int nextRow = Utils.getRow(nextPosition);
		int nextColumn = Utils.getColumn(nextPosition);

		int i = Utils.getRow(move.substring(0, 2));
		int j = Utils.getColumn(move.substring(0, 2));

		if(game.side == true) {
			// rocada alb dreapta
			if(j == 4 && nextColumn == 6) {
				
				// mutare tura 
				game.grid[0][5] = game.grid[0][7];
				game.grid[0][5].position = Utils.getPosition(0, 5);
				game.grid[0][7] = new Empty(Utils.getPosition(0, 7), false, game);
				game.castling = true;
			}
			// rocada alb stanga
			else if(j == 4 && nextColumn == 2) {

				// mutare tura 
				game.grid[0][3] = game.grid[0][0];
				game.grid[0][3].position = Utils.getPosition(0, 3);
				game.grid[0][0] = new Empty(Utils.getPosition(0, 0), false, game);
				game.castling = true;
			}

		} else {
			// rocada negru dreapta
			if(j == 4 && nextColumn == 6) {
				
				// mutare tura 
				game.grid[7][5] = game.grid[7][7];
				game.grid[7][5].position = Utils.getPosition(7, 5);
				game.grid[7][7] = new Empty(Utils.getPosition(7, 7), false, game);
				game.castling = true;
			}
			// rocada negru stanga
			else if(j == 4 && nextColumn == 2) {

				// mutare tura 
				game.grid[7][3] = game.grid[7][0];
				game.grid[7][3].position = Utils.getPosition(7, 3);
				game.grid[7][0] = new Empty(Utils.getPosition(7, 0), false, game);
				game.castling = true;
			}
		}
		// actualizare grid
		game.grid[nextRow][nextColumn] = game.grid[i][j];
		game.grid[nextRow][nextColumn].position = nextPosition;
		// setare camp gol pe pozitia anterioara
		game.grid[i][j] = new Empty(Utils.getPosition(i, j), false, game);
	}


	// Functie care updateaza vectorii whitePieceCount si blackPieceCount
	public void updatePieceCount(Game game, int i, int j) {
		if(game.grid[i][j] instanceof Empty) {
			return;
		}
		if(game.grid[i][j] instanceof Pawn) {
			if(game.grid[i][j].color == true) {
				game.whitePieceCount[0]--;
			} else {
				game.blackPieceCount[0]--;
			}
		}
		if(game.grid[i][j] instanceof Knight) {
			if(game.grid[i][j].color == true) {
				game.whitePieceCount[1]--;
			} else {
				game.blackPieceCount[1]--;
			}
		}
		if(game.grid[i][j] instanceof Bishop) {
			if(game.grid[i][j].color == true) {
				game.whitePieceCount[2]--;
			} else {
				game.blackPieceCount[2]--;
			}
		}
		if(game.grid[i][j] instanceof Rook) {
			if(game.grid[i][j].color == true) {
				game.whitePieceCount[3]--;
			} else {
				game.blackPieceCount[3]--;
			}
		}
		if(game.grid[i][j] instanceof Queen) {
			if(game.grid[i][j].color == true) {
				game.whitePieceCount[4]--;
			} else {
				game.blackPieceCount[4]--;
			}
		}
		if(game.grid[i][j] instanceof King) {
			if(game.grid[i][j].color == true) {
				game.whitePieceCount[5]--;
			} else {
				game.blackPieceCount[5]--;
			}
		}
	}

	// metoda care intoarce pozitia pe care se afla regele engine-ului
	public String getKingPosition(Game game) {
		String kingPosition = "";
		int i, j, k;
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 8; j++) {
				if(game.grid[i][j] instanceof King && game.grid[i][j].color == game.side) {
					kingPosition = Utils.getPosition(i, j);
					return kingPosition;
				}
			}
		}
		return null;
	}

	// verifica daca regele engine-ului se afla in sah
	public boolean isCheck(Game game) {

		String kingPosition = game.getKingPosition(game);
		int i, j, k;

		if(kingPosition == null) {
			return true;
		}

		Game copy = game.cloneGame(game);
		copy.side = !(copy.side);

		copy.updateAllPossibleMoves(copy);
		for(k = 0; k < copy.allMoves.size(); k++) {
			if(copy.allMoves.get(k) != null && copy.allMoves.get(k).indexOf(kingPosition) >= 0) {
				return true;
			}
		}
		return false;
	}

}