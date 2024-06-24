#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include <string>
#include <map>

typedef unsigned long long U64;

using namespace std;


typedef struct  {
	// keys are contained in pieces
	map<char, U64> piecesBitmaps;
	U64 WEnPassant;
	U64 BEnPassant;
	bool WToMove;

} BoardState;

class Board 
{
private:
	struct Color whiteColor;
	struct Color blackColor;
	int squareSize;
	int boardSize;
	struct Vector2 pos;
	BoardState state;
	std::map<char, Texture> piecesTextures;
	char pieces[12] 
		= {'K', 'Q', 'B', 'N', 'R', 'P', 'k', 'q', 'b', 'n', 'r', 'p' };


	void drawSquare(int posx, int posy, struct Color squareColor);
	BoardState ReadFEN(std::string FENState);
	std::map<char, Texture> LoadPiecesImages();
public:
	Board(
		struct Color newWhiteColor,
		struct Color newBlackColor,
		int newBoardSize,
		struct Vector2 newPos,
		std::string startingFENState);// using FEN notation https://www.chessprogramming.org/Forsyth-Edwards_Notation
	void drawBoard();
	void drawBitBoard(Color highlightColor, U64 bitboard, Texture texture = Texture{});

};

#endif // !BOARD_H