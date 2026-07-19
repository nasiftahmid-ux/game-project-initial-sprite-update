#include "raylib.h"
#include "inventory.h"   // also pulls in collectible.h, playerstats.h, raylib.h
#include <string.h>
#include <stdio.h>
#include <math.h>        // sinf – kept for any future animated elements

// ===========================================================================
// Core bag management  (unchanged from original implementation)
// ===========================================================================

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

    // 2) No matching stack – open a new slot if capacity allows.
    if (inv->usedSlots < INVENTORY_MAX_SLOTS)
    {
        inv->slots[inv->usedSlots].type     = type;
        inv->slots[inv->usedSlots].quantity = 1;
        inv->usedSlots++;
        return true;
    }

    return false;   // bag full
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
                // Compact – keep slots packed at the front with no holes.
                for (int j = i; j < inv->usedSlots - 1; j++)
                    inv->slots[j] = inv->slots[j + 1];
                inv->usedSlots--;
            }
            return true;
        }
    }
    return false;   // type not held, or not enough quantity
}

int GetInventoryItemCount(Inventory *inv, CollectibleType type)
{
    for (int i = 0; i < inv->usedSlots; i++)
        if (inv->slots[i].type == type)
            return inv->slots[i].quantity;
    return 0;
}

bool IsInventoryFull(Inventory *inv)
{
    return inv->usedSlots >= INVENTORY_MAX_SLOTS;
}

// ---------------------------------------------------------------------------
// Sidebar HUD – compact text list, always visible in MODE_OVERWORLD
// ---------------------------------------------------------------------------
void DrawInventoryHUD(Inventory *inv, int x, int y)
{
    DrawText("INVENTORY", x, y, 18, BLACK);

    if (inv->usedSlots == 0)
    {
        DrawText("(empty)", x, y + 22, 14, GRAY);
        return;
    }

    int rowH = 20;
    for (int i = 0; i < inv->usedSlots; i++)
    {
        char line[64];
        snprintf(line, sizeof(line), "%s  x%d",
                 GetCollectibleName(inv->slots[i].type), inv->slots[i].quantity);
        DrawText(line, x, y + 22 + i * rowH, 14, DARKGRAY);
    }
}

// ===========================================================================
// UseItemFromInventory
// ===========================================================================
bool UseItemFromInventory(Inventory *inv, int slotIndex, PlayerStats *player)
{
    if (!inv || !player)                              return false;
    if (slotIndex < 0 || slotIndex >= inv->usedSlots) return false;

    InventoryItem *slot = &inv->slots[slotIndex];
    if (slot->quantity <= 0)                          return false;

    // Apply the item's stat effect, capped at the relevant maximum.
    switch (slot->type)
    {
        case POTION_HEALTH_SMALL:
            player->currentHp += 10;
            if (player->currentHp > player->maxHp)
                player->currentHp = player->maxHp;
            break;

        case POTION_HEALTH_LARGE:
            player->currentHp += 50;
            if (player->currentHp > player->maxHp)
                player->currentHp = player->maxHp;
            break;

        case POTION_CURSED_ENERGY:
            player->cursedEnergy += 30;
            if (player->cursedEnergy > player->maxCursedEnergy)
                player->cursedEnergy = player->maxCursedEnergy;
            break;

        default:
            return false;   // unrecognised – do nothing
    }

    // Consume one from the stack.
    slot->quantity--;

    // If the stack is now empty, compact the array so usedSlots stays packed.
    if (slot->quantity == 0)
    {
        for (int j = slotIndex; j < inv->usedSlots - 1; j++)
            inv->slots[j] = inv->slots[j + 1];
        inv->usedSlots--;
    }

    return true;
}

