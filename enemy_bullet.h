#ifndef ENEMY_BULLET_H
#define ENEMY_BULLET_H

#include <vector>

struct EnemyBullet {
    float x, y;
    float vx, vy;
    float speed = 7.0f;
};

extern std::vector<EnemyBullet> enemyBullets;

void shootEnemyBullet(float startX, float startY, float targetX, float targetY);
void updateEnemyBullets();
void drawEnemyBullets();

#endif

