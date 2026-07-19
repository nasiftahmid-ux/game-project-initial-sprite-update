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
#include "headerfiles/gamemode.h"
#include "headerfiles/playerstats.c"
#include "headerfiles/collectible.c"
#include "headerfiles/inventory.c"
// RANDOM ENCOUNTER FUNCTION
//====================================================

int main()
{

    InitWindow(screenWidth, screenHeight, "Jujutsu Kaisen - raylib (C)");
    Image icon=LoadImage("Assets&resources/jjk2.png");
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetTargetFPS(60);
    srand((unsigned int)time(NULL));
    // Load texture
    Texture2D texture = LoadTexture("Assets&resources/gojo normal walk.png");
    Texture2D tileset = LoadTexture("Assets&resources/tiles.png");

    // Individual potion textures – one PNG per CollectibleType.
    // Files are already in Assets&resources/ (potion.png, super_potion.png,
    // cursed_energy_elixir.png) so no sprite-sheet assembly is needed.
    Texture2D potionTextures[COLLECTIBLE_TYPE_COUNT];
    potionTextures[POTION_HEALTH_SMALL]  = LoadTexture("Assets&resources/potion.png");
    potionTextures[POTION_HEALTH_LARGE]  = LoadTexture("Assets&resources/super_potion.png");
    potionTextures[POTION_CURSED_ENERGY] = LoadTexture("Assets&resources/cursed_energy_elixir.png");

    int frameWidth = texture.width / 4;
    int frameHeight = texture.height / 4;

    Rectangle frameRec = {0, 0, frameWidth, frameHeight};
    Vector2 position = {700, 400};

    int currentFrame = 0;
    int currentRow = 0; // 0=down,1=left,2=right,3=up

    float frameTime = 0.0f;
    float frameSpeed = 0.15f;
    
    bool encounter = false;

    InitTileRects(tileRects);
    BattleScene battle;
    GameMode mode;
    bool isMenuOpen   = false;  // KEY_I toggles the bag screen
    int  selectedSlot = 0;      // cursor position inside the bag grid

    PlayerStats playerstats;
    // int playerMaxHp = 100;
    // int playerCurrentHp = 100;
    InitPlayerStats(&playerstats,"NASIF");

    Inventory playerInventory;
    InitInventory(&playerInventory);
    InitCollectibleTextures();
    InitCollectibles(collectibleList, &collectibleCount);

    while (!WindowShouldClose())
    {

        

        float dt = GetFrameTime();

        if (mode == MODE_OVERWORLD)
        {
            Vector2 nextPos = position;
            bool moving = false;
            // ---- KEY_I: toggle bag menu ----
            if (IsKeyPressed(KEY_I))
            {
                isMenuOpen  = !isMenuOpen;
                selectedSlot = 0;   // reset cursor on each open
            }

            if (isMenuOpen)
            {
                // Route all directional input to the bag grid cursor.
                // Wrapping on left/right; clamping on up/down (grid rows).
                int total = playerInventory.usedSlots;
                if (total > 0)
                {
                    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
                        selectedSlot = (selectedSlot + 1) % total;
                    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A))
                        selectedSlot = (selectedSlot - 1 + total) % total;
                    if (IsKeyPressed(KEY_DOWN)  || IsKeyPressed(KEY_S))
                    {
                        int nxt = selectedSlot + INV_GRID_COLS;
                        if (nxt < total) selectedSlot = nxt;
                    }
                    if (IsKeyPressed(KEY_UP)    || IsKeyPressed(KEY_W))
                    {
                        int prv = selectedSlot - INV_GRID_COLS;
                        if (prv >= 0) selectedSlot = prv;
                    }

                    // ENTER or SPACE: consume the highlighted item
                    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
                    {
                        UseItemFromInventory(&playerInventory, selectedSlot, &playerstats);
                        // If the used slot was depleted and compacted, clamp cursor
                        if (selectedSlot >= playerInventory.usedSlots &&
                            playerInventory.usedSlots > 0)
                            selectedSlot = playerInventory.usedSlots - 1;
                    }
                }
            }
            else if (!encounter)
            {
                charactermovement(&nextPos, &currentRow, frameWidth, frameHeight, TILE_SIZE, &moving);
            }

            enemyspawn(&nextPos, &encounter, moving, TILE_SIZE);
            // character hitbox corners
            collisionfunc(&nextPos, &position, frameWidth, frameHeight, TILE_SIZE);

            // Clamp position to screen bounds
            clampcharacter(&position, frameWidth, frameHeight);
            // Animation timing
            animation(&frameTime, &currentFrame, frameSpeed);

            // Update frame rectangle
            UpdateFrame(&frameRec, &currentFrame, &currentRow, frameWidth, frameHeight);

            // Collectibles: animate, then check pickup against the resolved position
            UpdateCollectibles(collectibleList, collectibleCount, dt);
            CheckCollectibleCollisions(collectibleList, collectibleCount, position.x, position.y, &playerInventory);

            // Start battle only when menu is closed (SPACE is also used to use items)
            if (encounter && !isMenuOpen && IsKeyPressed(KEY_SPACE))
            {
                InitBattleScene(&battle,
                    "NASIF", "Assets&resources/gojo-nasif.png", 4, 3,
                    "ZARIF", "Assets&resources/geto-nasif.png", 4, 3
                );
                battle.player.maxHp =playerstats.maxHp;
                battle.player.currentHp = playerstats.currentHp;
                battle.player.displayedHp = playerstats.currentHp;
                mode = MODE_BATTLE;
            }
        }
        else if (mode == MODE_BATTLE)
        {
            if (!IsBattleOver(&battle))
            {
                UpdateBattleScene(&battle, dt);
            }
            else
            {
                 playerstats.currentHp = battle.player.currentHp;
                 playerstats.maxHp=battle.player.maxHp;
                 if(battle.won)
                 {
                    GainExp(&playerstats,battle.rewardExp);
                 }
                encounter = false;
                mode = MODE_OVERWORLD;
            }
        }

       

        BeginDrawing();
        ClearBackground(BLACK);

        if (mode == MODE_OVERWORLD)
        {
            DRAWLAYERFIRST(tileset, tileRects, map, TILE_SIZE);
            DRAWLAYERsecond(tileset, tileRects, basemaps, TILE_SIZE);
            DRAWLAYERTHIRD(tileset, tileRects, maps, TILE_SIZE);
            DRAWLAYER4TH(tileset, tileRects, mapoverlap, TILE_SIZE);

            // Potions render AFTER all map layers so the overlay tiles (trees,
            // walls, etc.) cannot bury them. Items float with a sinusoidal bob.
            RenderCollectibles(collectibleList, collectibleCount, potionTextures);

            // Character always on top of items
            DrawCharacter(texture, frameRec, position, frameWidth, frameHeight);

            
           DrawMinor(encounter, screenWidth, screenHeight);
           DrawPlayerHud(&playerstats, 1140, 20);
           // InventoryHUD moved down to clear the new CE bar in DrawPlayerHud
           DrawInventoryHUD(&playerInventory, 1140, 130);

           // Full-screen bag overlay – drawn absolutely last so it sits on top
           if (isMenuOpen)
               DrawInventoryMenu(&playerInventory, selectedSlot, potionTextures);

        }
        
        else if (mode == MODE_BATTLE)
        { 
            
            DrawBattleScene(&battle);
        }
        
        DrawFPS(10, 10);
        EndDrawing();
    }
    UnloadBattleScene(&battle);
    UnloadTexture(texture);
    UnloadTexture(potionTextures[POTION_HEALTH_SMALL]);
    UnloadTexture(potionTextures[POTION_HEALTH_LARGE]);
    UnloadTexture(potionTextures[POTION_CURSED_ENERGY]);
    UnloadCollectibleTextures();
    CloseWindow();

    return 0;
}