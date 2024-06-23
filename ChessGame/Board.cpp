#include "raylib.h"
#include "Board.h"

Board::Board(
	struct Color newWhiteColor,
	struct Color newBlackColor,
	int newBoardSize,
	struct Vector2 newPos
) {
	whiteColor = newWhiteColor;
	blackColor = newBlackColor;
	boardSize = newBoardSize;
	pos = newPos;
	squareSize = boardSize / 8;
}


void Board::drawSquare(int posx, int posy, struct Color squareColor){
	DrawRectangle(posx, posy, squareSize, squareSize, squareColor);
}

void Board::drawBoard() {
	struct Color color;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2) {
				color = blackColor;
			}
			else
			{
				color = whiteColor;
			}
			drawSquare(pos.x + i * squareSize, pos.y + j * squareSize, color);

		}
	}
}

