#include "raylib.h"
#include "Board.h"
#include <ctype.h>
#include <iterator>

//DEBUG :
#include <iostream>
#include <bitset>

using namespace std;


const std::vector<char> Board::pieces = { 'K', 'Q', 'B', 'N', 'R', 'P', 'k', 'q', 'b', 'n', 'r', 'p' };


Board::Board(
	struct Color newWhiteColor,
	struct Color newBlackColor,
	int newBoardSize,
	struct Vector2 newPos,
	std::string startingFENState
) {
	std::cout << "------------- Initializing Board -----------\n";

	whiteColor = newWhiteColor;
	blackColor = newBlackColor;
	boardSize = newBoardSize;
	pos = newPos;
	squareSize = boardSize / 8;
	state = ReadFEN(startingFENState);
	piecesTextures = LoadPiecesImages();

	squareSelected = Vector2Int{ -1, -1 };
}


void Board::drawBoard() {
	struct Color color;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			if ((i + j) % 2) {
				color = blackColor;
			}
			else {
				color = whiteColor;
			}
			drawSquare(int(pos.x) + i * squareSize, int(pos.y) + j * squareSize, color);
		}
	}

	if (squareSelected.x != -1) {
		drawBitBoard(Color{ 0,0,0,50 }, getMaskBitBoard(squareSelected));
	}

	for (int i = 0; i < 12; i++) {
		drawBitBoard(Color{ 255, 0, 0, 100 }, state.piecesBitmaps[pieces[i]], piecesTextures[pieces[i]]);
	}

	// drawBitBoard(GREEN, shiftMask(getMaskBitBoard(Vector2Int{ 2, 2 }), Vector2Int{ 2,2 }));

	drawBitBoard(RED, getValidMovesBitBoardKnight(Vector2Int{1, 0}, true));

	

	// TODO add some sort of marker for which side it is the turn to play

};


void Board::onMouseClick(){
	if (squareSelected.x == -1) {
		squareSelected = processClick(GetMouseX(), GetMouseY());

		if (squareSelected.x == -2) {
			squareSelected = Vector2Int{ -1, -1 };
		}
		if (state.WToMove) {
			if (!whatIsOnSquare(squareSelected, WPieces)) {
				squareSelected = Vector2Int{ -1, -1 };
			}
		}
		else {
			if (!whatIsOnSquare(squareSelected, BPieces)) {
				squareSelected = Vector2Int{ -1, -1 };
			}
		}
	}
	else {
		Vector2Int targetSquare = processClick(GetMouseX(), GetMouseY());
		if (targetSquare.x != -2) {
			if (movePiece(squareSelected, targetSquare)) {
				state.WToMove = !state.WToMove;
			}
		}
		squareSelected.x = -1;
	}
};

/*
Uses the state variable to access the bitboards and whose turn it is to move
TODO :
 - rules for each pieces
 - checks
 - checkmate
 - en Passant
*/
bool Board::movePiece(Vector2Int from, Vector2Int to) {
	// first find what piece is on the from square (assumes 1 piece per square)
	char friendlyPiece = whatIsOnSquare(from);
	

	
	std::vector<char> friendlyPieces;
	if (state.WToMove) {
		friendlyPieces = WPieces;
	}
	else
	{
		friendlyPieces = BPieces;
	}

	// check if occupied by friendly piece
	if (whatIsOnSquare(to, friendlyPieces)) {
		return false;
	}

	// check if the target square is occupied by enemy, if so kill it
	// TODO make it correct, but for now kills the enemy piece on the square
	
	char enemyPiece = whatIsOnSquare(to); // can't be a friendly piece since it was checked before
	if (enemyPiece) { // false if the square is empty
		removePiece(to, enemyPiece);
	}
	

	// check that there is no piece on the to square
	bool empty = !whatIsOnSquare(to);

	if (empty) {
		removePiece(from, friendlyPiece);
		addPiece(to, friendlyPiece);
		return true;
	}
	else {
		
	}
	return false;
};


