#include "ChessGame.h"
#include "Board.h"
#include <iostream>
#include <algorithm>
#include "raylib.h"

#define windowHeight 1000
#define windowWidth  1200
#define sideBuffer 10

int main(void)
{   
    InitWindow(windowWidth, windowHeight, "ChessBoard");

    


    Board testBoard = Board(
        Color{ 238, 238, 210, 255 },
        Color{ 118, 150, 86, 255 },
        std::min(windowHeight, windowWidth) - 2 * sideBuffer, 
        Vector2{ sideBuffer ,sideBuffer }, // only does nice spacing for square windows
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    );

    std::cout << Texture{}.height;
    

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
}

void processInput(Board *board) {
    if (IsMouseButtonPressed(0)) {
        board->onMouseClick();
    }
}

