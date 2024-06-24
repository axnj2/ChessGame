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
			drawSquare(int(pos.x) + i * squareSize, int(pos.y) + j * squareSize, color);

		}
	}

	drawBitBoard(Color{ 255, 0,0,100 }, state.BPawns, piecesTextures["p"]);
	drawBitBoard(Color{ 0, 0,255,100 }, state.WPawns);


	// TODO add some sort of marker for which side it is the turn to play
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


std::map<std::string, Texture> Board::LoadPiecesImages() {
	std::map<std::string, Image> newPiecesImages;
	std::map<std::string, Texture> newPiecesTextures;

	Image completeImage = LoadImage("C:/Users/antoi/source/repos/ChessGame/ChessGame/assets/allPieces.png");

	ImageCrop(&completeImage, Rectangle{ 2,1,2556, 852 });
	std::cout << "width : " << completeImage.width << " height : " << completeImage.height << "\n";

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
			switch (piecesPos[i]) {
			case 'p':
				boardState.BPawns |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'n':
				boardState.BKnights |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'b':
				boardState.BBishops |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'q':
				boardState.BQueens |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'k':
				boardState.BKing |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'r':
				boardState.BRooks |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'P':
				boardState.WPawns |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'N':
				boardState.WKnights |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'B':
				boardState.WBishops |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'Q':
				boardState.WQueens |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'K':
				boardState.WKing |= static_cast<U64>(1) << (col+ row * 8);
				break;
			case 'R':
				boardState.WRooks |= static_cast<U64>(1) << (col  + row * 8);
				break;
			default:
				break;
			}
			col += 1;
		}
	};

	// std::cout << std::bitset<64>(boardState.BPawns) << '\n';


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
