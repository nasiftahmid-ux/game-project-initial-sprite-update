#include "playerstats.h"
#include <string.h>
#include <stdio.h>
#define BASE_EXP_TO_LEVEL 100
#define baseexp 0;
#define HP_GAIN_PER_LEVEL 10

void InitPlayerStats(PlayerStats *stats, const char *name)
{
    strncpy(stats->name, name, sizeof(stats->name) - 1);
    stats->name[sizeof(stats->name) - 1] = '\0';
    stats->level          = 1;
    stats->maxHp          = 100;
    stats->currentHp      = 100;
    stats->currentExp     = 0;
    stats->expToNextLevel = BASE_EXP_TO_LEVEL;
    stats->cursedEnergy    = 100;   // start full
    stats->maxCursedEnergy = 100;
}
void GainExp(PlayerStats *stats,int amount)
{
    stats->currentExp+=amount;
    //WE use while loop cuz Level Might Increase Based on EXP eARNED 
    while(stats->currentExp>=stats->expToNextLevel)
    {
        stats->currentExp -=stats->expToNextLevel;
        stats->level++;
        stats->maxHp+=HP_GAIN_PER_LEVEL;
        stats->currentHp=stats->maxHp;
        stats->expToNextLevel=stats->level*baseexp;
    }
}
void DrawPlayerHud(PlayerStats *stats, int x, int y)
{
    char namelevel[64];
    snprintf(namelevel, sizeof(namelevel), "%s  Lv:%d", stats->name, stats->level);
    DrawText(namelevel, x, y, 20, BLACK);

    int BarWidth = 220, BarHeight = 18;

    // ---- HP bar ----
    float hpPct = (stats->maxHp > 0) ? (float)stats->currentHp / (float)stats->maxHp : 0.0f;
    if (hpPct < 0.0f) hpPct = 0.0f;
    if (hpPct > 1.0f) hpPct = 1.0f;
    DrawRectangle(x, y + 24, BarWidth, BarHeight, BLACK);
    Color hpColor = GREEN;
    if (hpPct < 0.5f) hpColor = YELLOW;
    if (hpPct < 0.2f) hpColor = RED;
    DrawRectangle(x, y + 24, (int)(BarWidth * hpPct), BarHeight, hpColor);
    DrawRectangleLines(x, y + 24, BarWidth, BarHeight, BLACK);
    char hptext[32];
    snprintf(hptext, sizeof(hptext), "HP %d/%d", stats->currentHp, stats->maxHp);
    DrawText(hptext, x, y + 44, 11, DARKGRAY);

    // ---- EXP bar ----
    float expPct = (stats->expToNextLevel > 0)
        ? (float)stats->currentExp / (float)stats->expToNextLevel : 0.0f;
    if (expPct < 0.0f) expPct = 0.0f;
    if (expPct > 1.0f) expPct = 1.0f;
    DrawRectangle(x, y + 58, BarWidth, 10, DARKGRAY);
    DrawRectangle(x, y + 58, (int)(BarWidth * expPct), 10, SKYBLUE);
    DrawRectangleLines(x, y + 58, BarWidth, 10, BLACK);
    char exptext[32];
    snprintf(exptext, sizeof(exptext), "EXP %d/%d", stats->currentExp, stats->expToNextLevel);
    DrawText(exptext, x, y + 70, 11, DARKGRAY);

    // ---- Cursed Energy bar ----
    float cePct = (stats->maxCursedEnergy > 0)
        ? (float)stats->cursedEnergy / (float)stats->maxCursedEnergy : 0.0f;
    if (cePct < 0.0f) cePct = 0.0f;
    if (cePct > 1.0f) cePct = 1.0f;
    DrawRectangle(x, y + 84, BarWidth, 10, DARKGRAY);
    DrawRectangle(x, y + 84, (int)(BarWidth * cePct), 10, (Color){ 80, 210, 225, 255 });
    DrawRectangleLines(x, y + 84, BarWidth, 10, BLACK);
    char cetext[32];
    snprintf(cetext, sizeof(cetext), "CE  %d/%d", stats->cursedEnergy, stats->maxCursedEnergy);
    DrawText(cetext, x, y + 96, 11, (Color){ 60, 190, 210, 255 });
}