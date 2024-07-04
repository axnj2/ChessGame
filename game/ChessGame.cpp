#include "ChessGame.h"
#include "Board.h"
#include <iostream>
#include <algorithm>
#include "raylib.h"

#define windowHeight 1000
#define windowWidth  1300
#define sideBuffer 10

int main(void)
{   
    InitWindow(windowWidth, windowHeight, "ChessBoard");
    SetTargetFPS(144); // avoids the game running at crazy 3000 FPS or more using 10% of CPU 
    


    Board testBoard = Board(
        Color{ 238, 238, 210, 255 },
        Color{ 118, 150, 86, 255 },
        std::min(windowHeight, windowWidth) - 2 * sideBuffer, 
        Vector2{ sideBuffer ,sideBuffer }, // places the board on the left of the screen
        "rnbqkbnr/pppppppp/8/3p1p2/4B3/3p4/PPPPPPPp/RNBQKBNR w KQkq - 0 1"
    );
    // "rnbqkbnr/pppppppp/8/3p1p2/4B3/3p4/PPPPPPPp/RNBQKBNR w KQkq - 0 1"
    //"8/Q5pk/8/8/8/8/8/4K3 b KQkq - 0 1"

    while (!WindowShouldClose())
    {
        BeginDrawing();
        drawWindow(&testBoard);
        EndDrawing();
        processInput(&testBoard);
    }

    CloseWindow();

    return 0;
}

void drawWindow(Board *board) {
    ClearBackground(RAYWHITE);
    board->drawBoard();
    DrawFPS(windowWidth - 150, 25);
}

void processInput(Board *board) {
    if (IsMouseButtonPressed(0)) {
        board->onMouseClick();
    }
}

