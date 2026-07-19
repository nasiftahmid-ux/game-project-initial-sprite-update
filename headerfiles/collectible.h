#ifndef COLLECTIBLE_H
#define COLLECTIBLE_H

#include "raylib.h"
#include <stdbool.h>
#include "map.h" // TILE_SIZE

// ---------------------------------------------------------------------------
// Tunables
// ---------------------------------------------------------------------------
#define MAX_COLLECTIBLES        32   // fixed-size world database capacity
#define COLLECTIBLE_TYPE_COUNT  3    // number of rows in the sprite sheet
#define COLLECTIBLE_ANIM_FRAMES 4    // number of columns (idle/shimmer anim)
#define COLLECTIBLE_FRAME_SPEED 0.12f
#define COLLECTIBLE_SPRITE_SIZE 40   // width/height (px) of ONE frame in the
                                     // sheet, and of the fallback pickup box

// ---------------------------------------------------------------------------
// Individual potion texture files (one PNG per CollectibleType).
//   Index 0  ->  POTION_HEALTH_SMALL  : "Assets&resources/potion.png"
//   Index 1  ->  POTION_HEALTH_LARGE  : "Assets&resources/super_potion.png"
//   Index 2  ->  POTION_CURSED_ENERGY : "Assets&resources/cursed_energy_elixir.png"
// Passed as Texture2D potionTextures[COLLECTIBLE_TYPE_COUNT] to RenderCollectibles().
// ---------------------------------------------------------------------------
#define POTION_RENDER_SIZE  48.0f    // destination draw size (px) on the 60px world tile

// Size (px) of the player's pickup hitbox in world space.
// This mirrors the size used to draw the player in character.c
// (DrawCharacter uses frameWidth/4.0f, frameHeight/4.0f as destRec size).
// If you change the player texture or its draw scale, update this to match.
#define PLAYER_COLLIDER_SIZE 57.0f

// ---------------------------------------------------------------------------
// Item identity. Extend this list (and the sprite sheet rows,
// COLLECTIBLE_TYPE_COUNT, and GetCollectibleName) to add new pickups.
// ---------------------------------------------------------------------------
typedef enum CollectibleType
{
    POTION_HEALTH_SMALL = 0, // "Potion"                (10+ Health)
    POTION_HEALTH_LARGE,     // "Super Potion"          (50+ Health)
    POTION_CURSED_ENERGY     // "Cursed Energy Elixir"  (30+ Cursed Energy)
} CollectibleType;

// ---------------------------------------------------------------------------
// A single pickup placed somewhere in the world.
// ---------------------------------------------------------------------------
typedef struct Collectible
{
    Vector2         worldPos;     // top-left world-space position
    CollectibleType type;
    bool            isPickedUp;   // true once collected - skipped by update/draw/collision
    int             currentFrame; // current animation column
    float           frameTimer;   // accumulator toward next frame
} Collectible;

// ---------------------------------------------------------------------------
// Forward declaration only - avoids a circular #include with inventory.h.
// Inventory is only ever used here as a pointer, so the incomplete type is
// sufficient for this header. inventory.h supplies the full definition.
// ---------------------------------------------------------------------------
#ifndef INVENTORY_TYPE_DEFINED
#define INVENTORY_TYPE_DEFINED
typedef struct Inventory Inventory;
#endif

// ---------------------------------------------------------------------------
// World database. Defined (not just declared) in collectible.c.
// Pass these two into the functions below from your main loop.
// ---------------------------------------------------------------------------
extern Collectible collectibleList[MAX_COLLECTIBLES];
extern int         collectibleCount;

// Texture / sprite-sheet lifecycle
void InitCollectibleTextures(void);
void UnloadCollectibleTextures(void);

// Populates list[] with the world's pickups and writes the count out.
void InitCollectibles(Collectible list[], int *count);

// Advances pickup idle/shimmer animation frames. Call once per frame.
void UpdateCollectibles(Collectible *list, int count, float dt);

// Draws every collectible that hasn't been picked up yet (internal fallback /
// animated sprite-sheet path).  Call this AFTER your ground/base map layers
// and BEFORE your top occlusion/overlay layer, so items sit on the ground
// but can be hidden behind tall foreground tiles (trees, cave walls, etc.).
void DrawCollectibles(Collectible *list, int count);

// ---------------------------------------------------------------------------
// RenderCollectibles
// ---------------------------------------------------------------------------
// Draws every non-picked-up potion using an array of per-type textures.
//
//   potionTextures[0] = potion.png              (POTION_HEALTH_SMALL)
//   potionTextures[1] = super_potion.png        (POTION_HEALTH_LARGE)
//   potionTextures[2] = cursed_energy_elixir.png(POTION_CURSED_ENERGY)
//
// Each sprite is scaled to POTION_RENDER_SIZE x POTION_RENDER_SIZE px and
// centred over its 60px world tile with a sinusoidal floating bob.
//
// Draw order in MAINGAME.c:
//   DRAWLAYER4TH(...)          <- overlay last
//   RenderCollectibles(...)    <- potions on top of overlay
//   DrawCharacter(...)         <- player on top of everything
void RenderCollectibles(const Collectible *list, int count,
                        Texture2D potionTextures[COLLECTIBLE_TYPE_COUNT]);

// Player-vs-item overlap check. On a hit: marks the item picked up and
// adds it to the inventory. Collectibles never block movement - this is
// purely a pickup trigger, run independently of your tile collision.
void CheckCollectibleCollisions(Collectible *list, int count, float playerX, float playerY, Inventory *inv);

// Human-readable name/description, used by the inventory HUD.
const char *GetCollectibleName(CollectibleType type);

#endif // COLLECTIBLE_H
