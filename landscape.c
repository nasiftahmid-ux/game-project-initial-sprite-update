#include "raylib.h"
#include <stdlib.h>
#include <time.h>

// Tile settings
#define TILE_SIZE 64
#define MAP_WIDTH 1000
#define MAP_HEIGHT 6000

// Tile types
#define GRASS 0
#define WATER 1
#define TREE 2
#define PATH 3

int map[MAP_HEIGHT][MAP_WIDTH];

// Generate random world
void GenerateWorld()
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int r = rand() % 100;

            if (r < 70)
                map[y][x] = GRASS;

            else if (r < 80)
                map[y][x] = TREE;

            else if (r < 90)
                map[y][x] = PATH;

            else
                map[y][x] = WATER;
        }
    }
}
