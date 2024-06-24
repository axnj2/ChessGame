#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include <string>

typedef unsigned long long U64;


typedef struct  {
	U64 WPawns;
	U64 WRooks;
	U64 WKnights;
	U64 WBishops;
	U64 WQueens;
	U64 WKing;
	U64 BPawns;
	U64 BRooks;
	U64 BKnights;
	U64 BBishops;
	U64 BQueens;
	U64 BKing;
	U64 WEnPassant;
	U64 BEnPassant;
	bool WToMove;

} BoardState;

typedef struct {
	Image WPawn;
	Image WRook;
	Image WKnight;
	Image WBishop;
	Image WQueen;
	Image WKing;
	Image BPawn;
	Image BRook;
	Image BKnight;
	Image BBishop;
	Image BQueen;
} PiecesImages;

class Board 
{
private:
	struct Color whiteColor;
	struct Color blackColor;
	int squareSize;
	int boardSize;
	struct Vector2 pos;
	BoardState state;
	PiecesImages piecesImages;

	void drawSquare(int posx, int posy, struct Color squareColor);
	BoardState ReadFEN(std::string FENState);
	PiecesImages LoadPiecesImages();
public:
	Board(
		struct Color newWhiteColor,
		struct Color newBlackColor,
		int newBoardSize,
		struct Vector2 newPos,
		std::string startingFENState);// using FEN notation https://www.chessprogramming.org/Forsyth-Edwards_Notation
	void drawBoard();
	void drawBitBoard(Color highlightColor, U64 bitboard);

};

#endif // !BOARD_H