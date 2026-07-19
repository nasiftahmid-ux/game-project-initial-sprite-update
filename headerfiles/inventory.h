#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>
#include "raylib.h"
#include "collectible.h"  // CollectibleType, COLLECTIBLE_TYPE_COUNT
#include "playerstats.h"  // PlayerStats — needed by UseItemFromInventory

// ---------------------------------------------------------------------------
// Capacity / grid layout constants
// ---------------------------------------------------------------------------
#define INVENTORY_MAX_SLOTS  20   // total bag cells (4 cols x 5 rows)
#define INV_GRID_COLS         4   // columns rendered by DrawInventoryMenu

// ---------------------------------------------------------------------------
// One stack of a single item type.
// quantity == 0  means the slot is empty/unused.
// ---------------------------------------------------------------------------
typedef struct InventoryItem
{
    CollectibleType type;
    int             quantity;
} InventoryItem;

// ---------------------------------------------------------------------------
// Fixed-capacity bag.
// Slots are always packed at the front — no holes (see RemoveFromInventory).
// ---------------------------------------------------------------------------
typedef struct Inventory
{
    InventoryItem slots[INVENTORY_MAX_SLOTS];
    int           usedSlots;
} Inventory;

// ---------------------------------------------------------------------------
// Core bag management  (unchanged from original)
// ---------------------------------------------------------------------------
void InitInventory(Inventory *inv);
bool AddToInventory(Inventory *inv, CollectibleType type);
bool RemoveFromInventory(Inventory *inv, CollectibleType type, int amount);
int  GetInventoryItemCount(Inventory *inv, CollectibleType type);
bool IsInventoryFull(Inventory *inv);

// ---------------------------------------------------------------------------
// Sidebar HUD  (small text list, always visible in MODE_OVERWORLD)
// ---------------------------------------------------------------------------
void DrawInventoryHUD(Inventory *inv, int x, int y);

// ---------------------------------------------------------------------------
// UseItemFromInventory
// ---------------------------------------------------------------------------
// Consumes one item from slots[slotIndex] and applies its stat effect to
// *player*.  Returns false if the index is out of range or the slot is empty.
// On success the quantity is decremented; reaching 0 compacts the array so
// there are never any holes in the packed slot list.
//
//   POTION_HEALTH_SMALL  -> +10 HP   (capped at maxHp)
//   POTION_HEALTH_LARGE  -> +50 HP   (capped at maxHp)
//   POTION_CURSED_ENERGY -> +30 CE   (capped at maxCursedEnergy)
// ---------------------------------------------------------------------------
bool UseItemFromInventory(Inventory *inv, int slotIndex, PlayerStats *player);

// ---------------------------------------------------------------------------
// DrawInventoryMenu
// ---------------------------------------------------------------------------
// Full-screen bag overlay.  Call this LAST in the draw block so it renders
// on top of everything else.  Opened/closed by the KEY_I toggle in main.
//
// potionTextures[] must be indexed by CollectibleType:
//   [0] = potion.png              (POTION_HEALTH_SMALL)
//   [1] = super_potion.png        (POTION_HEALTH_LARGE)
//   [2] = cursed_energy_elixir.png(POTION_CURSED_ENERGY)
//
// Layout: 4-column × 5-row grid (20 cells = INVENTORY_MAX_SLOTS).
// Each cell shows the item icon scaled to 42x42, a quantity badge "xN", and
// a gold highlight frame on the selectedSlot cursor position.
// An info bar below the grid shows the selected item's name and effect.
// ---------------------------------------------------------------------------
void DrawInventoryMenu(const Inventory *inv, int selectedSlot,
                       Texture2D potionTextures[COLLECTIBLE_TYPE_COUNT]);

#endif // INVENTORY_H
