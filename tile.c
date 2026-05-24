#include "raylib.h"

#define TILE_SIZE 16

// Easy tile selector
#define TILE(col,row) (Rectangle){col*16,row*16,16,16}

int main(void)
{
    // Window
    InitWindow(800, 450, "Single Tile Example");

    // Load tileset
    Texture2D tileset = LoadTexture("tiles.png");

    // Select ONE tile from tileset
    // Example:
    // column = 4
    // row = 8
    Rectangle grassTile = TILE(2,8);

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        // Draw the selected tile
        DrawTexturePro(
            tileset,

            // SOURCE rectangle
            grassTile,

            // DESTINATION rectangle
            (Rectangle)
            {
                200, 100,      // screen position
                TILE_SIZE*8,   // scaled width
                TILE_SIZE*8    // scaled height
            },

            (Vector2){0,0},
            0.0f,
            WHITE
        );

        DrawText("Single Tile Example", 10, 10, 20, WHITE);

        EndDrawing();
    }

    // Cleanup
    UnloadTexture(tileset);

    CloseWindow();

    return 0;
}