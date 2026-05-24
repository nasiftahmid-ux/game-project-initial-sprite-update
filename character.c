#include"raylib.h"
#include<stdio.h>
#include"character.h"
const int screenWidth = 800;
   
const int screenHeight = 450;
void DrawCharacter(Texture2D texture,Rectangle frameRec,Vector2 position,int frameWidth,int frameHeight)
{
    Rectangle destRec = { position.x, position.y, frameWidth/3.0f, frameHeight/3.0f };
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

