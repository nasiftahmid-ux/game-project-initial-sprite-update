#include "raylib.h"
#include "raymath.h"
#include"character.h"
#include"character.c"
#include"raygui.c"
#include"map.c"
#include"map.h"
//#include"raygui.h"
//const int screenWidth = 800;
   

int main(void)
{
    

    InitWindow(screenWidth, screenHeight, "Pokemon Style Character - raylib (C)");
    SetTargetFPS(144);

    // Load texture
    Texture2D texture = LoadTexture("player.png");
    Texture2D tileset = LoadTexture("tiles.png");

    int frameWidth = texture.width / 4;
    int frameHeight = texture.height / 4;

    Rectangle frameRec = { 0, 0, frameWidth, frameHeight };
    Vector2 position = { 400, 300 };
  

    int currentFrame = 0;
    int currentRow = 0; // 0=down,1=left,2=right,3=up

    float frameTime = 0.0f;
    float frameSpeed = 0.15f;
   
    InitTileRects(tileRects);
    
    
   
    while (!WindowShouldClose())
    {
        
        
        //CHARACTER  MOVEMENT
        charactermovement(&position,&currentRow);
        // Clamp position to screen bounds
        clampcharacter(&position, frameWidth,frameHeight);
        // Animation timing
        animation(&frameTime,&currentFrame,frameSpeed);
        

        // Update frame rectangle
        UpdateFrame(&frameRec,&currentFrame,&currentRow,frameWidth,frameHeight);
        
        

        BeginDrawing();
        ClearBackground(BLACK);
       

        //WHOLE MAP
        DRAWLAYERFIRST(tileset,tileRects,map,TILE_SIZE);
        DRAWLAYERsecond(tileset,tileRects,basemaps,TILE_SIZE);
        DRAWLAYERTHIRD(tileset,tileRects,maps,TILE_SIZE);
        DRAWLAYER4TH(tileset,tileRects,mapoverlap,TILE_SIZE);
        //Character
        DrawCharacter(texture, frameRec,position,frameWidth,frameHeight);
       
        DrawFPS(10,10);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
