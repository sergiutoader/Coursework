
public class Eval {
	// Functia de evaluare a grid-ului
	public static int eval(Game game) {
		int i, j;
		int score = 0;
		for(i = 0; i <= 7; i++) {
			for(j = 0; j <= 7; j++) {
				if(!(game.grid[i][j] instanceof Empty)) {
					// punctare in functie de pozitia pe care se afla pionii
					// (in functie de valorile PieceSquare)

					// pentru pion exista 2 PieceSqareValues, pentru early, respectiv mid-late game
					if(game.moveCount < 10) {
						if(game.grid[i][j] instanceof Pawn && (game.grid[i][j].color == game.side)) {
							if(game.side == true) {
								score += PieceSquareValues.EarlyWhitePawnSquare[i][j];
							} else {
								score += PieceSquareValues.EarlyBlackPawnSquare[i][j];
							}					
						}

						if(game.grid[i][j] instanceof Pawn && (game.grid[i][j].color != game.side)) {
							if(game.side == true) {
								score -= PieceSquareValues.WhitePawnSquare[i][j];
							} else {
								score -= PieceSquareValues.BlackPawnSquare[i][j];
							}				
						}
					} else {
						if(game.grid[i][j] instanceof Pawn && (game.grid[i][j].color == game.side)) {
							if(game.side == true) {
								score += PieceSquareValues.WhitePawnSquare[i][j];
							} else {
								score += PieceSquareValues.BlackPawnSquare[i][j];
							}					
						}

						if(game.grid[i][j] instanceof Pawn && (game.grid[i][j].color != game.side)) {
							if(game.side == true) {
								score -= PieceSquareValues.WhitePawnSquare[i][j];
							} else {
								score -= PieceSquareValues.BlackPawnSquare[i][j];
							}				
						}
					}
					
				
					// punctare in functie de pozitia pe care se afla caii
					if(game.grid[i][j] instanceof Knight && (game.grid[i][j].color == game.side)) {
						if(game.side == true) {
							score += PieceSquareValues.WhiteKnightSquare[i][j];
						} else {
							score += PieceSquareValues.BlackKnightSquare[i][j];
						}
					}

					if(game.grid[i][j] instanceof Knight && (game.grid[i][j].color != game.side)) {
						if(game.side == true) {
							score -= PieceSquareValues.WhiteKnightSquare[i][j];
						} else {
							score -= PieceSquareValues.BlackKnightSquare[i][j];
						}
					}
								
					// punctare in functie de pozitia pe care se afla nebunii
					if(game.grid[i][j] instanceof Bishop && (game.grid[i][j].color == game.side)) {
						if(game.side == true) {
							score += PieceSquareValues.WhiteBishopSquare[i][j];
						} else {
							score += PieceSquareValues.BlackBishopSquare[i][j];
						}
					}

					if(game.grid[i][j] instanceof Bishop && (game.grid[i][j].color != game.side)) {
						if(game.side == true) {
							score -= PieceSquareValues.WhiteBishopSquare[i][j];
						} else {
							score -= PieceSquareValues.BlackBishopSquare[i][j];
						}
					}
								
					// punctare in functie de pozitia pe care se afla turele
					if(game.grid[i][j] instanceof Rook && (game.grid[i][j].color == game.side)) {
						if(game.side == true) {
							score += PieceSquareValues.WhiteRookSquare[i][j];
						} else {
							score += PieceSquareValues.BlackRookSquare[i][j];
						}					
					}

					if(game.grid[i][j] instanceof Rook && (game.grid[i][j].color != game.side)) {
						if(game.side == true) {
							score -= PieceSquareValues.WhiteRookSquare[i][j];
						} else {
							score -= PieceSquareValues.BlackRookSquare[i][j];
						}					
					}
				
					// punctare in functie de pozitia pe care se afla regina
					if(game.grid[i][j] instanceof Queen && (game.grid[i][j].color == game.side)) {
						if(game.side == true) {
							score += PieceSquareValues.WhiteQueenSquare[i][j];
						} else {
							score += PieceSquareValues.BlackQueenSquare[i][j];
						}			
					}

					if(game.grid[i][j] instanceof Queen && (game.grid[i][j].color != game.side)) {
						if(game.side == true) {
							score -= PieceSquareValues.WhiteQueenSquare[i][j];
						} else {
							score -= PieceSquareValues.BlackQueenSquare[i][j];
						}			
					}	
				
					// punctare in functie de pozitia pe care se afla regele
					// daca nu niciun jucator nu mai are regine, se trece la o stategie late-game
					if(game.grid[i][j] instanceof King && (game.grid[i][j].color == game.side)) {
						if(game.whitePieceCount[4] != 0 || game.blackPieceCount[4] != 0) {
							if(game.side == true) {
								score += PieceSquareValues.WhiteKingSquare[i][j];
							} else {
								score += PieceSquareValues.BlackKingSquare[i][j];
							}
						} else {
							score += PieceSquareValues.KingEndgameSquare[i][j];
						}
					}

					if(game.grid[i][j] instanceof King && (game.grid[i][j].color != game.side)) {
						if(game.whitePieceCount[4] != 0 || game.blackPieceCount[4] != 0) {
							if(game.side == true) {
								score -= PieceSquareValues.WhiteKingSquare[i][j];
							} else {
								score -= PieceSquareValues.BlackKingSquare[i][j];
							}
						} else {
							score -= PieceSquareValues.KingEndgameSquare[i][j];
						}
					}	

					if ((game.grid[i][j].color == game.side)) {
						score += game.grid[i][j].score;
					} else if (!(game.grid[i][j] instanceof Empty) &&
						(game.grid[i][j].color != game.side)){
						score -= game.grid[i][j].score;
					}
				}		
			}
		}
		return score;
	}

	public static boolean ended(Game game) {
		return game.whitePieceCount[5] == 0 || game.blackPieceCount[5] == 0;
	}
}