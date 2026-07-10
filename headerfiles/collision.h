#include"raylib.h"
#include"map.h"


int IsBlocked(int x, int y);
void collisionfunc(Vector2 *nextPos,Vector2 *position,int frameWidth,int frameHeight,int tilesize);