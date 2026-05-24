#include "raylib.h"
#include "raymath.h"
#include"landscape.c"
#include"raygui.c"
//#include"raygui.h"
const int screenWidth = 800;
   
const int screenHeight = 600;
void DrawCharacter(Texture2D texture,Rectangle frameRec,Vector2 position,int frameWidth,int frameHeight);
void charactermovement(Vector2  *position,int *currentRow);
void clampcharacter(Vector2  *position,int frameWidth,int frameHeight);
void animation(float *frameTime,int *currentFrame,float frameSpeed);
void UpdateFrame(Rectangle *frameRec,int *currentFrame,int *currentRow,int frameWidth,int frameHeight);
int main(void)
{
    

    InitWindow(screenWidth, screenHeight, "Pokemon Style Character - raylib (C)");
    SetTargetFPS(144);

    // Load texture
    Texture2D texture = LoadTexture("player.png");

    int frameWidth = texture.width / 4;
    int frameHeight = texture.height / 4;

    Rectangle frameRec = { 0, 0, frameWidth, frameHeight };
    Vector2 position = { 400, 300 };
  

    int currentFrame = 0;
    int currentRow = 0; // 0=down,1=left,2=right,3=up

    float frameTime = 0.0f;
    float frameSpeed = 0.15f;
   

    
    
   
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
       
        DrawCharacter(texture, frameRec,position,frameWidth,frameHeight);
        
       
        DrawFPS(10,10);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
void DrawCharacter(Texture2D texture,Rectangle frameRec,Vector2 position,int frameWidth,int frameHeight)
{
    Rectangle destRec = { position.x, position.y, frameWidth/2.0f, frameHeight/2.0f };
    Vector2 origin = {0, 0};
    DrawTexturePro(texture, frameRec, destRec, origin, 0.0f, WHITE);    
}
void charactermovement(Vector2  *position,int *currentRow)
{
    if (IsKeyDown(KEY_RIGHT))
        {
            position->x += 2;
            *currentRow = 3;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            position->x -= 2;
            *currentRow = 2;
        }
        else if (IsKeyDown(KEY_UP))
        {
            position->y -= 2;
            *currentRow = 1;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            position->y += 2;
            *currentRow = 0;
        }
}
void clampcharacter(Vector2  *position,int frameWidth,int frameHeight)
{
    position->x = Clamp(position->x, 0, screenWidth - frameWidth+50);
    position->y = Clamp(position->y, 0, screenHeight-frameHeight+90 ); 
}
void animation(float *frameTime,int *currentFrame,float frameSpeed)
{
    if(IsKeyDown(KEY_UP) ||IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_RIGHT))
        {
            *frameTime += GetFrameTime();
            if (*frameTime >= frameSpeed)
            {
                *frameTime = 0.0f;
                (*currentFrame)++;
                if (*currentFrame > 3) 
                {
                    *currentFrame = 0;
                }
            }
        }
}
void UpdateFrame(Rectangle *frameRec,int *currentFrame,int *currentRow,int frameWidth,int frameHeight)
{
    frameRec->x = (*currentFrame) * frameWidth;
    frameRec->y = (*currentRow) * frameHeight;
        
}

