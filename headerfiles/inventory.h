#ifndef INVENTORY_H
#define INVENTORY_H

#include <stdbool.h>
#include "collectible.h" // CollectibleType lives here - Inventory stores whatever Collectible defines

// ---------------------------------------------------------------------------
// Inventory capacity
// ---------------------------------------------------------------------------
#define INVENTORY_MAX_SLOTS 20

// ---------------------------------------------------------------------------
// A single inventory slot: an item identity + how many the player is holding.
// quantity == 0 means the slot is empty/unused.
// ---------------------------------------------------------------------------
typedef struct InventoryItem
{
    CollectibleType type;
    int             quantity;
} InventoryItem;

// ---------------------------------------------------------------------------
// Fixed-capacity inventory. usedSlots tracks how many entries in slots[] are
// currently occupied (slots are kept packed at the front - see
// RemoveFromInventory in inventory.c).
// ---------------------------------------------------------------------------
typedef struct Inventory
{
    InventoryItem slots[INVENTORY_MAX_SLOTS];
    int           usedSlots;
} Inventory;

void InitInventory(Inventory *inv);
bool AddToInventory(Inventory *inv, CollectibleType type);
bool RemoveFromInventory(Inventory *inv, CollectibleType type, int amount);
int  GetInventoryItemCount(Inventory *inv, CollectibleType type);
bool IsInventoryFull(Inventory *inv);
void DrawInventoryHUD(Inventory *inv, int x, int y);

#endif // INVENTORY_H
