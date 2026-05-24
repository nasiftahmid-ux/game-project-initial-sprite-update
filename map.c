#include "raylib.h"
#include "map.h"

#define TILE_SIZE 16

// Tile selector
// #define TILE(col,row) (Rectangle){col*16,row*16,16,16}

// Map size
#define MAP_WIDTH 30
#define MAP_HEIGHT 20

int main(void)
{
    InitWindow(800, 450, "Tilemap Example");

    // Load tileset
    Texture2D tileset = LoadTexture("tiles.png");

    // MAP DATA
    // numbers = tile IDs

    // Tile locations in tileset
    

    tileRects[0] = TILE(0, 0);   /// sand
    tileRects[1] = TILE(7, 3);   // grass
    tileRects[2] = TILE(1, 4);   // Mountain
    tileRects[3] = TILE(8, 0);   // wall
    tileRects[4] = TILE(0, 9);   // SEA
    tileRects[5] = TILE(7, 1);   // WATERFALL
    tileRects[6] = TILE(6, 3);   // WHITE STUFF
    tileRects[7] = TILE(13, 3);  // CLEAN POND BOUNDARY
    tileRects[8] = TILE(14, 3);  // cLEAN pond Boundary 2
    tileRects[9] = TILE(13, 2);  // CLEAR PONDS
    tileRects[10] = TILE(14, 2); // SIDE BORDER OF POND
    tileRects[11] = TILE(3, 0);  // ROck
    tileRects[12] = TILE(1, 5);  // DARK CAVE BORDER
    tileRects[13] = TILE(0, 4);  // DARK CAVE BORDER side
    tileRects[14] = TILE(0, 5);  // DARK CAVE BORDER curve
    tileRects[15] = TILE(7, 0);  // DOOR
    tileRects[16] = TILE(10, 5); // sea edge
    tileRects[17] = TILE(11, 5); // corner sea
    tileRects[18] = TILE(10, 6); // middlesea edge
    tileRects[19] = TILE(11, 6); // side sea edge
    tileRects[20] = TILE(9, 1);  // QUICK SAND SIDE EDGE
    tileRects[21] = TILE(10, 1); // upper quick sand edge
    tileRects[22] = TILE(1, 5);  // pahar
    tileRects[23] = TILE(2, 5);  // pahar er kona
    tileRects[24] = TILE(2, 4);  // pahar er  kona
    tileRects[25] = TILE(3, 4);  // onno pahar er side
    tileRects[26] = TILE(3, 5);  // curve of onno pahar
    tileRects[27] = TILE(4, 4);  // onno pahar
    tileRects[28] = TILE(4, 5);  // onno  pahar front side
    tileRects[29] = TILE(0, 3);  // pahar    first curve
    tileRects[30] = TILE(1, 3);  // paharu uperface
    tileRects[31] = TILE(5, 1);  // BUSH
    tileRects[32] = TILE(9, 2);  // QUICK SAND SIDE LAYER
    tileRects[33] = TILE(10, 2); // qUICK SAND MAIN
    tileRects[34] = TILE(5, 0);  // Sign board
    tileRects[35] = TILE(5, 2);  // SMALL CACTUS

    // ELEMENT LOCATION  IN TILESET

    tileRects[36] = TILE(4, 2); // CACTUS LONG
    tileRects[37] = TILE(4, 1); // cactus long
    tileRects[38] = TILE(4, 0); // CACTUS LONG
    tileRects[39] = TILE(1, 2); // LONG TREE
    tileRects[40] = TILE(1, 1); // LONG TREE
    tileRects[41] = TILE(1, 0); // LONG TREEE
    tileRects[42] = TILE(2, 0); // LONG TREE
    tileRects[43] = TILE(2, 1); // LONG TTREE
    tileRects[44] = TILE(2, 2); // LONG TREE
    tileRects[45] = TILE(3, 1); // SMALL TREE
    tileRects[46] = TILE(3, 2); // SMALL TREE
    tileRects[47] = TILE(8, 0); // butterfly

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);

        // DRAW MAP
        for (int y = 0; y < MAP_HEIGHT; y++)
        {
            for (int x = 0; x < MAP_WIDTH; x++)
            {
                // tile id from map
                int tileID = map[y][x];

                // draw tile
                DrawTexturePro(
                    tileset,

                    // source tile
                    tileRects[tileID],

                    // destination on screen
                    (Rectangle){
                        x * TILE_SIZE * 2,
                        y * TILE_SIZE * 2,

                        TILE_SIZE * 2,
                        TILE_SIZE * 2},

                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
        }
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 30; x++)
            {
                // tile id from map
                int tileID = basemaps[y][x];

                // draw tile
                DrawTexturePro(
                    tileset,

                    // source tile
                    tileRects[tileID],

                    // destination on screen
                    (Rectangle){
                        x * TILE_SIZE * 2,
                        y * TILE_SIZE * 2,

                        TILE_SIZE * 2,
                        TILE_SIZE * 2},

                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
        }
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 30; x++)
            {
                // tile id from map
                int tileID = maps[y][x];

                // draw tile
                DrawTexturePro(
                    tileset,

                    // source tile
                    tileRects[tileID],

                    // destination on screen
                    (Rectangle){
                        x * TILE_SIZE * 2,
                        y * TILE_SIZE * 2,

                        TILE_SIZE * 2,
                        TILE_SIZE * 2},

                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
        }
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 30; x++)
            {
                // tile id from map
                int tileID = mapoverlap[y][x];

                // draw tile
                DrawTexturePro(
                    tileset,

                    // source tile
                    tileRects[tileID],

                    // destination on screen
                    (Rectangle){
                        x * TILE_SIZE * 2,
                        y * TILE_SIZE * 2,

                        TILE_SIZE * 2,
                        TILE_SIZE * 2},

                    (Vector2){0, 0},
                    0.0f,
                    WHITE);
            }
        }

        DrawText("Tilemap Example", 10, 10, 20, WHITE);

        EndDrawing();
    }

    UnloadTexture(tileset);

    CloseWindow();

    return 0;
}