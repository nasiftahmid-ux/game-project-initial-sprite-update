#include "raylib.h"
#include "raymath.h"
// #include"character.h"
#include "headerfiles/character.c"
#include "raygui.c"
#include "headerfiles/map.c"
// #include"map.h"
#include "headerfiles/collision.c"
// #include"raygui.h"
// const int screenWidth = 800;
#include <stdbool.h>
#include "headerfiles/enemyspawn.c"
#include "headerfiles/battle.c"
#include <time.h>
#include <stdlib.h>
// RANDOM ENCOUNTER FUNCTION
//====================================================

int main()
{

    InitWindow(screenWidth, screenHeight, "Jujutsu Kaisen - raylib (C)");
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));
    // Load texture
    Texture2D texture = LoadTexture("Assets&resources/player.png");
    Texture2D tileset = LoadTexture("Assets&resources/tiles.png");

    int frameWidth = texture.width / 4;
    int frameHeight = texture.height / 4;

    Rectangle frameRec = {0, 0, frameWidth, frameHeight};
    Vector2 position = {700, 400};

    int currentFrame = 0;
    int currentRow = 0; // 0=down,1=left,2=right,3=up

    float frameTime = 0.0f;
    float frameSpeed = 0.15f;
    // Battle STATE
    bool encounter = false;

    InitTileRects(tileRects);
    BattleScene battle;
    InitBattleScene(&battle, "NASIF", "ZARIF");

    while (!WindowShouldClose())
    {

        Vector2 nextPos = position;
        bool moving = false;
        if (!encounter)
        {
            charactermovement(&nextPos, &currentRow, frameWidth, frameHeight, TILE_SIZE, &moving);
        }

        enemyspawn(&nextPos, &encounter, moving, TILE_SIZE);

        float dt = GetFrameTime();

        if (!IsBattleOver(&battle))
        {
            UpdateBattleScene(&battle, dt);
        }

        // character hitbox corners
        collisionfunc(&nextPos, &position, frameWidth, frameHeight, TILE_SIZE);

        // Clamp position to screen bounds
        clampcharacter(&position, frameWidth, frameHeight);
        // Animation timing
        animation(&frameTime, &currentFrame, frameSpeed);

        // Update frame rectangle
        UpdateFrame(&frameRec, &currentFrame, &currentRow, frameWidth, frameHeight);

        BeginDrawing();
        ClearBackground(BLACK);

        // WHOLE MAP
        DRAWLAYERFIRST(tileset, tileRects, map, TILE_SIZE);
        DRAWLAYERsecond(tileset, tileRects, basemaps, TILE_SIZE);
        DRAWLAYERTHIRD(tileset, tileRects, maps, TILE_SIZE);
        DRAWLAYER4TH(tileset, tileRects, mapoverlap, TILE_SIZE);
        // Character
        DrawCharacter(texture, frameRec, position, frameWidth, frameHeight);
        DrawMinor(encounter, screenWidth, screenHeight);
        if (IsKeyDown(KEY_SPACE))
        {
            InitBattleScene(&battle, "NASIF", "ZARIF");
            while (1)
            {
                
                float dt = GetFrameTime();

                if (!IsBattleOver(&battle))
                {
                    UpdateBattleScene(&battle, dt);
                }

                BeginDrawing();
                if (!IsBattleOver(&battle))
                {
                    DrawBattleScene(&battle);
                }
                else
                {
                    ClearBackground(RAYWHITE);
                    // encounter=false;
                    break;
                    
                }
                EndDrawing();
            
            }
            
        }
        DrawFPS(10, 10);
        EndDrawing();
    }
    UnloadBattleScene(&battle);
    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
