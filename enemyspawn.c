#include "raylib.h"
#include <stdbool.h>
#include "enemyspawn.h"
void enemyspawn(Vector2 *nextPos,bool *encounter,bool moving,int tilesize)
{
    int tileX = nextPos->x / tilesize;
        int tileY = nextPos->y /tilesize;

     if (tileX >= 0 && tileX < 30 &&
            tileY >= 0 && tileY < 20)
        {
            //------------------------------------------------
            // CHECK RANDOM ENCOUNTER
            //------------------------------------------------

            if (CheckPokemonEncounter(mapoverlap[tileY][tileX],
                                      moving))
            {
                *encounter = true;
            }
        }

        //ENCOUNTER RESET
    if (*encounter && IsKeyPressed(KEY_ENTER))
        {
            *encounter = false;
        }
}
bool CheckPokemonEncounter(int currentTile, bool isMoving)
{
    // Grass tile
    if (currentTile == 0 && isMoving)
    {
        // Random encounter chance
        int randomChance = GetRandomValue(0, 500);

        // Encounter trigger
        if (randomChance == 6)
        {
            return true;
        }
    }

    return false;
}
