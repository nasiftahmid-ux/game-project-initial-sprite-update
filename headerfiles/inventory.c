#include "raylib.h"
#include "inventory.h"
#include <string.h>
#include <stdio.h>

void InitInventory(Inventory *inv)
{
    memset(inv, 0, sizeof(Inventory));
    inv->usedSlots = 0;
}

bool AddToInventory(Inventory *inv, CollectibleType type)
{
    // 1) Stack onto an existing slot of the same type first.
    for (int i = 0; i < inv->usedSlots; i++)
    {
        if (inv->slots[i].type == type && inv->slots[i].quantity > 0)
        {
            inv->slots[i].quantity++;
            return true;
        }
    }

    // 2) No matching stack yet - open a brand new slot if capacity allows.
    if (inv->usedSlots < INVENTORY_MAX_SLOTS)
    {
        inv->slots[inv->usedSlots].type     = type;
        inv->slots[inv->usedSlots].quantity = 1;
        inv->usedSlots++;
        return true;
    }

    // 3) Inventory full.
    return false;
}

bool RemoveFromInventory(Inventory *inv, CollectibleType type, int amount)
{
    for (int i = 0; i < inv->usedSlots; i++)
    {
        if (inv->slots[i].type == type && inv->slots[i].quantity >= amount)
        {
            inv->slots[i].quantity -= amount;

            if (inv->slots[i].quantity == 0)
            {
                // Compact the array so usedSlots always spans live stacks
                // (slots stay packed at the front - no holes to skip over).
                for (int j = i; j < inv->usedSlots - 1; j++)
                {
                    inv->slots[j] = inv->slots[j + 1];
                }
                inv->usedSlots--;
            }
            return true;
        }
    }
    return false; // type not held, or not enough of it
}

int GetInventoryItemCount(Inventory *inv, CollectibleType type)
{
    for (int i = 0; i < inv->usedSlots; i++)
    {
        if (inv->slots[i].type == type)
        {
            return inv->slots[i].quantity;
        }
    }
    return 0;
}

bool IsInventoryFull(Inventory *inv)
{
    return inv->usedSlots >= INVENTORY_MAX_SLOTS;
}

void DrawInventoryHUD(Inventory *inv, int x, int y)
{
    DrawText("INVENTORY", x, y, 20, BLACK);

    if (inv->usedSlots == 0)
    {
        DrawText("(empty)", x, y + 26, 16, GRAY);
        return;
    }

    int rowHeight = 22;
    for (int i = 0; i < inv->usedSlots; i++)
    {
        char line[64];
        snprintf(line, sizeof(line), "%s  x%d",
                 GetCollectibleName(inv->slots[i].type), inv->slots[i].quantity);
        DrawText(line, x, y + 26 + i * rowHeight, 16, DARKGRAY);
    }
}
