#include <windows.h>
#include <dwmapi.h>
#include "raylib.h"

int main(void)
{
    InitWindow(800, 600, "My Game");

    HWND hwnd = GetWindowHandle();

    BOOL dark = TRUE;
    DwmSetWindowAttribute(
        hwnd,
        DWMWA_USE_IMMERSIVE_DARK_MODE,
        &dark,
        sizeof(dark)
    );

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
}