#include "raylib.h"
#include "collectible.h"
#include "inventory.h" // AddToInventory
#include <stdio.h>
#include <math.h>

// ---------------------------------------------------------------------------
// Global world database - matches the pattern used by map.c's static grids,
// but exposed (non-static) so main.c can pass it into the API below.
// ---------------------------------------------------------------------------
Collectible collectibleList[MAX_COLLECTIBLES];
int         collectibleCount = 0;

// Sprite sheet state. Kept file-static; nothing outside this file touches
// the texture directly.
static Texture2D collectibleTexture;
static bool      collectibleTextureLoaded = false;
static Rectangle collectibleFrames[COLLECTIBLE_TYPE_COUNT][COLLECTIBLE_ANIM_FRAMES];

// ---------------------------------------------------------------------------
// Fallback look, used only if "Assets&resources/potions.png" hasn't been
// added yet, so the feature is playable/testable immediately without art.
// Colors loosely match the reference mock (purple potion, blue super potion,
// cyan cursed energy elixir).
// ---------------------------------------------------------------------------
static Color GetCollectibleFallbackColor(CollectibleType type)
{
    switch (type)
    {
        case POTION_HEALTH_SMALL:  return (Color){ 170, 90, 200, 255 };
        case POTION_HEALTH_LARGE:  return (Color){ 60, 140, 220, 255 };
        case POTION_CURSED_ENERGY: return (Color){ 80, 210, 225, 255 };
        default:                   return WHITE;
    }
}

void InitCollectibleTextures(void)
{
    collectibleTexture = LoadTexture("Assets&resources/potions.png");
    collectibleTextureLoaded = (collectibleTexture.id != 0);

    if (!collectibleTextureLoaded)
    {
        TraceLog(LOG_WARNING,
            "COLLECTIBLE: 'Assets&resources/potions.png' not found - using placeholder pickup shapes. "
            "Add a %dx%d-per-frame sheet with %d rows (one per CollectibleType) and %d columns "
            "(animation frames) to enable real sprites.",
            COLLECTIBLE_SPRITE_SIZE, COLLECTIBLE_SPRITE_SIZE, COLLECTIBLE_TYPE_COUNT, COLLECTIBLE_ANIM_FRAMES);
        return;
    }

    for (int type = 0; type < COLLECTIBLE_TYPE_COUNT; type++)
    {
        for (int frame = 0; frame < COLLECTIBLE_ANIM_FRAMES; frame++)
        {
            collectibleFrames[type][frame] = (Rectangle){
                (float)(frame * COLLECTIBLE_SPRITE_SIZE),
                (float)(type * COLLECTIBLE_SPRITE_SIZE),
                (float)COLLECTIBLE_SPRITE_SIZE,
                (float)COLLECTIBLE_SPRITE_SIZE
            };
        }
    }
}

void UnloadCollectibleTextures(void)
{
    if (collectibleTextureLoaded)
    {
        UnloadTexture(collectibleTexture);
        collectibleTextureLoaded = false;
    }
}

// ---------------------------------------------------------------------------
// World placement. Coordinates below were hand-picked against mapoverlap[][]
// in map.h - all sit on tileID 0 (open ground), clear of every tile flagged
// blocked in collision.c's IsBlocked(), and inside the 1500x900 screen so
// nothing spawns off-view (this project draws directly to screen space,
// with no scrolling camera).
// ---------------------------------------------------------------------------
void InitCollectibles(Collectible list[], int *count)
{
    int n = 0;

    struct { int col; int row; CollectibleType type; } placements[] = {
        {  7, 7, POTION_HEALTH_SMALL  },
        {  9, 7, POTION_HEALTH_LARGE  },
        { 11, 7, POTION_CURSED_ENERGY },
        {  6, 8, POTION_HEALTH_SMALL  },
        { 10, 8, POTION_CURSED_ENERGY },
        { 14, 8, POTION_HEALTH_LARGE  },
    };

    int placementCount = (int)(sizeof(placements) / sizeof(placements[0]));

    for (int i = 0; i < placementCount && n < MAX_COLLECTIBLES; i++)
    {
        list[n].worldPos     = (Vector2){ (float)(placements[i].col * TILE_SIZE),
                                           (float)(placements[i].row * TILE_SIZE) };
        list[n].type         = placements[i].type;
        list[n].isPickedUp   = false;
        list[n].currentFrame = 0;
        list[n].frameTimer   = 0.0f;
        n++;
    }

    *count = n;
}

void UpdateCollectibles(Collectible *list, int count, float dt)
{
    for (int i = 0; i < count; i++)
    {
        if (list[i].isPickedUp) continue;

        list[i].frameTimer += dt;
        if (list[i].frameTimer >= COLLECTIBLE_FRAME_SPEED)
        {
            list[i].frameTimer = 0.0f;
            list[i].currentFrame++;
            if (list[i].currentFrame >= COLLECTIBLE_ANIM_FRAMES)
            {
                list[i].currentFrame = 0;
            }
        }
    }
}

