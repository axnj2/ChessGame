#include "raylib.h"
#include "Board.h"
#include <ctype.h>

//DEBUG :
#include <iostream>
#include <bitset>

using namespace std;

Board::Board(
	struct Color newWhiteColor,
	struct Color newBlackColor,
	int newBoardSize,
	struct Vector2 newPos,
	std::string startingFENState
) {
	std::cout << "-------------Initializing Board ---------\n";

	whiteColor = newWhiteColor;
	blackColor = newBlackColor;
	boardSize = newBoardSize;
	pos = newPos;
	squareSize = boardSize / 8;
	state = ReadFEN(startingFENState);
	piecesTextures = LoadPiecesImages();

	squareSelected.x = -1;
	squareSelected.y = -1;
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
			drawSquare(int(pos.x) + i * squareSize, int(pos.y) + j * squareSize, color);

		}
	}

	for (int i = 0; i < 12; i++) {
		drawBitBoard(Color{ 255, 0,0,100 }, state.piecesBitmaps[pieces[i]], piecesTextures[pieces[i]]);
	}

	// TODO add some sort of marker for which side it is the turn to play
};


void Board::onMouseClick(){
	cout << squareSelected.x << "  " << squareSelected.y << endl;
	if (squareSelected.x == -1) {
		squareSelected.x = (GetMouseX() - int(pos.x)) / squareSize;
		squareSelected.y = (GetMouseY() - int(pos.y)) / squareSize;

		if (!whatIsOnSquare(squareSelected)) {
			squareSelected = Vector2Int{ -1, -1 };
		}
	}
	else {
		Vector2Int targetSquare = Vector2Int{
			(GetMouseX() - int(pos.x)) / squareSize,
			(GetMouseY() - int(pos.y)) / squareSize};

		movePiece(squareSelected, targetSquare);

		squareSelected.x = -1;
	}
	cout << squareSelected.x << "  " << squareSelected.y << endl;
};


void Board::movePiece(Vector2Int from, Vector2Int to) {
	// first find what piece is on the from square (assumes 1 piece per square)
	char piece = whatIsOnSquare(from);

	// check that there is no piece on the to square
	bool empty = !whatIsOnSquare(to);

	if (empty) {
		removePiece(from, piece);
		addPiece(to, piece);
	}
};


U64 Board::getMaskBitBoard(Vector2Int square) {
	return static_cast<U64>(1) << (square.x + square.y * 8);
}

char Board::whatIsOnSquare(Vector2Int square)
{
	char piece = 0;
	U64 fromBitBoardMask = getMaskBitBoard(square);

	for (int i = 0; i < 12; i++) {
		if (state.piecesBitmaps[pieces[i]] & fromBitBoardMask) {
			piece = pieces[i];
			break;
		}
	}

	return piece;
}

void Board::removePiece(Vector2Int square, char piece) {
	U64 removeMask = ~getMaskBitBoard(square);
	std::cout << std::bitset<64>(removeMask) << '\n';
	state.piecesBitmaps[piece] &= removeMask;
}

void Board::addPiece(Vector2Int square, char piece) {
	U64 addMask = getMaskBitBoard(square);
	std::cout << std::bitset<64>(addMask) << '\n';
	state.piecesBitmaps[piece] |= addMask;
}


void Board::drawSquare(int posx, int posy, struct Color squareColor) {
	DrawRectangle(posx, posy, squareSize, squareSize, squareColor);
};


void Board::drawBitBoard(Color highlightColor, U64 bitboard, Texture texture) {
	for (int col = 0; col < 8; col++) {
		for (int row = 0; row < 8; row++) {
			if (bitboard & static_cast<U64>(1) << (col + row * 8)) {
				if (texture.width == 0) {
					drawSquare(
						int(pos.x) + col * squareSize,
						int(pos.y) + row * squareSize,
						highlightColor);
				}
				else {
					DrawTexture(
						texture,
						int(pos.x) + col * squareSize,
						int(pos.y) + row * squareSize,
						WHITE);
				}
			}
		}
	}
};


