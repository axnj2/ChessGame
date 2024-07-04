#include "raylib.h"
#include "Board.h"
#include <ctype.h>
#include <iterator>
#include <exception>

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
	//draw the squares
	drawBitBoard(whiteColor, 0b1010101001010101101010100101010110101010010101011010101001010101ull);
	drawBitBoard(blackColor, ~0b1010101001010101101010100101010110101010010101011010101001010101ull);

	if (squareSelected.x != -1) {
		vector<char> allies;
		if (state.WToMove) {
			allies = WPieces;
		}
		else {
			allies = BPieces;
		}

		drawBitBoard(Color{ 0,0,0,50 }, getMaskBitBoard(squareSelected));
		drawBitBoard(
			Color{ 255, 0 , 0, 100 },
			getValidMovesBitBoard(squareSelected, whatIsOnSquare(squareSelected), state));
	}

	for (int i = 0; i < pieces.size(); i++) {
		drawBitBoard(Color{ 255, 0, 0, 100 }, state.piecesBitmaps[pieces[i]], piecesTextures[pieces[i]]);
	}

	drawBitBoard(Color{ 0,0,255,100 }, getAttackedSquaresBy(state.WToMove, state));
	drawBitBoard(Color{ 0, 255, 0, 100 }, getMaskBitBoard(state.enPassant));
	
	// draw the turn number
	DrawText(to_string(state.turn).c_str(), squareSize * 8 + squareSize / 4, squareSize * 4, 50, GRAY);

	// indicate whose turn is is : 
	string text = "Your Turn";
	DrawText(text.c_str(), squareSize * 8 + squareSize / 4, squareSize + state.WToMove * (squareSize * 6), 50, BLACK);


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
			if (safeMovePiece(squareSelected, targetSquare)) {
				state.WToMove = !state.WToMove;
				if (state.WToMove) {
					state.turn += 1;
				}
			}
		}
		squareSelected.x = -1;
	}
};

/*
Uses the state variable to access the bitboards and whose turn it is to move
TODO :
 - en Passant
 - checkmate
*/
bool Board::safeMovePiece(Vector2Int from, Vector2Int to) {
	// first find what piece is on the from square (assumes 1 piece per square)
	char pieceOnSquare = whatIsOnSquare(from);
	
	// check that the move is valid :
	if (!(getValidMovesBitBoard(from, pieceOnSquare, state) & getMaskBitBoard(to))) {
		return false;
	}

	


	// now that we know the move is valid
	state = movePiece(from, to, state);
	return true;
}

// doesn't do any checks, assumes the to square is either empty or as an enemy piece that needs to be killeds
BoardState Board::movePiece(Vector2Int from, Vector2Int to, BoardState oldState)
{
	BoardState newState = oldState;
	// first find what piece is on the from square (assumes 1 piece per square)
	char pieceOnSquare = whatIsOnSquare(from, newState);

	// check if the target square is occupied by enemy, if so kill it
	char enemyPiece = whatIsOnSquare(to, newState); // can't be a friendly piece since it was checked before
	if (enemyPiece) { // false if the square is empty
		newState = removePiece(to, enemyPiece, newState);
	}


	// check that there is no piece on the to square
	bool empty = !whatIsOnSquare(to, newState);

	if (empty) {
		newState = removePiece(from, pieceOnSquare, newState);
		newState = addPiece(to, pieceOnSquare, newState);
	}
	else {
		throw std::runtime_error("there was still a piece on the target square");
	}

	// kill pawn that was a victim of enPassant :
	if (to == oldState.enPassant && (pieceOnSquare == 'p' || pieceOnSquare == 'P')) {
		if (isupper(pieceOnSquare)) {
			newState = removePiece(Vector2Int{ oldState.enPassant.x, oldState.enPassant.y + 1 }, 'p', oldState);
		}
		else {
			newState = removePiece(Vector2Int{ oldState.enPassant.x, oldState.enPassant.y - 1 }, 'P', oldState);
		}
	}


	// save EnPassant opportunity :
	if ((pieceOnSquare == 'p' || pieceOnSquare == 'P') && (from.y == 1 || from.y == 6) && (to.y == 3 || to.y == 4)) {
		if (to.y == 3) {
			newState.enPassant = Vector2Int{ to.x, 2 };
		}
		else if (to.y == 4) {
			newState.enPassant = Vector2Int{ to.x, 5 };
		}
		else {
			cout << "ERROR : there was an issue in move Piece in the save En passant opportunity logic" << endl;
		}
	}
	else {
		newState.enPassant = Vector2Int{ -1, -1 };
	}

	return newState;
}







