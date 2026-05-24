// #include "raylib.h"

// int main()
// {
//     const int screenWidth = 800;
//     const int screenHeight = 600;

//     InitWindow(screenWidth, screenHeight, "2D Camera Follow Character");
//     SetTargetFPS(60);

//     Vector2 position = { 400, 300 };

//     Camera2D camera = { 0 };
//     camera.target = position;              // what camera follows
//     camera.offset = (Vector2){ screenWidth/2.0f, screenHeight/2.0f };
//     camera.rotation = 0.0f;
//     camera.zoom = 1.0f;

//     while (!WindowShouldClose())
//     {
//         // Movement
//         if (IsKeyDown(KEY_RIGHT)) position.x += 3;
//         if (IsKeyDown(KEY_LEFT))  position.x -= 3;
//         if (IsKeyDown(KEY_UP))    position.y -= 3;
//         if (IsKeyDown(KEY_DOWN))  position.y += 3;

//         // Update camera target (FOLLOW PLAYER)
//         camera.target = position;

//         BeginDrawing();
//         ClearBackground(RAYWHITE);

//         BeginMode2D(camera);

//         // 🧍 character (simple rectangle for demo)
//         DrawRectangle(position.x - 20, position.y - 20, 40, 40, BLUE);

//         // 🧱 test world (so you see camera movement)
//         DrawRectangle(200, 200, 100, 100, RED);
//         DrawRectangle(600, 400, 120, 120, GREEN);
//         DrawLine(0, 0, 1000, 1000, BLACK);

//         EndMode2D();

//        // DrawText("Camera following player", 10, 10, 20, DARKGRAY);
//         DrawFPS(10,10);
//         EndDrawing();
//     }

//     CloseWindow();
//     return 0;
// }
#include "raylib.h"

#define TILE 32
#define W 20
#define H 15

int main(void)
{
    InitWindow(W * TILE, H * TILE, "Pokemon Style Grass Map");

    SetTargetFPS(60);

    // 0 = normal ground
    // 1 = tall grass (encounter grass)
    // 2 = path
    int map[H][W] =
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,0,0,0,0,2,0,0,0,1,1,1,1,1,1,1,1},
        {1,1,1,0,0,0,0,2,2,2,0,0,0,1,1,1,1,1,1,1},
        {1,1,0,0,0,0,2,2,2,2,2,0,0,0,1,1,1,1,1,1},
        {1,1,1,0,0,0,0,2,2,2,0,0,0,1,1,1,1,1,1,1},
        {1,1,1,1,0,0,0,0,2,0,0,0,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    };

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(SKYBLUE);

        for (int y = 0; y < H; y++)
        {
            for (int x = 0; x < W; x++)
            {
                Rectangle tile = { x * TILE, y * TILE, TILE, TILE };

                if (map[y][x] == 1)
                {
                    // 🌿 tall grass (Pokémon encounter grass feel)
                    DrawRectangleRec(tile, DARKGREEN);
                    
                    // little texture effect (makes it less flat)
                    DrawRectangle(x * TILE + 6, y * TILE + 6, 8, 8, GREEN);
                }
                else if (map[y][x] == 2)
                {
                    // 🟫 path
                    DrawRectangleRec(tile, BEIGE);
                }
                else
                {
                    // 🌱 normal ground
                    DrawRectangleRec(tile, LIME);
                }

                // optional grid (remove later)
                DrawRectangleLinesEx(tile, 1, Fade(BLACK, 0.2f));
            }
        }

        DrawText("Pokemon-style Tall Grass Map", 10, 10, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}