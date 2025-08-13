#include "bullet.h"
#include "enemy.h"
#include "collision.h"
#include "map.h"
#include <GL/freeglut.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include "globals.h"
#include <iostream>

// Lista de balas activas
std::vector<Bullet> bullets;
extern std::vector<Enemy> enemies;
extern int score;

static void onBossDefeated(float x, float y) {
    bossAlive = false;
    bossExploding = true;
    bossExplodeTimer = 90; // ~1.5s
    bossDeathX = x;
    bossDeathY = y;
    // Portal se coloca al terminar la explosión (ver Main.timer)
}

void shootBullet(float startX, float startY, float targetX, float targetY) {
    Bullet b;
    b.x = startX;
    b.y = startY;

    float dx = targetX - startX;
    float dy = targetY - startY;
    float length = sqrt(dx * dx + dy * dy);

    if (length != 0) {
        b.vx = (dx / length) * b.speed;
        b.vy = (dy / length) * b.speed;
    } else {
        b.vx = 0;
        b.vy = b.speed;
    }

    bullets.push_back(b);
}

void updateBullets() {
    // 0) Defensa: evitar acumulación infinita de balas
    if (bullets.size() > 2000) {
        bullets.erase(bullets.begin(), bullets.begin() + 1000);
    }

    // Listas de borrado diferido
    std::vector<size_t> bulletsToErase;
    std::vector<size_t> enemiesToErase;

    // 1) Avanza y chequea cada bala
    for (size_t bi = 0; bi < bullets.size(); ++bi) {
        Bullet &b = bullets[bi];
        b.x += b.vx;
        b.y += b.vy;

        bool hit = false;

        // 1a) Colisión con enemigos (no borrar aquí)
        for (size_t ei = 0; ei < enemies.size(); ++ei) {
            Enemy &e = enemies[ei];
            if (e.health <= 0 || e.type >= ENEMY_INVALID) continue;

            if (bulletHitsEnemy(b.x, b.y, e.x, e.y, e.size)) {
                e.health--;

                if (e.health <= 0) {
                    // si era boss, dispara secuencia
                    if (e.type == ENEMY_BOSS) {
                        onBossDefeated(e.x, e.y);
                    }
                    enemiesToErase.push_back(ei);
                    score += 1000;
                }
                hit = true;
                break; // esta bala ya impactó
            }
        }

        // 1b) Colisión con pared o fuera de mundo
        if (!hit) {
            if (isWall(b.x, b.y) ||
                b.y > 2000 || b.y < -2000 || b.x > 2000 || b.x < -2000) {
                hit = true;
            }
        }

        if (hit) bulletsToErase.push_back(bi);
    }

    // 2) Función de borrado decreciente (sin 'auto' en lambda)
    struct EraseHelper {
        static void eraseDescending(std::vector<Bullet> &vec, const std::vector<size_t> &indices) {
            if (indices.empty()) return;
            std::vector<size_t> sorted = indices;
            std::sort(sorted.begin(), sorted.end());
            sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
            for (int i = (int)sorted.size() - 1; i >= 0; --i) {
                size_t idx = sorted[i];
                if (idx < vec.size()) vec.erase(vec.begin() + idx);
            }
        }
        static void eraseDescendingEnemies(std::vector<Enemy> &vec, const std::vector<size_t> &indices) {
            if (indices.empty()) return;
            std::vector<size_t> sorted = indices;
            std::sort(sorted.begin(), sorted.end());
            sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
            for (int i = (int)sorted.size() - 1; i >= 0; --i) {
                size_t idx = sorted[i];
                if (idx < vec.size()) vec.erase(vec.begin() + idx);
            }
        }
    };

    // 3) Ejecutar borrados
    EraseHelper::eraseDescending(bullets, bulletsToErase);
    EraseHelper::eraseDescendingEnemies(enemies, enemiesToErase);
}



void drawBullets() {
    glColor3f(1.0f, 1.0f, 0.0f); // Amarillo
    for (auto& b : bullets) {
        glRectf(b.x - 2, b.y - 2, b.x + 2, b.y + 2);
    }
}

