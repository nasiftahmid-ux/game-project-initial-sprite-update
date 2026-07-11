#include "battle.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define HP_LERP_SPEED     60.0f
#define FLASH_DURATION    0.15f
#define MESSAGE_PAUSE_DURATION 1.2f
#define ATTACK_MOVE_SPEED 900.0f
#define ATTACK_HIT_PAUSE  0.2f
#define ATTACK_LUNGE_DIST 80.0f

static float Vec2Distance(Vector2 a, Vector2 b)
{
    float dx = a.x - b.x, dy = a.y - b.y;
   // float dx =900 - 400, dy = 900 - 300;
    return sqrtf(dx * dx + dy * dy);
}

static Vector2 MoveToward(Vector2 current, Vector2 target, float maxDist)
{
    float dx = target.x - current.x;
    float dy = target.y - current.y;
    float dist = sqrtf(dx * dx + dy * dy);
    if (dist <= maxDist || dist == 0.0f)
    {
        return target;
    } 
    float t = maxDist / dist;
    return (Vector2){ current.x + dx * t, current.y + dy * t };
}

void InitBattleScene(BattleScene *battle, const char *playerName, const char *enemyName)
{
    strncpy(battle->player.name, playerName, MAX_NAME_LEN - 1);
    battle->player.name[MAX_NAME_LEN - 1] = '\0';
    battle->player.maxHp = 100;
    battle->player.currentHp = 100;
    battle->player.displayedHp = 100;
    battle->player.size = (Vector2){ 60, 90 };
    battle->player.baseColor = BLUE;
    battle->player.basePos = (Vector2){120, 300 };
    battle->player.pos = battle->player.basePos;
    battle->player.tint = battle->player.baseColor;
    battle->player.flashTimer = 0.0f;

    strncpy(battle->enemy.name, enemyName, MAX_NAME_LEN - 1);
    battle->enemy.name[MAX_NAME_LEN - 1] = '\0';
    battle->enemy.maxHp = 80;
    battle->enemy.currentHp = 80;
    battle->enemy.displayedHp = 80;
    battle->enemy.size = (Vector2){ 60, 70 };
    battle->enemy.baseColor = MAROON;
    battle->enemy.basePos = (Vector2){ 600, 200 };
    battle->enemy.pos = battle->enemy.basePos;
    battle->enemy.tint = battle->enemy.baseColor;
    battle->enemy.flashTimer = 0.0f;

    strcpy(battle->playerMoves[0].name, "Nasif's Slash");
    battle->playerMoves[0].minDamage = 5;
    battle->playerMoves[0].maxDamage = 15;

    strcpy(battle->playerMoves[1].name, "MOOD SWING STRIKE");
    battle->playerMoves[1].minDamage = 16;
    battle->playerMoves[1].maxDamage = 17;

    strcpy(battle->playerMoves[2].name, "LAGA 50TA NOTE");
    battle->playerMoves[2].minDamage = 20;
    battle->playerMoves[2].maxDamage = 30;

    strcpy(battle->playerMoves[3].name, "Poke");
    battle->playerMoves[3].minDamage = 3;
    battle->playerMoves[3].maxDamage = 6;

    battle->moveCount = 4;
    battle->selectedMoveIndex = 0;

    strcpy(battle->enemyMoves[0].name, "MINOR Bite");
    battle->enemyMoves[0].minDamage = 8;
    battle->enemyMoves[0].maxDamage = 9;

    strcpy(battle->enemyMoves[1].name, "MINOR Swipe");
    battle->enemyMoves[1].minDamage = 6;
    battle->enemyMoves[1].maxDamage = 19;

    strcpy(battle->enemyMoves[2].name, "MINOR Growl Slam");
    battle->enemyMoves[2].minDamage = 14;
    battle->enemyMoves[2].maxDamage = 25;

    strcpy(battle->enemyMoves[3].name, "ONLY MINOR SLAM");
    battle->enemyMoves[3].minDamage = 16;
    battle->enemyMoves[3].maxDamage = 17;

    battle->enemyMoveCount = 4;

    battle->pendingMove = NULL;
    battle->pendingDamage = 0;

    battle->state = BATTLE_INTRO;
    battle->stateTimer = 0.0f;
    strcpy(battle->messageText, "");
    battle->nextStateAfterMessage = BATTLE_PLAYER_MENU;

    battle->rewardExp = 0;
    battle->rewardCoins = 0;
}