// ===========================================================================
// DrawInventoryMenu  –  full-screen bag overlay
// ===========================================================================
// Called last in the MODE_OVERWORLD draw block when isMenuOpen == true.
// Renders completely with Raylib primitives + the existing potionTextures[].
//
// Grid layout:  INV_GRID_COLS(4) x 5 rows = 20 cells = INVENTORY_MAX_SLOTS
//   All 20 cells are drawn (empty cells shown as dark placeholder boxes).
//   The selectedSlot cursor is highlighted with a bright gold frame.
//   An info bar below the grid shows the selected item name + effect text.
// ===========================================================================
void DrawInventoryMenu(const Inventory *inv, int selectedSlot,
                       Texture2D potionTextures[COLLECTIBLE_TYPE_COUNT])
{
    int sw = GetScreenWidth();
    int sh = GetScreenHeight();

    // -------------------------------------------------------------------------
    // 1. Full-screen dim overlay
    // -------------------------------------------------------------------------
    DrawRectangle(0, 0, sw, sh, (Color){ 0, 0, 8, 185 });

    // -------------------------------------------------------------------------
    // 2. Menu box
    // -------------------------------------------------------------------------
    const int BOX_W = 560;
    const int BOX_H = 565;
    int boxX = (sw - BOX_W) / 2;
    int boxY = (sh - BOX_H) / 2;

    // Drop shadow
    DrawRectangle(boxX + 7, boxY + 7, BOX_W, BOX_H, (Color){ 0, 0, 0, 110 });

    // Dark navy background
    DrawRectangle(boxX, boxY, BOX_W, BOX_H, (Color){ 14, 16, 28, 252 });

    // Outer gold border  (2.5 px)
    DrawRectangleLinesEx(
        (Rectangle){ (float)boxX, (float)boxY, (float)BOX_W, (float)BOX_H },
        2.5f, (Color){ 212, 175, 55, 255 });

    // Inner accent border (subtle)
    DrawRectangleLinesEx(
        (Rectangle){ (float)(boxX+5), (float)(boxY+5),
                     (float)(BOX_W-10), (float)(BOX_H-10) },
        1.0f, (Color){ 212, 175, 55, 55 });

    // -------------------------------------------------------------------------
    // 3. Title  "BAG"
    // -------------------------------------------------------------------------
    const Color GOLD_COLOR  = { 212, 175,  55, 255 };
    const Color LGOLD_COLOR = { 255, 215,  80, 255 };   // brighter – used for selection
    const Color LTXT  = { 185, 190, 220, 255 };   // light lavender body text

    const char *title = "BAG";
    int tSz = 28;
    int tW  = MeasureText(title, tSz);
    DrawText(title, boxX + (BOX_W - tW) / 2, boxY + 12, tSz, GOLD_COLOR);

    // Thin separator under title
    DrawRectangle(boxX + 18, boxY + 48, BOX_W - 36, 1, (Color){ 212, 175, 55, 140 });

    // -------------------------------------------------------------------------
    // 4. Item grid   (4 cols × 5 rows = 20 cells)
    // -------------------------------------------------------------------------
    const int COLS   = INV_GRID_COLS;                    // 4
    const int ROWS   = INVENTORY_MAX_SLOTS / COLS;       // 5
    const int SLOT_W = 118;
    const int SLOT_H = 76;
    const int PAD_X  = 10;
    const int PAD_Y  = 8;
    const int ICO_SZ = 42;   // icon draw size inside each slot

    // Centre grid horizontally inside the box
    //   totalGridW = 4*118 + 3*10 = 502
    int totalGridW = COLS * SLOT_W + (COLS - 1) * PAD_X;
    int gridX = boxX + (BOX_W - totalGridW) / 2;
    int gridY = boxY + 56;

    for (int idx = 0; idx < INVENTORY_MAX_SLOTS; idx++)
    {
        int col = idx % COLS;
        int row = idx / COLS;
        int sx  = gridX + col * (SLOT_W + PAD_X);
        int sy  = gridY + row * (SLOT_H + PAD_Y);

        bool occupied = (idx < inv->usedSlots);
        bool sel      = (idx == selectedSlot && occupied);

        // -- Slot background
        Color bgCol = occupied
            ? (sel ? (Color){ 40, 44, 72, 255 } : (Color){ 26, 29, 50, 255 })
            : (Color){ 18, 20, 36, 165 };
        DrawRectangle(sx, sy, SLOT_W, SLOT_H, bgCol);

        // -- Border: gold glow for selected, subtle line for others
        if (sel)
        {
            // Outer glow ring
            DrawRectangleLinesEx(
                (Rectangle){ (float)(sx-2), (float)(sy-2),
                             (float)(SLOT_W+4), (float)(SLOT_H+4) },
                3.0f, LGOLD_COLOR);
            // Inner crisp line
            DrawRectangleLinesEx(
                (Rectangle){ (float)sx, (float)sy,
                             (float)SLOT_W, (float)SLOT_H },
                1.0f, (Color){ 255, 240, 180, 170 });
        }
        else
        {
            Color bdrCol = occupied
                ? (Color){ 65, 72, 110, 215 }
                : (Color){ 28, 32, 52, 130 };
            DrawRectangleLinesEx(
                (Rectangle){ (float)sx, (float)sy,
                             (float)SLOT_W, (float)SLOT_H },
                1.0f, bdrCol);
        }

        if (!occupied) continue;   // nothing more to draw for empty cells

        const InventoryItem *item = &inv->slots[idx];
        int typeIdx = (int)item->type;

        // -- Icon  (centred horizontally, 5 px from top of slot)
        int icoX = sx + (SLOT_W - ICO_SZ) / 2;
        int icoY = sy + 5;

        if (typeIdx >= 0 && typeIdx < COLLECTIBLE_TYPE_COUNT &&
            potionTextures[typeIdx].id != 0)
        {
            Texture2D t = potionTextures[typeIdx];
            Rectangle src = { 0, 0, (float)t.width, (float)t.height };
            Rectangle dst = { (float)icoX, (float)icoY, (float)ICO_SZ, (float)ICO_SZ };
            DrawTexturePro(t, src, dst, (Vector2){ 0, 0 }, 0.0f, WHITE);
        }
        else
        {
            // Coloured circle fallback (matches RenderCollectibles palette)
            static const Color fb[3] = {
                { 170,  90, 200, 220 },
                {  60, 140, 220, 220 },
                {  80, 210, 225, 220 },
            };
            Color c = (typeIdx >= 0 && typeIdx < 3) ? fb[typeIdx] : GRAY;
            DrawCircle(icoX + ICO_SZ/2, icoY + ICO_SZ/2, ICO_SZ/2, c);
        }

        // -- Quantity badge  "x3"  (bottom-right of slot)
        char qty[12];
        snprintf(qty, sizeof(qty), "x%d", item->quantity);
        int qSz = 13;
        int qW  = MeasureText(qty, qSz);
        Color qCol = sel ? LGOLD_COLOR : (Color){ 210, 210, 210, 255 };
        DrawText(qty, sx + SLOT_W - qW - 4, sy + SLOT_H - qSz - 3, qSz, qCol);
    }

    // -------------------------------------------------------------------------
    // 5. Info bar  (selected item name + effect, below the grid)
    // -------------------------------------------------------------------------
    //   Grid bottom = gridY + ROWS*SLOT_H + (ROWS-1)*PAD_Y
    //               = boxY+56 + 5*76 + 4*8 = boxY+56+380+32 = boxY+468
    int infoY = gridY + ROWS * SLOT_H + (ROWS - 1) * PAD_Y + 10;   // boxY + 478

    DrawRectangle(boxX + 12, infoY, BOX_W - 24, 40, (Color){ 20, 22, 40, 240 });
    DrawRectangleLinesEx(
        (Rectangle){ (float)(boxX+12), (float)infoY,
                     (float)(BOX_W-24), 40.0f },
        1.0f, (Color){ 212, 175, 55, 85 });

    if (inv->usedSlots > 0 && selectedSlot >= 0 && selectedSlot < inv->usedSlots)
    {
        const InventoryItem *sel = &inv->slots[selectedSlot];
        const char *desc = "Unknown item";
        switch (sel->type)
        {
            case POTION_HEALTH_SMALL:
                desc = "Potion  |  Restores 10 HP"; break;
            case POTION_HEALTH_LARGE:
                desc = "Super Potion  |  Restores 50 HP"; break;
            case POTION_CURSED_ENERGY:
                desc = "Cursed Energy Elixir  |  +30 Cursed Energy"; break;
            default: break;
        }
        DrawText(desc, boxX + 20, infoY + 12, 14, LTXT);
    }
    else
    {
        // Empty bag message
        const char *msg = "Your bag is empty.";
        int mW = MeasureText(msg, 15);
        DrawText(msg, boxX + (BOX_W - mW) / 2, infoY + 11, 15,
                 (Color){ 120, 125, 160, 200 });
    }

    // -------------------------------------------------------------------------
    // 6. Controls hint  (bottom of box)
    // -------------------------------------------------------------------------
    const char *hint = "[I] Close    [Arrows] Navigate    [ENTER] Use Item";
    int hSz = 12;
    int hW  = MeasureText(hint, hSz);
    DrawText(hint, boxX + (BOX_W - hW) / 2, boxY + BOX_H - 18, hSz,
             (Color){ 125, 130, 168, 200 });
}