std::map<char, Texture> Board::LoadPiecesImages() {
	std::map<char, Image> newPiecesImages;
	std::map<char, Texture> newPiecesTextures;

	// TODO change this to relative path
	Image completeImage = LoadImage("C:/Users/antoi/source/repos/ChessGame/ChessGame/assets/allPieces.png");

	ImageCrop(&completeImage, Rectangle{ 2,1,2556, 852 });
	// std::cout << "width : " << completeImage.width << " height : " << completeImage.height << "\n";

	for (int i = 0; i < 12; i++) {
		newPiecesImages[pieces[i]] = ImageCopy(completeImage);
		ImageCrop(&newPiecesImages[pieces[i]], Rectangle{ float(i % 6 * 426),float( int(i/6) * 426), 426, 426 });
		ImageResize(&newPiecesImages[pieces[i]], squareSize, squareSize);
		newPiecesTextures[pieces[i]] = LoadTextureFromImage(newPiecesImages[pieces[i]]);
	}

	return newPiecesTextures;
}


/*
Uses the structure of the FEN notation : https://www.chessprogramming.org/Forsyth-Edwards_Notation

which is : <piecesPos> <Side to move> <Castling ability> 
           <En passant target square> <Half move clock> <Full move counter>
with spaces as delimiters
*/
BoardState Board::ReadFEN(std::string FENState) {
	BoardState boardState = BoardState{};
	string remainingFEN = FENState;

	string delimiter = " ";
	string piecesPos = remainingFEN.substr(0, remainingFEN.find(delimiter));
	//std::cout << piecesPos << "\n";

	remainingFEN = remainingFEN.substr(remainingFEN.find(delimiter) + 1);
	string sideToMove = remainingFEN.substr(0, remainingFEN.find(delimiter));
	//std::cout << sideToMove << "\n";

	remainingFEN = remainingFEN.substr(remainingFEN.find(delimiter) + 1);
	string castling = remainingFEN.substr(0, remainingFEN.find(delimiter));
	//std::cout << castling << "\n";

	remainingFEN = remainingFEN.substr(remainingFEN.find(delimiter) + 1);
	string enPassant = remainingFEN.substr(0, remainingFEN.find(delimiter));
	//std::cout << enPassant << "\n";

	remainingFEN = remainingFEN.substr(remainingFEN.find(delimiter) + 1);
	string HalfMoveClock = remainingFEN.substr(0, remainingFEN.find(delimiter));
	//std::cout << HalfMoveClock << "\n";

	remainingFEN = remainingFEN.substr(remainingFEN.find(delimiter) + 1);
	string FullMoveClock = remainingFEN.substr(0, remainingFEN.find(delimiter));
	//std::cout << FullMoveClock << "\n";


	// Board pieces positions

	int row = 0;
	int col = 0;

	for (int i = 0; i < piecesPos.length(); i++) {
		if (isdigit(piecesPos[i])) {
			// std::cout << piecesPos[i];
			// std::cout << "\n";
			col += piecesPos[i] - '0';
		}
		else if (piecesPos[i] == '/') {
			row += 1;
			col = 0;
		}
		else {
			boardState.piecesBitmaps[piecesPos[i]] |= static_cast<U64>(1) << (col + row * 8);
			col += 1;
		}
	};

	// std::cout << std::bitset<64>(boardState.piecesBitmaps['p']) << '\n';


	// side to move
	if (sideToMove == "w") {
		boardState.WToMove = true;
	}
	else if (sideToMove == "b") {
		boardState.WToMove = false;
	}
	else {
		cout << "invalid side to move in FEN notation \n";
	}

	// TODO castling

	// TODO En passant letter -> col and number -> row 
	// then use static_cast<U64>(1) << (col+ row * 8)
	// if row = 3 => Black En passant else WEnPassant

	// TODO clocks

	return boardState;
};