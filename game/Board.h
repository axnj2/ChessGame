#pragma once

#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"
#include <string>
#include <map>
#include <vector>

typedef unsigned long long U64;

using namespace std;

struct Vector2Int {
	int x;
	int y;
};

bool operator==(const Vector2Int& lhs, const Vector2Int& rhs);


typedef struct BoardState {
	// keys are contained in pieces
	map<char, U64> piecesBitmaps;
	bool WToMove;
	unsigned int turn;
	Vector2Int enPassant;

} BoardState;



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
	bool safeMovePiece(Vector2Int from, Vector2Int to);
	BoardState movePiece(Vector2Int from, Vector2Int to, BoardState previousState);

	U64 getMaskBitBoard(Vector2Int); 

	U64 getAttacksBitBoard(Vector2Int square, char piece, BoardState workingState);
	U64 getValidMovesBitBoard(Vector2Int square, char piece, BoardState workingState);
	U64 getValidMovesBitBoardKnight(Vector2Int square, BoardState workingState);
	U64 getValidMovesBitBoardPawn(Vector2Int square, bool isWhite, BoardState workingState);
	U64 getValidAttacksPawn(Vector2Int square, bool isWhite, BoardState workingState);
	U64 getValidMovesBitBoardRook(Vector2Int square, BoardState workingState);
	U64 getValidMovesBitBoardBishop(Vector2Int square, BoardState workingState);
	U64 getValidMovesBitBoardQueen(Vector2Int square, BoardState workingState);
	U64 getValidMovesBitBoardKing(Vector2Int square, BoardState workingState);

	U64 shiftMask(U64, Vector2Int);

	U64 getAttackedSquaresBy(bool isWhite, BoardState positions);
	bool isInCheckBy(bool isWhite, BoardState positions);
	U64 removeChecksFromPossibleMoves(U64 possibleMoves, Vector2Int square, char piece);

	// remove from A all squares in B
	U64 removeOverLaps(U64 A, U64 B);
	U64 removeAllies(U64 mask, vector<char> allies);
	U64 lineMask(int line);
	U64 columnMask(int column);
	// return 0 if nothing is on the square
	char whatIsOnSquare(Vector2Int);
	char whatIsOnSquare(Vector2Int, const vector<char>);
	char whatIsOnSquare(Vector2Int, const vector<char>, BoardState);
	char whatIsOnSquare(Vector2Int, BoardState);
	void removePiece(Vector2Int, char);
	BoardState removePiece(Vector2Int, char, BoardState);
	void addPiece(Vector2Int, char);
	BoardState addPiece(Vector2Int, char, BoardState);
	Vector2Int processClick(int, int);
	void print(U64);
	void print(Vector2Int);

	vector<char> getAllies(bool isWhite);
	vector<Vector2Int> getAllPosInBitBoard(U64 bitBoard);



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