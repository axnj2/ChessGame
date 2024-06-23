#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"


class Board 
{
private:
	struct Color whiteColor;
	struct Color blackColor;
	int squareSize;
	int boardSize;
	struct Vector2 pos;

	void drawSquare(int posx, int posy, struct Color squareColor);
public:
	Board(
		struct Color newWhiteColor,
		struct Color newBlackColor,
		int newBoardSize,
		struct Vector2 newPos);
	void drawBoard();
	


};

#endif // !BOARD_H