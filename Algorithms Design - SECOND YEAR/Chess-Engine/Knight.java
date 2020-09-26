
import java.util.ArrayList;

public class Knight extends Piece {

	public Knight(String position, boolean color, Game game) {

		this.position = position;
		this.color = color;
		this.possibleMoves = new ArrayList<String>();
		this.game = game;
		this.score = 300;
	}

	public void updatePossibleMoves(boolean side, Game game) {

		// resetare arrayList possibleMoves
		this.possibleMoves = new ArrayList<String>();

		int row = this.getRow();
		int column = this.getColumn();
		this.game = game;

		// 2 sus, 1 stanga
		if ((row + 2 < 8) && (column - 1 >= 0) && (((this.game.getPiece(row + 2, column - 1).color != side))
				|| (this.game.getPiece(row + 2, column - 1) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 2, column - 1));
		}
		// 2 sus, 1 dreapta
		if ((row + 2 < 8) && (column + 1 < 8) && (((this.game.getPiece(row + 2, column + 1).color != side))
				|| (this.game.getPiece(row + 2, column + 1) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 2, column + 1));
		}
		// 2 jos, 1 stanga
		if ((row - 2 >= 0) && (column - 1 >= 0) && (((this.game.getPiece(row - 2, column - 1).color != side))
				|| (this.game.getPiece(row - 2, column - 1) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 2, column - 1));
		}
		// 2 jos, 1 dreapta
		if ((row - 2 >= 0) && (column + 1 < 8) && (((this.game.getPiece(row - 2, column + 1).color != side))
				|| (this.game.getPiece(row - 2, column + 1) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 2, column + 1));
		}

		// 2 stanga, 1 sus
		if ((row + 1 < 8) && (column - 2 >= 0) && (((this.game.getPiece(row + 1, column - 2).color != side))
				|| (this.game.getPiece(row + 1, column - 2) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 1, column - 2));
		}
		// 2 stanga, 1 jos
		if ((row - 1 >= 0) && (column - 2 >= 0) && (((this.game.getPiece(row - 1, column - 2).color != side))
				|| (this.game.getPiece(row - 1, column - 2) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 1, column - 2));
		}
		// 2 dreapta, 1 sus
		if ((row + 1 < 8) && (column + 2 < 8) && (((this.game.getPiece(row + 1, column + 2).color != side))
				|| (this.game.getPiece(row + 1, column + 2) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row + 1, column + 2));
		}
		// 2 dreapta, 1 jos
		if ((row - 1 >= 0) && (column + 2 < 8) && (((this.game.getPiece(row - 1, column + 2).color != side))
				|| (this.game.getPiece(row - 1, column + 2) instanceof Empty))) {
			this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(row - 1, column + 2));
		}
	}
}