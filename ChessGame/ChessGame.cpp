#include "ChessGame.h"
#include "Board.h"
#include <iostream>
#include <algorithm>
#include "raylib.h"

#define windowHeight 1000
#define windowWidth  1000
#define sideBuffer 10

int main(void)
{   
    Board testBoard = Board(
        Color{ 238, 238, 210, 255 },
        Color{ 118, 150, 86, 255 },
        std::min(windowHeight, windowWidth) - 2 * sideBuffer, 
        Vector2{ sideBuffer ,sideBuffer }, // only does nice spacing for square windows
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    );

    InitWindow(windowWidth, windowHeight, "ChessBoard");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        drawWindow(testBoard);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void drawWindow(Board board) {
    ClearBackground(RAYWHITE);
    board.drawBoard();
    // DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
}

