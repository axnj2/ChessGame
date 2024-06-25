#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include <string>
#include <map>
#include <vector>

typedef unsigned long long U64;

using namespace std;


typedef struct BoardState {
	// keys are contained in pieces
	map<char, U64> piecesBitmaps;
	U64 WEnPassant;
	U64 BEnPassant;
	bool WToMove;

} BoardState;

struct Vector2Int {
	int x;
	int y;
};

class Board {
private:
	struct Color whiteColor;
	struct Color blackColor;
	int squareSize;
	int boardSize;
	struct Vector2 pos;
	BoardState state;
	std::map<char, Texture> piecesTextures;
	static const std::vector<char> pieces;
	const std::vector<char> WPieces
		= { 'K', 'Q', 'B', 'N', 'R', 'P' };
	const std::vector<char> BPieces
		= { 'k', 'q', 'b', 'n', 'r', 'p' };

	// interactions
	Vector2Int squareSelected;

	void drawSquare(int posx, int posy, struct Color squareColor);
	BoardState ReadFEN(std::string FENState);
	std::map<char, Texture> LoadPiecesImages();

	void drawBitBoard(Color, U64, Texture = Texture{});
	// returns true if the move was valid
	bool movePiece(Vector2Int, Vector2Int);
	U64 getMaskBitBoard(Vector2Int);
	U64 getValidMovesBitBoard(Vector2Int, char);
	U64 getValidMovesBitBoardKnight(Vector2Int, bool);
	U64 shiftMask(U64, Vector2Int);
	U64 lineMask(int line);
	char whatIsOnSquare(Vector2Int, const vector<char> = pieces);
	void removePiece(Vector2Int, char);
	void addPiece(Vector2Int, char);
	Vector2Int processClick(int, int);
	void displayBitBoard(U64);

public:
	Board(
		struct Color newWhiteColor,
		struct Color newBlackColor,
		int newBoardSize,
		struct Vector2 newPos,
		std::string startingFENState);// using FEN notation https://www.chessprogramming.org/Forsyth-Edwards_Notation
	void drawBoard();
	void onMouseClick();
};



#endif // !BOARD_H