U64 Board::getMaskBitBoard(Vector2Int square) {
	return static_cast<U64>(1) << (square.x + square.y * 8);
}

U64 Board::getValidMovesBitBoard(Vector2Int square, char piece)
{
	if (piece == 'n' || piece == 'N') {
		return getValidMovesBitBoardKnight(square, isupper(piece));
	}

	return U64();
}

U64 Board::getValidMovesBitBoardKnight(Vector2Int square, bool isWhite)
{
	// for a knight in position (2, 2)
	//     0  x  0  x  0  0  0  0
	//     x  0  0  0  x  0  0  0
	//     0  0  n  0  0  0  0  0
	//     x  0  0  0  x  0  0  0
	//     0  x  0  x  0  0  0  0
	U64 upDownMask = 0b1010;
	U64 leftRightMask = 0b10001;
	U64 baseMask = upDownMask | (upDownMask << 4 * 8) | (leftRightMask << 8) | (leftRightMask << 3 * 8);

	Vector2Int shift = Vector2Int{ square.x - 2, square.y - 2 };

	U64 shiftedMask = shiftMask(baseMask, shift);

	U64 finalMask = shiftedMask;

	vector<char> pieceList;
	if (isWhite) {
		pieceList = WPieces;
	}
	else {
		pieceList = BPieces;
	}
	// todo remove squares that are occupied by allied pieces.
	
	return finalMask;
}

// pure function
U64 Board::shiftMask(U64 baseMask, Vector2Int shift)
{
	U64 vertShiftMask;
	// vectical shift
	if (shift.y >= 0) {
		vertShiftMask = baseMask << shift.y * 8;
	}
	else {
		vertShiftMask = baseMask >> -shift.y * 8;
	}

	U64 finalMask = 0ull;

	// horizontal shift
	//process line by line
	for (int i = 0; i < 8; i++) {
		if (shift.x >= 0) {
			finalMask |= ((vertShiftMask & lineMask(i)) << shift.x) & lineMask(i);
		}
		else {
			finalMask |= ((vertShiftMask & lineMask(i)) >> -shift.x) & lineMask(i);
		}
	}

	return finalMask;
}

// could have been hard coded in a list to be honnest.
U64 Board::lineMask(int line) {
	U64 mask;

	if (line == 0) {

		mask = (1ull << (8 * (line + 1))) - 1;
	}
	else if (line == 7) {
		mask = (1ull << (8 * (line + 1) - 1)) - 1 - ((1ull << (8 * (line))) - 1) + (1ull << 63);
	}
	else {
		mask = (1ull << (8 * (line + 1))) - 1 - ((1ull << (8 * (line))) - 1);
	}
	return mask;
}	


char Board::whatIsOnSquare(Vector2Int square, vector<char> SelectedPieces)
{
	char piece = 0;
	U64 fromBitBoardMask = getMaskBitBoard(square);

	for (int i = 0; i < SelectedPieces.size(); i++) {
		if (state.piecesBitmaps[SelectedPieces[i]] & fromBitBoardMask) {
			piece = SelectedPieces[i];
			break;
		}
	}

	return piece;
}


void Board::removePiece(Vector2Int square, char piece) {
	U64 removeMask = ~getMaskBitBoard(square);
	state.piecesBitmaps[piece] &= removeMask;
}


void Board::addPiece(Vector2Int square, char piece) {
	U64 addMask = getMaskBitBoard(square);
	state.piecesBitmaps[piece] |= addMask;
}


/*
Returns Vector2Int{-2, -2} is the click is outside of the board
*/
Vector2Int Board::processClick(int x, int y) {
	Vector2Int square;
	x -= pos.x;
	y -= pos.y;

	if (x > squareSize * 8 || y > squareSize * 8 || x <0 || y < 0) {
		square = Vector2Int{ -2, -2 };
	}
	else {
		square = Vector2Int{ x / squareSize, y / squareSize };
	}

	return square;
}

void Board::displayBitBoard(U64 bitBoard) {
	cout << std::bitset<64>(bitBoard) << endl;
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

