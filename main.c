#include "raylib.h"
#include "raymath.h"
#include"landscape.c"
#include"raygui.c"
//#include"raygui.h"
const int screenWidth = 800;
   
const int screenHeight = 600;

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
    srand(time(NULL));

    GenerateWorld();
    
   
    while (!WindowShouldClose())
    {
        
        if (IsKeyDown(KEY_RIGHT))
        {
            position.x += 2;
            currentRow = 3;
        }
        else if (IsKeyDown(KEY_LEFT))
        {
            position.x -= 2;
            currentRow = 2;
        }
        else if (IsKeyDown(KEY_UP))
        {
            position.y -= 2;
            currentRow = 1;
        }
        else if (IsKeyDown(KEY_DOWN))
        {
            position.y += 2;
            currentRow = 0;
        }
        
        // Clamp position to screen bounds
        position.x = Clamp(position.x, 0, screenWidth - frameWidth+50);
        position.y = Clamp(position.y, 0, screenHeight-frameHeight+90 );   
        

        // Animation timing
        if(IsKeyDown(KEY_UP) ||IsKeyDown(KEY_DOWN)||IsKeyDown(KEY_LEFT)||IsKeyDown(KEY_RIGHT))
        {
            frameTime += GetFrameTime();
            if (frameTime >= frameSpeed)
            {
                frameTime = 0.0f;
                currentFrame++;
                if (currentFrame > 3) currentFrame = 0;
            }
        }

        // Update frame rectangle
        frameRec.x = currentFrame * frameWidth;
        frameRec.y = currentRow * frameHeight;
        

        BeginDrawing();
        ClearBackground(BLACK);
       
        Rectangle destRec = { position.x, position.y, frameWidth/2.0f, frameHeight/2.0f };
        Vector2 origin = { 0, 0 };

        DrawTexturePro(texture, frameRec, destRec, origin, 0.0f, WHITE);
        
       
        DrawFPS(10,10);
        EndDrawing();
    }

    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