void UnloadBattleScene(BattleScene *battle)
{
    (void)battle;
}

int RollDamage(Move *move)
{
    return move->minDamage + rand() % (move->maxDamage - move->minDamage + 1);
}

void ApplyDamage(Character *target, int amount)
{
    target->currentHp -= amount;
    if (target->currentHp < 0) target->currentHp = 0;
    target->flashTimer = FLASH_DURATION;
}

void UpdateHpBarLerp(Character *c, float dt)
{
    if (c->displayedHp > c->currentHp) {
        c->displayedHp -= (int)(HP_LERP_SPEED * dt);
        if (c->displayedHp < c->currentHp) c->displayedHp = c->currentHp;
    } else if (c->displayedHp < c->currentHp) {
        c->displayedHp += (int)(HP_LERP_SPEED * dt);
        if (c->displayedHp > c->currentHp) c->displayedHp = c->currentHp;
    }
}

static void UpdateFlash(Character *c, float dt)
{
    if (c->flashTimer > 0.0f) {
        c->flashTimer -= dt;
        c->tint = RED;
        if (c->flashTimer <= 0.0f) {
            c->flashTimer = 0.0f;
            c->tint = c->baseColor;
        }
    }
}

void DrawHpBar(Character *c, Vector2 barPos, int barWidth, int barHeight)
{
    float pct = (float)c->displayedHp / (float)c->maxHp;
    if (pct < 0) pct = 0;

    DrawRectangle((int)barPos.x, (int)barPos.y, barWidth, barHeight, DARKGRAY);

    Color fillColor = GREEN;
    if (pct < 0.5f) fillColor = YELLOW;
    if (pct < 0.2f) fillColor = RED;

    DrawRectangle((int)barPos.x, (int)barPos.y, (int)(barWidth * pct), barHeight, fillColor);
    DrawRectangleLines((int)barPos.x, (int)barPos.y, barWidth, barHeight, BLACK);

    DrawText(c->name, (int)barPos.x, (int)barPos.y - 20, 18, BLACK);

    char hpText[16];
    snprintf(hpText, sizeof(hpText), "%d/%d", c->currentHp, c->maxHp);
    DrawText(hpText, (int)barPos.x, (int)barPos.y + barHeight + 2, 14, DARKGRAY);
}

static Move *PickEnemyMove(BattleScene *battle)
{
    int idx = rand() % battle->enemyMoveCount;
    return &battle->enemyMoves[idx];
}

bool IsBattleOver(BattleScene *battle)
{
    return battle->state == BATTLE_DONE;
}

