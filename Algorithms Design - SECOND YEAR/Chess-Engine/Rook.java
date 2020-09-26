
import java.util.ArrayList;

public class Rook extends Piece {

	public Rook(String position, boolean color, Game game){

		this.position = position;
		this.color = color;
		this.possibleMoves = new ArrayList <String>();
		this.game = game;
		this.score = 500;
		this.hasMoved = false;
	}

	public Rook(String position, boolean color, Game game, boolean hasMoved) {
		this(position, color, game);
		this.hasMoved = hasMoved;
	}

	public void updatePossibleMoves(boolean side, Game game) {
		
		// resetare arrayList possibleMoves
		this.possibleMoves = new ArrayList <String>();
		
		int row, column, i, j;
		
		row = this.getRow();
		column = this.getColumn();
		this.game = game;
		
		// Sus
		i = row;
		j = column;
		while(true) {
			i++;
			
			if(i < 0 || i > 7 || j < 0 || j > 7)break;
			
			if(Utils.freeSpace(i, j, game) )this.possibleMoves.add(
				Utils.getPosition(row, column) + Utils.getPosition(i, j));
			if(!(game.getPiece(i, j) instanceof Empty) )break;
		}
		
		// Jos
		i = row;
		j = column;
		while(true) {
			i--;
			
			if(i < 0 || i > 7 || j < 0 || j > 7)break;
			
			if(Utils.freeSpace(i, j, game) )this.possibleMoves.add(
				Utils.getPosition(row, column) + Utils.getPosition(i, j) );
			if(!(game.getPiece(i, j) instanceof Empty) )break;
		}
				
		// Stanga
		i = row;
		j = column;
		while(true) {
			j--;
			
			if(i < 0 || i > 7 || j < 0 || j > 7)break;
			
			if(Utils.freeSpace(i, j, game) )this.possibleMoves.add(
				Utils.getPosition(row, column) + Utils.getPosition(i, j) );
			if(!(game.getPiece(i, j) instanceof Empty) )break;
		}
		
		// Dreapta
		i = row;
		j = column;
		while(true) {
			j++;
			
			if(i < 0 || i > 7 || j < 0 || j > 7)break;
			
			if(Utils.freeSpace(i, j, game) )this.possibleMoves.add(
				Utils.getPosition(row, column) + Utils.getPosition(i, j) );
			if(!(game.getPiece(i, j) instanceof Empty) )break;
		}
	}
}