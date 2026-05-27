#include "raylib.h"
#include<stdlib.h>


#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TILE_SIZE 60
#define MAP_W 10
#define MAP_H 10

// 0 = empty, 1 = rock
int map[MAP_H][MAP_W] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,1,0,0,0,1,1,0,0},
    {0,1,0,0,0,0,0,1,0,0},
    {0,0,0,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0},
    {0,1,0,0,1,1,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0},
};
int mapover[MAP_H][MAP_W] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,0,0,1,1,0,0},
    {0,1,0,1,0,0,0,1,0,0},
    {0,0,0,1,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,1,1,0,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,0,0,0,0,0,1,0,0},
    {0,1,0,0,1,1,0,1,0,0},
    {0,0,0,0,0,0,0,0,0,0},
};

// check collision
int IsBlocked(int x, int y)
{
    // boundary check
    if (x < 0 || y < 0 || x >= MAP_W || y >= MAP_H)
        return 1;

    return (mapover[y][x] == 1);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tile Collision Example");
    SetTargetFPS(40);
    Texture2D texture = LoadTexture("player.png");

    // player in TILE coordinates
    // int playerX = 1;
    // int playerY = 1;
    int frameWidth = texture.width / 4;
    int frameHeight = texture.height / 4;

    Rectangle frameRec = { 0, 0, frameWidth, frameHeight };
    Vector2 position= { 1, 1 };
  

    int currentFrame = 0;
    int currentRow = 0; // 0=down,1=left,2=right,3=up

    float frameTime = 0.0f;
    float frameSpeed = 0.15f;

    while (!WindowShouldClose())
    {
        Vector2 nextPos = position;

if (IsKeyDown(KEY_RIGHT))
{
    nextPos.x += 2;
    currentRow = 3;
}
else if (IsKeyDown(KEY_LEFT))
{
    nextPos.x -= 2;
    currentRow = 2;
}
else if (IsKeyDown(KEY_UP))
{
    nextPos.y -= 2;
    currentRow = 1;
}
else if (IsKeyDown(KEY_DOWN))
{
    nextPos.y += 2;
    currentRow = 0;
}

float scale = 3.0f;

float playerWidth = frameWidth / scale;
float playerHeight = frameHeight / scale;

// character hitbox corners
int leftTile   = nextPos.x / TILE_SIZE;
int rightTile  = (nextPos.x + playerWidth - 1) / TILE_SIZE;
int topTile    = nextPos.y / TILE_SIZE;
int bottomTile = (nextPos.y + playerHeight - 1) / TILE_SIZE;

if (
    !IsBlocked(leftTile, topTile) &&
    !IsBlocked(rightTile, topTile) &&
    !IsBlocked(leftTile, bottomTile) &&
    !IsBlocked(rightTile, bottomTile)
)
{
    position = nextPos;
}
 if(IsKeyDown(KEY_UP) ||IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_RIGHT))
        {
            frameTime += GetFrameTime();
            if (frameTime >= frameSpeed)
            {
                frameTime = 0.0f;
                (currentFrame)++;
                if (currentFrame > 3) 
                {
                    currentFrame = 0;
                }
            }
        }
    
        frameRec.x = (currentFrame) * frameWidth;
    frameRec.y = (currentRow) * frameHeight;

        // DRAW
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw map
        for (int y = 0; y < MAP_H; y++)
        {
            for (int x = 0; x < MAP_W; x++)
            {
                Rectangle tile = {
                    x * TILE_SIZE,
                    y * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };

                if (map[y][x] == 1)
                    DrawRectangleRec(tile, DARKGRAY);
                else
                    DrawRectangleRec(tile, LIGHTGRAY);

                DrawRectangleLines(tile.x, tile.y, TILE_SIZE, TILE_SIZE, GRAY);
            }
        }
        for (int y = 0; y < MAP_H; y++)
        {
            for (int x = 0; x < MAP_W; x++)
            {
                Rectangle tile = {
                    x * TILE_SIZE,
                    y * TILE_SIZE,
                    TILE_SIZE,
                    TILE_SIZE
                };

                if (mapover[y][x] == 1)
                    DrawRectangleRec(tile, DARKGRAY);
                else
                    DrawRectangleRec(tile, LIGHTGRAY);

                DrawRectangleLines(tile.x, tile.y, TILE_SIZE, TILE_SIZE, GRAY);
            }
        }

        // draw player
        Rectangle destRec = { position.x, position.y, frameWidth/3.0f, frameHeight/3.0f };
    Vector2 origin = {0, 0};
    DrawTexturePro(texture, frameRec, destRec, origin, 0.0f, WHITE);  

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