void UpdateBattleScene(BattleScene *battle, float dt)
{
    battle->stateTimer += dt;

    UpdateHpBarLerp(&battle->player, dt);
    UpdateHpBarLerp(&battle->enemy, dt);
    UpdateFlash(&battle->player, dt);
    UpdateFlash(&battle->enemy, dt);

    if (battle->state == BATTLE_INTRO) {
        if (battle->stateTimer > 1.0f) {
            battle->state = BATTLE_PLAYER_MENU;
            battle->stateTimer = 0.0f;
        }
    }
    else if (battle->state == BATTLE_PLAYER_MENU) {
        if (IsKeyPressed(KEY_DOWN)) {
            battle->selectedMoveIndex = (battle->selectedMoveIndex + 1) % battle->moveCount;
        }
        if (IsKeyPressed(KEY_UP)) {
            battle->selectedMoveIndex =
                (battle->selectedMoveIndex - 1 + battle->moveCount) % battle->moveCount;
        }

        int chosen = -1;
        if (IsKeyPressed(KEY_ONE)) chosen = 0;
        if (IsKeyPressed(KEY_TWO)) chosen = 1;
        if (IsKeyPressed(KEY_THREE)) chosen = 2;
        if (IsKeyPressed(KEY_FOUR)) chosen = 3;
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) chosen = battle->selectedMoveIndex;

        if (chosen >= 0 && chosen < battle->moveCount) {
            battle->pendingMove = &battle->playerMoves[chosen];
            battle->state = BATTLE_PLAYER_ATTACK_IN;
            battle->stateTimer = 0.0f;
        }
    }
    else if (battle->state == BATTLE_PLAYER_ATTACK_IN) {
        Vector2 target = {
            battle->enemy.basePos.x - ATTACK_LUNGE_DIST,
            battle->enemy.basePos.y
        };
        battle->player.pos = MoveToward(battle->player.pos, target, ATTACK_MOVE_SPEED * dt);
        if (Vec2Distance(battle->player.pos, target) < 1.0f) {
            battle->state = BATTLE_PLAYER_ATTACK_HIT;
            battle->stateTimer = 0.0f;
        }
    }
    else if (battle->state == BATTLE_PLAYER_ATTACK_HIT) {
        if (battle->stateTimer == dt) {
            int dmg = RollDamage(battle->pendingMove);
            battle->pendingDamage = dmg;
            ApplyDamage(&battle->enemy, dmg);
            snprintf(battle->messageText, sizeof(battle->messageText),
                     "%s used %s!\n%s took %d damage!",
                     battle->player.name, battle->pendingMove->name,
                     battle->enemy.name, dmg);
        }
        if (battle->stateTimer > ATTACK_HIT_PAUSE) {
            battle->state = BATTLE_PLAYER_ATTACK_OUT;
            battle->stateTimer = 0.0f;
        }
    }
    else if (battle->state == BATTLE_PLAYER_ATTACK_OUT) {
        battle->player.pos = MoveToward(battle->player.pos, battle->player.basePos, ATTACK_MOVE_SPEED * dt);
        if (Vec2Distance(battle->player.pos, battle->player.basePos) < 1.0f) {
            battle->player.pos = battle->player.basePos;
            battle->state = BATTLE_MESSAGE_PAUSE;
            battle->stateTimer = 0.0f;
            battle->nextStateAfterMessage =
                (battle->enemy.currentHp <= 0) ? BATTLE_WIN : BATTLE_ENEMY_ATTACK_IN;
        }
    }
    else if (battle->state == BATTLE_MESSAGE_PAUSE) {
        if (battle->stateTimer > MESSAGE_PAUSE_DURATION) {
            battle->state = battle->nextStateAfterMessage;
            battle->stateTimer = 0.0f;

            if (battle->state == BATTLE_ENEMY_ATTACK_IN) {
                battle->pendingMove = PickEnemyMove(battle);
            }
            if (battle->state == BATTLE_WIN) {
                battle->rewardExp = 40 + rand() % 21;
                battle->rewardCoins = 80 + rand() % 41;
            }
        }
    }
    else if (battle->state == BATTLE_ENEMY_ATTACK_IN) {
        Vector2 target = {
            battle->player.basePos.x + ATTACK_LUNGE_DIST,
            battle->player.basePos.y
        };
        battle->enemy.pos = MoveToward(battle->enemy.pos, target, ATTACK_MOVE_SPEED * dt);
        if (Vec2Distance(battle->enemy.pos, target) < 1.0f) {
            battle->state = BATTLE_ENEMY_ATTACK_HIT;
            battle->stateTimer = 0.0f;
        }
    }
    else if (battle->state == BATTLE_ENEMY_ATTACK_HIT) {
        if (battle->stateTimer == dt) {
            int dmg = RollDamage(battle->pendingMove);
            battle->pendingDamage = dmg;
            ApplyDamage(&battle->player, dmg);
            snprintf(battle->messageText, sizeof(battle->messageText),
                     "%s used %s!\n%s took %d damage!",
                     battle->enemy.name, battle->pendingMove->name,
                     battle->player.name, dmg);
        }
        if (battle->stateTimer > ATTACK_HIT_PAUSE) {
            battle->state = BATTLE_ENEMY_ATTACK_OUT;
            battle->stateTimer = 0.0f;
        }
    }
    else if (battle->state == BATTLE_ENEMY_ATTACK_OUT) {
        battle->enemy.pos = MoveToward(battle->enemy.pos, battle->enemy.basePos, ATTACK_MOVE_SPEED * dt);
        if (Vec2Distance(battle->enemy.pos, battle->enemy.basePos) < 1.0f) {
            battle->enemy.pos = battle->enemy.basePos;
            battle->state = BATTLE_MESSAGE_PAUSE;
            battle->stateTimer = 0.0f;
            battle->nextStateAfterMessage =
                (battle->player.currentHp <= 0) ? BATTLE_LOSE : BATTLE_PLAYER_MENU;
        }
    }
    else if (battle->state == BATTLE_WIN) {
        if (battle->stateTimer > 0.5f &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
            battle->state = BATTLE_DONE;
        }
    }
    else if (battle->state == BATTLE_LOSE) {
        if (battle->stateTimer > 0.5f &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))) {
            battle->state = BATTLE_DONE;
        }
    }
    else if (battle->state == BATTLE_DONE) {
        // কিছুই করার নেই
    }
}
void DrawBattleScene(BattleScene *battle)
{
    ClearBackground(RAYWHITE);
    DrawLine(0, 400, 800, 400, LIGHTGRAY);

    DrawRectangleV(battle->player.pos, battle->player.size, battle->player.tint);
    DrawRectangleV(battle->enemy.pos, battle->enemy.size, battle->enemy.tint);

    DrawHpBar(&battle->player, (Vector2){ 60, 420 }, 200, 20);
    DrawHpBar(&battle->enemy, (Vector2){ 540, 60 }, 200, 20);

    if (battle->state == BATTLE_PLAYER_MENU) {
        DrawRectangle(0, 460, 800, 140, (Color){ 20, 20, 20, 220 });
        DrawText("Fight", 20, 470, 22, WHITE);
        for (int i = 0; i < battle->moveCount; i++) {
            char line[64];
            snprintf(line, sizeof(line), "%d  %s", i + 1, battle->playerMoves[i].name);
            Color col = (i == battle->selectedMoveIndex) ? YELLOW : WHITE;
            // little arrow marker on the selected move, like a real menu cursor
            if (i == battle->selectedMoveIndex) {
                DrawText(">", 5, 500 + i * 22, 18, YELLOW);
            }
            DrawText(line, 20, 500 + i * 22, 18, col);
        }
        DrawText("Arrows + Enter, or press 1-4", 500, 590, 14, GRAY);
    }

    bool showMessageBox =
        battle->state == BATTLE_MESSAGE_PAUSE ||
        battle->state == BATTLE_PLAYER_ATTACK_HIT ||
        battle->state == BATTLE_PLAYER_ATTACK_OUT ||
        battle->state == BATTLE_ENEMY_ATTACK_HIT ||
        battle->state == BATTLE_ENEMY_ATTACK_OUT;

    if (showMessageBox) {
        DrawRectangle(0, 460, 800, 140, (Color){ 20, 20, 20, 220 });
        DrawText(battle->messageText, 20, 490, 20, WHITE);
    }

    if (battle->state == BATTLE_WIN) {
        DrawRectangle(0, 460, 800, 140, (Color){ 20, 20, 20, 220 });
        DrawText("YOU WIN!", 20, 470, 32, GOLD);

        char rewardLine[64];
        snprintf(rewardLine, sizeof(rewardLine), "EXP +%d", battle->rewardExp);
        DrawText(rewardLine, 20, 515, 20, WHITE);

        snprintf(rewardLine, sizeof(rewardLine), "Coins +%d", battle->rewardCoins);
        DrawText(rewardLine, 20, 540, 20, WHITE);

        DrawText("Press Enter to continue", 20, 575, 16, GRAY);
    }

    if (battle->state == BATTLE_LOSE) {
        DrawRectangle(0, 460, 800, 140, (Color){ 20, 20, 20, 220 });
        DrawText("YOU LOST...", 20, 490, 32, RED);
        DrawText("Press Enter to continue", 20, 550, 16, GRAY);
    }
}