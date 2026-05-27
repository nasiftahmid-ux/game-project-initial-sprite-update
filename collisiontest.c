#include"raylib.h"
#include<stdio.h>
#include"collision.h"


void collisionfunc(Vector2 *nextPos,Vector2 *position,int frameWidth,int frameHeight,int tilesize)
{
float scale = 3.0f;
    float playerWidth = frameWidth / scale;
    float playerHeight = frameHeight / scale;
    int leftTile   = nextPos->x / (tilesize);
    int rightTile  = (nextPos->x + playerWidth - 1) / (tilesize);
    int topTile    = nextPos->y / (tilesize);
    int bottomTile = (nextPos->y + playerHeight - 1) / (tilesize);
    if (
    !IsBlocked(leftTile, topTile) &&
    !IsBlocked(rightTile, topTile) &&
    !IsBlocked(leftTile, bottomTile) &&
    !IsBlocked(rightTile, bottomTile)
)
{
    *position = *nextPos;
}



}
int IsBlocked(int x, int y)
{
    // boundary check
    // if (x < 0 || y < 0 || x >= 30 || y >= 20)
    //     return 1;

    if(mapoverlap[y][x] == 11 || mapoverlap[y][x] == 11)
    {
        return 1;
    }
    return 0;
}