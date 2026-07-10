#include "raylib.h"
//#include "battle.h"
#include "headerfiles/battle.c"
#include <time.h>
#include <stdlib.h>

int main(void)
{
    srand((unsigned int)time(NULL));

    InitWindow(800, 600, "Battle Test - Part 3");
    SetTargetFPS(60);

    BattleScene battle;
    InitBattleScene(&battle, "Hero", "Goblin");

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!IsBattleOver(&battle)) {
            UpdateBattleScene(&battle, dt);
        }

        BeginDrawing();
            if (!IsBattleOver(&battle)) {
                DrawBattleScene(&battle);
            } else {
                ClearBackground(RAYWHITE);
                DrawText("Battle finished - back to overworld here", 150, 280, 20, DARKGRAY);
            }
        EndDrawing();
    }

    UnloadBattleScene(&battle);
    CloseWindow();
    return 0;
}