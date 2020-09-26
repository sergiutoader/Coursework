
import java.util.ArrayList;

public class Queen extends Piece {

	public Queen(String position, boolean color, Game game) {

		this.position = position;
		this.color = color;
		this.possibleMoves = new ArrayList<String>();
		this.game = game;
		this.score = 900;

	}
	
	public void updatePossibleMoves(boolean side, Game game) {
		
		// resetare arrayList possibleMoves
		this.possibleMoves = new ArrayList<String>();

		int row, column, i, j;

		row = this.getRow();
		column = this.getColumn();
		this.game = game;
		
		// Stanga-sus
		i = row;
		j = column;
		while (true) {
			i++;
			j--;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Dreapta-sus
		i = row;
		j = column;
		while (true) {
			i++;
			j++;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Stanga-jos
		i = row;
		j = column;
		while (true) {
			i--;
			j--;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Dreapta-jos
		i = row;
		j = column;
		while (true) {
			i--;
			j++;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Sus
		i = row;
		j = column;
		while (true) {
			i++;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Jos
		i = row;
		j = column;
		while (true) {
			i--;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Stanga
		i = row;
		j = column;
		while (true) {
			j--;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}

		// Dreapta
		i = row;
		j = column;
		while (true) {
			j++;

			if (i < 0 || i > 7 || j < 0 || j > 7)
				break;

			if (Utils.freeSpace(i, j, game))
				this.possibleMoves.add(Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if (!(game.getPiece(i, j) instanceof Empty))
				break;
		}
	}
}