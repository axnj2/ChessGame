#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include <string>
#include <map>

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

class Board 
{
private:
	struct Color whiteColor;
	struct Color blackColor;
	int squareSize;
	int boardSize;
	struct Vector2 pos;
	BoardState state;
	std::map<std::string, Texture> piecesTextures;
	std::string pieces[12] 
		= {"K", "Q", "B", "N", "R", "P", "k", "q", "b", "n", "r", "p" };


	void drawSquare(int posx, int posy, struct Color squareColor);
	BoardState ReadFEN(std::string FENState);
	std::map<std::string, Texture> LoadPiecesImages();
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