U64 Board::getMaskBitBoard(Vector2Int square) {
	if (square.x == -1 || square.y == -1) {
		return 0ull;
	}
	else if (square.x < 0 || square.y < 0 || square.x > 7 || square.y > 7) {
		cout << "ERROR : in getMaskBitBoard, square out of range !" << endl;
		return 0ull;
	}

	return static_cast<U64>(1) << (square.x + square.y * 8);
}

// TODO this should be taking the working state as argument and not looking at the variable state
// that means changing all the sub functions
U64 Board::getAttacksBitBoard(Vector2Int square, char piece, BoardState workingState)
{
	U64 attacks = 0ull;
	if (piece == 'n' || piece == 'N') {
		attacks = getValidMovesBitBoardKnight(square, workingState);
	}
	else if (piece == 'p' || piece == 'P') {
		attacks = getValidAttacksPawn(square, isupper(piece), workingState);
	}
	else if (piece == 'r' || piece == 'R') {
		attacks = getValidMovesBitBoardRook(square, workingState);
	}
	else if (piece == 'b' || piece == 'B') {
		attacks = getValidMovesBitBoardBishop(square, workingState);
	}
	else if (piece == 'q' || piece == 'Q') {
		attacks = getValidMovesBitBoardQueen(square, workingState);
	}
	else if (piece == 'k' || piece == 'K') {
		attacks = getValidMovesBitBoardKing(square, workingState);
	}

	return attacks;
}

U64 Board::getValidMovesBitBoard(Vector2Int square, char piece, BoardState workingState)
{
	U64 validMoves;
	if (piece == 'p' || piece == 'P') {
		validMoves = getValidMovesBitBoardPawn(square, isupper(piece), workingState);
	}
	else {
		validMoves = getAttacksBitBoard(square, piece, workingState);
	}
	validMoves = removeAllies(validMoves, getAllies(isupper(piece)));
	validMoves = removeChecksFromPossibleMoves(validMoves, square, piece);
	return validMoves;
}



U64 Board::getValidMovesBitBoardKnight(Vector2Int square, BoardState workingState)
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

	return  finalMask;
}

U64 Board::getValidMovesBitBoardPawn(Vector2Int square, bool isWhite, BoardState workingState)
{
	U64 finalBitBoard = 0;
	int direction;
	bool onHomeRow = false;
	if (isWhite) {
		direction = -1;
		if (square.y == 6) {
			onHomeRow = true;
		}
	}
	else {
		direction = 1;
		if (square.y == 1) {
			onHomeRow = true;
		}
	}

	if (!whatIsOnSquare(Vector2Int{ square.x, square.y + direction }, pieces, workingState)) {
		finalBitBoard |= getMaskBitBoard(Vector2Int{ square.x, square.y + direction });
		if (onHomeRow && !whatIsOnSquare(Vector2Int{ square.x, square.y + 2 * direction }, pieces, workingState)) {
			finalBitBoard |= getMaskBitBoard(Vector2Int{ square.x, square.y + 2 * direction });
		}
	}

	// attacks
	finalBitBoard |= getValidAttacksPawn(square, isWhite, workingState);

	

	return finalBitBoard;
}

U64 Board::getValidAttacksPawn(Vector2Int square, bool isWhite, BoardState workingState)
{
	U64 finalBitBoard = 0;
	int direction;
	if (isWhite) {
		direction = -1;
	}
	else {
		direction = 1;
	}

	// attacks
	if (square.x - 1 >= 0 && whatIsOnSquare(Vector2Int{ square.x - 1, square.y + direction }, pieces, workingState)) {
		finalBitBoard |= getMaskBitBoard(Vector2Int{ square.x - 1, square.y + direction });
	}
	if (square.x + 1 < 8 && whatIsOnSquare(Vector2Int{ square.x + 1, square.y + direction }, pieces, workingState)) {
		finalBitBoard |= getMaskBitBoard(Vector2Int{ square.x + 1, square.y + direction });
	}

	// TODO en passant !
	if (square.y + direction == workingState.enPassant.y) {
		if (square.x + 1 == workingState.enPassant.x || square.x - 1 == workingState.enPassant.x) {
			finalBitBoard |= getMaskBitBoard(workingState.enPassant);
		}
	}

	return finalBitBoard;
}

