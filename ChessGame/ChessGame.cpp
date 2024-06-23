// ChessGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ChessGame.h"
#include <iostream>
#include "raylib.h"

int main(void)
{
    InitWindow(800, 450, "raylib [core] example - basic window");

    while (!WindowShouldClose())
    {
        BeginDrawing();
        drawWindow();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}

void drawWindow() {
    ClearBackground(RAYWHITE);
    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);
}

