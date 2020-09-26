
import java.util.ArrayList;

public abstract class Piece {

	public Game game;
	public String position;
	public boolean color;
	public ArrayList <String> possibleMoves;
	public String enPassant = null;

	public int score;

	public boolean hasMoved = false;
	
	public abstract void updatePossibleMoves(boolean side, Game game);
	protected int getRow(){
		return position.charAt(1) - '0' - 1;
	}

	protected int getColumn(){
		return position.charAt(0) - 'a';
	}

	@Override
	public String toString(){
		return ("Position: " + position +"; Color: " + color + "; Score: " + score);
	}
}