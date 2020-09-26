
public class Empty extends Piece {

	public Empty(String position, boolean color, Game game){

		this.position = position;
		this.color = color;
		this.possibleMoves = null;
		this.game = game;
		this.score = 0;
		
	}

	public void updatePossibleMoves(boolean side, Game game) {
	}
}