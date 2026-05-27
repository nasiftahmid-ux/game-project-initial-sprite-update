#include "raylib.h"

int main()
{
    const int screenWidth = 1500;
    const int screenHeight = 850;

    InitWindow(screenWidth, screenHeight, "Scrolling Background");

    Texture2D bg = LoadTexture("redbal.png");

    float bgX = 0;
    float speed = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Background move
        bgX -= speed;

        // Reset when fully gone
        if (bgX <= -bg.width)
        {
            bgX = 0;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        // Draw 2 copies
        DrawTexture(bg, bgX, 0, WHITE);
        DrawTexture(bg, bgX + bg.width, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(bg);

    CloseWindow();

    return 0;
}