
import java.util.ArrayList;

public class Pawn extends Piece {

	public Pawn(String position, boolean color, Game game) {

		this.position = position;
		this.color = color;
		// Pionul poate avea maxim 4 mutari posibile la un moment de timp
		this.possibleMoves = new ArrayList<String>();
		this.game = game;
		this.score = 100;
	}

	public void updatePossibleMoves(boolean side, Game game) {
		
		// resetare arrayList possibleMoves
		this.possibleMoves = new ArrayList<String>();

		int row = this.getRow();
		int column = this.getColumn();
		this.game = game;
		
		if (side == false) {

			// mutare de tip en passant
			if (this.enPassant != null) {
				this.possibleMoves.add(Utils.getPosition(row, column) + enPassant);
			}

			// cucerire piesa stanga fata (negru)
			if ((row > 0) && (column > 0) && (!(this.game.getPiece(row - 1, column - 1) instanceof Empty))
					&& (this.game.getPiece(row - 1, column - 1).color == true)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 1, column - 1));
			}

			// cucerire piesa dreapta fata (negru)
			if ((row > 0) && (column < 7) && (!(this.game.getPiece(row - 1, column + 1) instanceof Empty))
					&& (this.game.getPiece(row - 1, column + 1).color == true)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 1, column + 1));
			}

			// mutare 2 pozitii in fata(negru)
			if ((row == 6) && (this.game.getPiece(row - 1, column) instanceof Empty)
					&& (this.game.getPiece(row - 2, column) instanceof Empty)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 2, column));
			}

			// mutare o pozitie in fata (negru)
			if ((row > 0) && (this.game.getPiece(row - 1, column) instanceof Empty)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 1, column));
			}
		} else {
			
			// mutare de tip en passant
			if (this.enPassant != null) {
				this.possibleMoves.add(Utils.getPosition(row, column) + enPassant);
			}
			
			// cucerire piesa stanga fata (alb)
			if ((row < 7) && (column > 0) && (!(this.game.getPiece(row + 1, column - 1) instanceof Empty))
					&& (this.game.getPiece(row + 1, column - 1).color == false)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 1, column - 1));
			}

			// cucerire piesa dreapta fata (alb)
			if ((row < 7) && (column < 7) && (!(this.game.getPiece(row + 1, column + 1) instanceof Empty))
					&& (this.game.getPiece(row + 1, column + 1).color == false)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 1, column + 1));
			}

			// mutare 2 pozitii in fata(alb)
			if ((row == 1) && (this.game.getPiece(row + 1, column) instanceof Empty)
					&& (this.game.getPiece(row + 2, column) instanceof Empty)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 2, column));
			}

			// mutare o pozitie in fata (alb)
			if ((row < 7) && (this.game.getPiece(row + 1, column) instanceof Empty)) {

				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 1, column));
			}
		}
	}
}