void DrawCollectibles(Collectible *list, int count)
{
    for (int i = 0; i < count; i++)
    {
        if (list[i].isPickedUp) continue;

        // Small floating bob so items read as "alive"/interactive on the map.
        float   bob     = sinf((float)GetTime() * 3.0f + (float)i) * 4.0f;
        Vector2 drawPos = { list[i].worldPos.x, list[i].worldPos.y + bob };

        if (collectibleTextureLoaded)
        {
            Rectangle src = collectibleFrames[list[i].type][list[i].currentFrame];
            DrawTextureRec(collectibleTexture, src, drawPos, WHITE);
        }
        else
        {
            Rectangle body = { drawPos.x, drawPos.y,
                                (float)COLLECTIBLE_SPRITE_SIZE, (float)COLLECTIBLE_SPRITE_SIZE };
            DrawRectangleRounded(body, 0.35f, 6, GetCollectibleFallbackColor(list[i].type));
            DrawRectangleLinesEx(body, 2.0f, BLACK);
        }
    }
}

// ---------------------------------------------------------------------------
// RenderCollectibles
// ---------------------------------------------------------------------------
// Draws every non-picked-up potion using an array of individual textures:
//   potionTextures[0] = potion.png               (POTION_HEALTH_SMALL)
//   potionTextures[1] = super_potion.png         (POTION_HEALTH_LARGE)
//   potionTextures[2] = cursed_energy_elixir.png (POTION_CURSED_ENERGY)
//
// Each sprite is scaled to POTION_RENDER_SIZE x POTION_RENDER_SIZE on screen
// and centred over its 60-px world tile with a smooth sinusoidal float bob.
// ---------------------------------------------------------------------------
void RenderCollectibles(const Collectible *list, int count,
                        Texture2D potionTextures[COLLECTIBLE_TYPE_COUNT])
{
    // Destination size (px) on screen – fills the 60px tile nicely.
    const float DST = POTION_RENDER_SIZE;   // 48.0f

    // Centre a DST×DST box over the 60px tile:
    //   X: push right by (60 - DST) / 2 = 6px
    //   Y: push up   by 16px so the bottle base sits on the tile floor
    const float OFF_X = (TILE_SIZE - DST) / 2.0f;  // 6.0
    const float OFF_Y = 16.0f;

    // Fallback colours when a texture failed to load (impossible to miss)
    static const Color fallbackColors[COLLECTIBLE_TYPE_COUNT] = {
        { 170,  90, 200, 220 },   // purple  – POTION_HEALTH_SMALL
        {  60, 140, 220, 220 },   // blue    – POTION_HEALTH_LARGE
        {  80, 210, 225, 220 },   // cyan    – POTION_CURSED_ENERGY
    };

    for (int i = 0; i < count; i++)
    {
        if (list[i].isPickedUp) continue;

        int typeIdx = (int)list[i].type;

        // Guard against an out-of-range type value
        if (typeIdx < 0 || typeIdx >= COLLECTIBLE_TYPE_COUNT) continue;

        // Smooth sinusoidal bob – each item has its own phase so they don't
        // all move in lockstep.
        float bob  = sinf((float)GetTime() * 3.0f + (float)i * 1.3f) * 5.0f;

        float drawX = list[i].worldPos.x + OFF_X;
        float drawY = list[i].worldPos.y - OFF_Y + bob;

        Texture2D tex = potionTextures[typeIdx];

        if (tex.id != 0)
        {
            // Full-texture source rect (we have one PNG per potion, no atlas)
            Rectangle src = { 0.0f, 0.0f,
                              (float)tex.width, (float)tex.height };

            // Scale to DST×DST, centred over tile, with bob applied
            Rectangle dst = { drawX, drawY, DST, DST };

            DrawTexturePro(tex, src, dst, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else
        {
            // Coloured rounded rectangle fallback so missing textures are
            // immediately obvious in-game.
            Rectangle body = { drawX, drawY, DST, DST };
            DrawRectangleRounded(body, 0.4f, 8, fallbackColors[typeIdx]);
            DrawRectangleLinesEx(body, 2.0f, BLACK);
        }
    }
}




void CheckCollectibleCollisions(Collectible *list, int count, float playerX, float playerY, Inventory *inv)
{
    Rectangle playerBox = { playerX, playerY, PLAYER_COLLIDER_SIZE, PLAYER_COLLIDER_SIZE };

    for (int i = 0; i < count; i++)
    {
        if (list[i].isPickedUp) continue;

        Rectangle itemBox = { list[i].worldPos.x, list[i].worldPos.y,
                               (float)COLLECTIBLE_SPRITE_SIZE, (float)COLLECTIBLE_SPRITE_SIZE };

        if (CheckCollisionRecs(playerBox, itemBox))
        {
            list[i].isPickedUp = true;
            AddToInventory(inv, list[i].type);
            // NOTE: collectibles never set/consult IsBlocked() - they are
            // purely an overlap trigger and never obstruct player movement.
        }
    }
}

const char *GetCollectibleName(CollectibleType type)
{
    switch (type)
    {
        case POTION_HEALTH_SMALL:  return "Potion (10+ Health)";
        case POTION_HEALTH_LARGE:  return "Super Potion (50+ Health)";
        case POTION_CURSED_ENERGY: return "Cursed Energy Elixir (30+ CE)";
        default:                   return "Unknown Item";
    }
}