// ugly implementation really not proud of this
U64 Board::getValidMovesBitBoardRook(Vector2Int square, BoardState workingState) {
	U64 finalMask;

	finalMask = 0ull;
	
	//East
	for (int x = square.x + 1; x < 8; x++) {
		if (!whatIsOnSquare(Vector2Int{ x, square.y}, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ x,square.y });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ x, square.y });
			break;
		}
	}
	// W
	for (int x = square.x - 1; x >= 0; x--) {
		if (!whatIsOnSquare(Vector2Int{ x, square.y }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ x,square.y });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ x,square.y });
			break;
		}
	}

	// N
	for (int y = square.y - 1; y >= 0; y--) {
		if (!whatIsOnSquare(Vector2Int{ square.x, y }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ square.x, y });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ square.x, y });
			break;
		}
	}
	
	for (int y = square.y + 1; y < 8; y++) {
		if (!whatIsOnSquare(Vector2Int{ square.x, y }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ square.x, y });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ square.x, y});
			break;
		}
	}

	return finalMask;
}

U64 Board::getValidMovesBitBoardBishop(Vector2Int square, BoardState workingState)
{
	U64 finalMask = 0ull;

	// SouthEast
	for (int x = square.x +1; x < 8 && x + square.y - square.x <8; x++) {
		if (!whatIsOnSquare(Vector2Int{ x, x +square.y - square.x }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ x, x + square.y - square.x });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ x, x + square.y - square.x });
			break;
		}
	}
	// N
	for (int x = square.x - 1; x >= 0 && x + square.y - square.x  >= 0; x--) {
		if (!whatIsOnSquare(Vector2Int{ x, x + square.y - square.x }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ x, x + square.y - square.x });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ x, x + square.y - square.x });
			break;
		}
	}
	// NE
	for (int x = square.x + 1; x < 8 && -x + square.y + square.x < 8 && -x + square.y + square.x >=0; x++) {
		if (!whatIsOnSquare(Vector2Int{ x, -x + square.y + square.x }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ x, -x + square.y + square.x });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ x, -x + square.y + square.x });
			break;
		}
	}
	// SO
	for (int x = square.x - 1; x >= 0 && -x + square.y + square.x >= 0 && -x + square.y + square.x <8; x--) {
		if (!whatIsOnSquare(Vector2Int{ x, -x + square.y + square.x }, workingState)) {
			finalMask |= getMaskBitBoard(Vector2Int{ x, -x + square.y + square.x });
		}
		else {
			finalMask |= getMaskBitBoard(Vector2Int{ x, -x + square.y + square.x });
			break;
		}
	}

	return finalMask;
}

U64 Board::getValidMovesBitBoardQueen(Vector2Int square, BoardState workingState)
{
	return getValidMovesBitBoardRook(square, workingState) | getValidMovesBitBoardBishop(square, workingState);
}

