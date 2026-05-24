#include "raylib.h"
#include <stdlib.h>
#include <time.h>

// Tile settings
#define TILE_SIZE 32
#define MAP_WIDTH 200
#define MAP_HEIGHT 200

// Tile types
#define GRASS 0
#define WATER 1
#define TREE 2
#define PATH 3

int map[MAP_HEIGHT][MAP_WIDTH];

// Generate random world
void GenerateWorld()
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int r = rand() % 100;

            if (r < 70)
                map[y][x] = GRASS;

            else if (r < 80)
                map[y][x] = TREE;

            else if (r < 90)
                map[y][x] = PATH;

            else
                map[y][x] = WATER;
        }
    }
}

int main()
{
    const int screenWidth = 1000;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Dynamic Pokemon Style World");

    srand(time(NULL));

    GenerateWorld();

    // Player
    Rectangle player = {400, 300, 28, 28};
    float speed = 200.0f;

    // Camera
    Camera2D camera = {0};
    camera.target = (Vector2){player.x, player.y};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.zoom = 1.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Movement
        if (IsKeyDown(KEY_RIGHT))
            player.x += speed * dt;

        if (IsKeyDown(KEY_LEFT))
            player.x -= speed * dt;

        if (IsKeyDown(KEY_UP))
            player.y -= speed * dt;

        if (IsKeyDown(KEY_DOWN))
            player.y += speed * dt;

        // Camera follows player
        camera.target = (Vector2)
        {
            player.x + player.width / 2,
            player.y + player.height / 2
        };

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(camera);

        // Determine visible tiles only
        int startX = (camera.target.x - screenWidth / 2) / TILE_SIZE - 2;
        int endX   = (camera.target.x + screenWidth / 2) / TILE_SIZE + 2;

        int startY = (camera.target.y - screenHeight / 2) / TILE_SIZE - 2;
        int endY   = (camera.target.y + screenHeight / 2) / TILE_SIZE + 2;

        // Clamp
        if (startX < 0) startX = 0;
        if (startY < 0) startY = 0;

        if (endX > MAP_WIDTH) endX = MAP_WIDTH;
        if (endY > MAP_HEIGHT) endY = MAP_HEIGHT;

        // Draw visible world
        for (int y = startY; y < endY; y++)
        {
            for (int x = startX; x < endX; x++)
            {
                int tile = map[y][x];

                int drawX = x * TILE_SIZE;
                int drawY = y * TILE_SIZE;

                // Grass
                if (tile == GRASS)
                {
                    DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, GREEN);

                    DrawCircle(drawX + 8, drawY + 10, 2, DARKGREEN);
                    DrawCircle(drawX + 20, drawY + 20, 2, DARKGREEN);
                }

                // Water
                else if (tile == WATER)
                {
                    DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, BLUE);

                    DrawLine(drawX + 5, drawY + 8,
                             drawX + 25, drawY + 8, SKYBLUE);
                }

                // Tree
                else if (tile == TREE)
                {
                    DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, GREEN);

                    DrawRectangle(drawX + 12, drawY + 16, 8, 14, BROWN);

                    DrawCircle(drawX + 16, drawY + 12, 12, DARKGREEN);
                }

                // Path
                else if (tile == PATH)
                {
                    DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, BROWN);
                }
            }
        }

        // Draw player
        DrawRectangleRec(player, RED);

        EndMode2D();

        DrawText("Move with Arrow Keys", 20, 20, 25, WHITE);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}