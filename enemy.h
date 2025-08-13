#ifndef ENEMY_H
#define ENEMY_H

#include <vector>

enum EnemyType {
    ENEMY_MELEE = 0,
    ENEMY_RANGED = 1,
    ENEMY_BASIC = 2,
    ENEMY_SNIPER = 3,
    ENEMY_FAST = 4,
    ENEMY_BOSS   = 5, 
    ENEMY_INVALID = 99
};

struct Enemy {
    float x = 0.0f;
    float y = 0.0f;
    float speed = 1.5f;
    bool activo = false;
    int health = 2;
    int maxHealth = 2;
    EnemyType type = ENEMY_MELEE;
    int shootCooldown = 0;
    int damageTimer = 0;
    
    // === NUEVO (para BOSS y afinados)
    float size = 10.0f;      // colisión/dibujo
    int   phase = 0;         // patrón actual del boss
    int   phaseTimer = 0;    // cronómetro del patrón
    int   spawnCooldown2 = 0;// cooldown para generar minions
};

extern std::vector<Enemy> enemies;

void updateEnemies(float playerX, float playerY);
void drawEnemies();
void cleanupInvalidEnemies();
void addEnemy(float x, float y, EnemyType type);
bool addEnemyFromMapChar(char symbol, float x, float y);

#endif
