#include "enemy_bullet.h"
#include "globals.h"
#include "collision.h"
#include <cmath>
#include <GL/freeglut.h>

std::vector<EnemyBullet> enemyBullets;

void shootEnemyBullet(float startX, float startY, float targetX, float targetY) {
    EnemyBullet b;
    b.x = startX;
    b.y = startY;

    float dx = targetX - startX;
    float dy = targetY - startY;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length != 0) {
        b.vx = (dx / length) * b.speed;
        b.vy = (dy / length) * b.speed;
    } else {
        b.vx = 0;
        b.vy = b.speed;
    }

    enemyBullets.push_back(b);
}

void updateEnemyBullets() {
    for (auto it = enemyBullets.begin(); it != enemyBullets.end(); ) {
        it->x += it->vx;
        it->y += it->vy;

        // Colisión con jugador
        float dx = it->x - cameraX;
        float dy = it->y - cameraY;
        float dist = sqrt(dx * dx + dy * dy);
        if (dist < 15.0f && invulnerableTimer == 0) {
            playerHealth--;
            invulnerableTimer = 60;
            it = enemyBullets.erase(it);
            continue;
        }

        // Colisión con pared o fuera de límites
        if (isWall(it->x, it->y) || std::abs(it->x) > 2000 || std::abs(it->y) > 2000) {
            it = enemyBullets.erase(it);
        } else {
            ++it;
        }
    }
}

void drawEnemyBullets() {
    glColor3f(1.0f, 0.3f, 0.3f); // rojizo
    for (auto& b : enemyBullets) {
        glRectf(b.x - 3, b.y - 3, b.x + 3, b.y + 3);
    }
}