U64 Board::getValidMovesBitBoardKing(Vector2Int square, BoardState workingState)
{
	U64 finalMask = 0ull;

	// x  x  x  0  0  0  0  0
	// x  k  x  0  0  0  0  0
	// x  x  x 
	U64 baseMask = 0b1110000010100000111ull;

	Vector2Int shift = Vector2Int{ square.x - 1, square.y - 1 };

	finalMask = shiftMask(baseMask, shift);

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

U64 Board::getAttackedSquaresBy(bool isWhite, BoardState positions)
{
	vector<char> attackers = getAllies(isWhite);
	U64 attackedSquares = 0ull;

	for (char piece : attackers) {
		for (Vector2Int piecePosition : getAllPosInBitBoard(positions.piecesBitmaps[piece])) {
			attackedSquares |= getAttacksBitBoard(piecePosition, piece, positions);
		}
	}

	return attackedSquares;
}

bool Board::isInCheckBy(bool isWhite, BoardState positions)
{
	U64 attackedSquares = getAttackedSquaresBy(isWhite, positions);
	/*cout << "attackedSquares :";
	print(attackedSquares);
	cout << "king position :  ";
	print(positions.piecesBitmaps[getAllies(!isWhite)[0]]);*/
	// WPieces and BPieces have the king in first position
	return attackedSquares & positions.piecesBitmaps[getAllies(!isWhite)[0]];
}

U64 Board::removeChecksFromPossibleMoves(U64 possibleMoves, Vector2Int square, char piece)
{
	//cout << "piece : " << piece << endl;
	U64 newPossibleMoves = possibleMoves;

	for (Vector2Int move : getAllPosInBitBoard(possibleMoves)) {
		if (isInCheckBy(islower(piece), movePiece(square, move, state))) {
			//cout << "found invalid move, before : ";
			//print(newPossibleMoves);
			newPossibleMoves &= ~getMaskBitBoard(move);
			//cout << "after :                      ";
			//print(newPossibleMoves);
		}
	}

	return newPossibleMoves;
}



U64 Board::removeOverLaps(U64 A, U64 B)
{
	U64 overlaps = A & B;
	return A - overlaps;
}

U64 Board::removeAllies(U64 mask, vector<char> allies)
{
	U64 finalMask = mask;
	
	// remove squares that are occupied by allied pieces.
	for (int i = 0; i < allies.size(); i++) {
		finalMask = removeOverLaps(finalMask, state.piecesBitmaps[allies[i]]);
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

U64 Board::columnMask(int column) {
	U64 firstCol = 0b0000000100000001000000010000000100000001000000010000000100000001ull;

	return firstCol << column;
}



char Board::whatIsOnSquare(Vector2Int square)
{
	return whatIsOnSquare(square, pieces, state);
}

char Board::whatIsOnSquare(Vector2Int square, vector<char> SelectedPieces)
{
	return whatIsOnSquare(square, SelectedPieces, state);
}

char Board::whatIsOnSquare(Vector2Int square, const vector<char> SelectedPieces, BoardState currentState)
{
	char piece = 0;
	U64 fromBitBoardMask = getMaskBitBoard(square);

	for (int i = 0; i < SelectedPieces.size(); i++) {
		if (currentState.piecesBitmaps[SelectedPieces[i]] & fromBitBoardMask) {
			piece = SelectedPieces[i];
			break;
		}
	}

	return piece;
}

char Board::whatIsOnSquare(Vector2Int square,  BoardState currentState)
{
	return whatIsOnSquare(square, pieces, currentState);
}


void Board::removePiece(Vector2Int square, char piece) {
	// TODO make a version with previous and new state variable
	U64 removeMask = ~getMaskBitBoard(square); 
	state.piecesBitmaps[piece] &= removeMask;
}

BoardState Board::removePiece(Vector2Int square, char piece, BoardState oldState)
{
	U64 removeMask = ~getMaskBitBoard(square);
	oldState.piecesBitmaps[piece] &= removeMask;
	return oldState;
}


void Board::addPiece(Vector2Int square, char piece) {
	// TODO make a version with previous and new state variable
	U64 addMask = getMaskBitBoard(square);
	state.piecesBitmaps[piece] |= addMask;
}

BoardState Board::addPiece(Vector2Int square, char piece, BoardState oldState)
{
	U64 addMask = getMaskBitBoard(square);
	oldState.piecesBitmaps[piece] |= addMask;
	return oldState;
}




/*
Returns Vector2Int{-2, -2} is the click is outside of the board
*/
Vector2Int Board::processClick(int x, int y) {
	Vector2Int square;
	x -= int(pos.x);
	y -= int(pos.y);

	if (x > squareSize * 8 || y > squareSize * 8 || x <0 || y < 0) {
		square = Vector2Int{ -2, -2 };
	}
	else {
		square = Vector2Int{ x / squareSize, y / squareSize };
	}

	return square;
}

void Board::print(U64 bitBoard) {
	cout << std::bitset<64>(bitBoard) << endl;
}

void Board::print(Vector2Int vect)
{
	cout << "(x, y) = (" << vect.x << ", " << vect.y << ")" << endl;
}

vector<char> Board::getAllies(bool isWhite)
{
	if (isWhite) {
		return WPieces;
	}
	else {
		return BPieces;
	}
}

vector<Vector2Int> Board::getAllPosInBitBoard(U64 bitBoard)
{
	vector<Vector2Int> allPos = vector<Vector2Int>();
	// could be optimized
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			if (bitBoard & getMaskBitBoard(Vector2Int{ x, y })) {
				allPos.push_back(Vector2Int{ x, y });
			}
		}
	}

	return allPos;
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

	Image completeImage = LoadImage("./allPieces.png");
	
	if (!completeImage.width) {
		// only needed when running in Visual studio
		completeImage = LoadImage("C:/Users/antoi/source/repos/ChessGame/x64/Debug/allPieces.png");
	}

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
	boardState.enPassant = Vector2Int{ -1, -1 };

	// TODO halfTurn Clock

	boardState.turn = stoi(FullMoveClock);

	return boardState;
};

bool operator==(const Vector2Int& lhs, const Vector2Int& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
