#include "raylib.h"

void DrawCharacter(Texture2D texture,Rectangle frameRec,Vector2 position,int frameWidth,int frameHeight);
void charactermovement(Vector2  *position,int *currentRow);
void clampcharacter(Vector2  *position,int frameWidth,int frameHeight);
void animation(float *frameTime,int *currentFrame,float frameSpeed);
void UpdateFrame(Rectangle *frameRec,int *currentFrame,int *currentRow,int frameWidth,int frameHeight);