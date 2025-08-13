#include "enemy.h"
#include "collision.h"
#include "globals.h"
#include <GL/freeglut.h>
#include <cmath>
#include <iostream>
#include <algorithm>
#include "enemy_bullet.h"
#include "map.h"  // arriba de Enemy.cpp

std::vector<Enemy> enemies;

void addEnemy(float x, float y, EnemyType type) {
    Enemy e;
    e.x = x;
    e.y = y;
    e.type = type;

    switch(type) {
        case ENEMY_MELEE:  e.speed=1.5f; e.health=2; break;
        case ENEMY_RANGED: e.speed=0.8f; e.health=1; break;
        case ENEMY_BASIC:  e.speed=1.0f; e.health=3; break;
        case ENEMY_SNIPER: e.speed=0.6f; e.health=2; break;
        case ENEMY_FAST:   e.speed=2.5f; e.health=1; break;

        case ENEMY_BOSS:
            e.speed = 0.7f;
            e.health = 40;
            e.maxHealth = 40;
            e.size = 20.0f;
            e.phase = 0;
            e.phaseTimer = 0;
            e.spawnCooldown2 = 180;
            break;

        default:
            std::cerr << "Tipo de enemigo desconocido, usando valores por defecto\n";
            e.type = ENEMY_MELEE;
            e.speed = 1.5f;
            e.health = 2;
    }

    e.maxHealth = e.health;
    e.activo = false;
    enemies.push_back(e);

    // === NUEVO: log definitivo de inserción ===
    std::cout << "[SPAWN] Enemy type=" << (int)e.type
              << " at (" << e.x << "," << e.y << ")"
              << " health=" << e.health << " size=" << e.size << "\n";
}


bool addEnemyFromMapChar(char symbol, float x, float y) {
    unsigned char us = static_cast<unsigned char>(symbol);
    char s = static_cast<char>(std::toupper(us));  // normaliza a MAYÚSCULA

    switch (s) {
        case 'E': 
            std::cout << "[DEBUG] Enemigo MELEE colocado en (" << x << "," << y << ")\n";
            addEnemy(x,y,ENEMY_MELEE);  
            return true;
        case 'B': 
            std::cout << "[DEBUG] Enemigo BASIC colocado en (" << x << "," << y << ")\n";
            addEnemy(x,y,ENEMY_BASIC);  
            return true;
        case 'S': 
            std::cout << "[DEBUG] Enemigo SNIPER colocado en (" << x << "," << y << ")\n";
            addEnemy(x,y,ENEMY_SNIPER); 
            return true;
        case 'F': 
            std::cout << "[DEBUG] Enemigo FAST colocado en (" << x << "," << y << ")\n";
            addEnemy(x,y,ENEMY_FAST);   
            return true;
        case 'K':
            std::cout << "[DEBUG] BOSS colocado en (" << x << "," << y << ") - Nivel actual: " << nivelActual << "\n";
            addEnemy(x,y,ENEMY_BOSS);
            bossAlive = true;
            return true;
        default:
            return false;
    }
}

void updateEnemies(float playerX, float playerY) {
    const float visionRange = 300.0f;
    bool playerDamagedThisFrame = false;

    for (auto& e : enemies) {
        if (e.health <= 0 || e.type >= ENEMY_INVALID) continue;

        float dx = playerX - e.x;
        float dy = playerY - e.y;
        float distance = sqrt(dx*dx + dy*dy);
        
        // Debug de activación
        if (!e.activo && distance < visionRange) {
            std::cout << "Activando enemigo en (" << e.x << "," << e.y 
                      << ") - Tipo: " << e.type << std::endl;
            e.activo = true;
        }

        // Activación por proximidad
        if (!e.activo && distance < visionRange) {
            e.activo = true;
            // Asegurar valores correctos al activarse
            e.health = std::max(1, e.health);
            if (e.type == ENEMY_MELEE) {
                e.speed = 1.5f;
            } else {
                e.speed = 0.8f;
            }
        }

        // Movimiento solo para melee activos
        if (e.activo && e.type == ENEMY_MELEE) {
            if (distance > 20.0f) {
                float dirX = dx/distance;
                float dirY = dy/distance;
                
                // Movimiento con colisión
                float nextX = e.x + dirX * e.speed;
                if (!isWall(nextX, e.y)) e.x = nextX;
                
                float nextY = e.y + dirY * e.speed;
                if (!isWall(e.x, nextY)) e.y = nextY;
            }
        }
        else if (e.activo && e.type == ENEMY_SNIPER) {
		    float dx = playerX - e.x;
		    float dy = playerY - e.y;
		    float distance = sqrt(dx*dx + dy*dy);
		
		    // Dispara cada cierto tiempo si ve al jugador
		    if (distance < 500.0f) {
		        if (e.shootCooldown <= 0) {
		            shootEnemyBullet(e.x, e.y, playerX, playerY);
		            e.shootCooldown = 120; // 2 segundos entre disparos (60 FPS)
		        } else {
		            e.shootCooldown--;
		        }
		    }
		}
		else if (e.activo && e.type == ENEMY_FAST) {
		    if (distance > 20.0f) {
		        float dirX = dx / distance;
		        float dirY = dy / distance;
		
		        float nextX = e.x + dirX * e.speed;
		        if (!isWall(nextX, e.y)) e.x = nextX;
		
		        float nextY = e.y + dirY * e.speed;
		        if (!isWall(e.x, nextY)) e.y = nextY;
		    }
		}
		// dentro de updateEnemies(...)
		else if (e.activo && e.type == ENEMY_BOSS) {
		    // 1) El boss se mueve lento hacia/alejado del jugador con un vaivén
		    float wiggle = sin(e.phaseTimer * 0.05f) * 0.8f;
		    float dirX = dx / std::max(1.0f, distance);
		    float dirY = dy / std::max(1.0f, distance);
		    // Mantener cierta distancia media (kiteo simple)
		    float desired = 220.0f;
		    float push = (distance > desired ? 1.0f : -1.0f);
		    float nextX = e.x + (dirX * e.speed * push) + wiggle;
		    float nextY = e.y + (dirY * e.speed * push) - wiggle;
		    if (!isWall(nextX, e.y)) e.x = nextX;
		    if (!isWall(e.x, nextY)) e.y = nextY;
		
		    // 2) Ciclo de fases (cada ~6s cambia)
		    e.phaseTimer++;
		    if (e.phaseTimer > 360) {
		        e.phaseTimer = 0;
		        e.phase = (e.phase + 1) % 3; // 0: minions, 1: radial, 2: sniper burst
		    }
		
		    // 3) Acciones por fase
		    if (e.phase == 0) {
		        // Spawn de minions rápidos cada 3s
		        if (e.spawnCooldown2 <= 0) {
		            // genera 2-3 FAST alrededor
		            addEnemy(e.x + 60, e.y, ENEMY_FAST);
		            addEnemy(e.x - 60, e.y, ENEMY_FAST);
		            if (rand() % 2) addEnemy(e.x, e.y + 80, ENEMY_FAST);
		            e.spawnCooldown2 = 180;
		        } else e.spawnCooldown2--;
		    }
		    else if (e.phase == 1) {
		        // Ráfaga radial de balas cada 45 frames
		        if (e.shootCooldown <= 0) {
		            const int N = 12;
		            for (int i=0;i<N;++i) {
		                float ang = (2.0f * 3.1415926f * i) / N;
		                float tx = e.x + cos(ang) * 200.0f;
		                float ty = e.y + sin(ang) * 200.0f;
		                shootEnemyBullet(e.x, e.y, tx, ty);
		            }
		            e.shootCooldown = 45;
		        } else e.shootCooldown--;
		    }
		    else if (e.phase == 2) {
		        // Disparos dirigidos en ráfagas de 3
		        if (e.shootCooldown <= 0) {
		            for (int k=0;k<3;++k) {
		                shootEnemyBullet(e.x, e.y, playerX, playerY);
		            }
		            e.shootCooldown = 60;
		        } else e.shootCooldown--;
		    }
		}




        // Daño al jugador
        if (e.activo && distance < 20.0f && invulnerableTimer == 0 && !playerDamagedThisFrame) {
            playerHealth--;
            invulnerableTimer = 60;
            playerDamagedThisFrame = true;
            
            // Empujar al enemigo
            e.x += (e.x < playerX ? -40 : 40);
            e.y += (e.y < playerY ? -40 : 40);
        }
    }
}

void drawEnemies() {
    static int frameCounter = 0;
    frameCounter++;

    for (auto& e : enemies) {
        // Validación general
        if (e.health <= 0 || std::isnan(e.x) || std::isnan(e.y)) continue;

        // Color por tipo de enemigo
		switch (e.type) {
		    case ENEMY_MELEE:  glColor3f(1.0f, 0.0f, 0.0f); break;
		    case ENEMY_BASIC:  glColor3f(1.0f, 1.0f, 0.0f); break;
		    case ENEMY_SNIPER: glColor3f(0.3f, 0.3f, 1.0f); break;
		    case ENEMY_FAST:   glColor3f(0.0f, 1.0f, 0.0f); break;
		    case ENEMY_BOSS:   glColor3f(0.9f, 0.3f, 0.9f); break; // morado
		    default:           glColor3f(1.0f, 1.0f, 1.0f); break;
		}

        // Cuerpo del enemigo (cuadrado de 20x20)
        float r = e.size;
		glBegin(GL_QUADS);
		glVertex2f(e.x - r, e.y - r);
		glVertex2f(e.x + r, e.y - r);
		glVertex2f(e.x + r, e.y + r);
		glVertex2f(e.x - r, e.y + r);
		glEnd();

        // Barra de vida (visible siempre que tenga vida)
        if (e.health > 0) {
		    float barWidth = r * 2.0f;
		    float barHeight = 4.0f;
		    float xLeft = e.x - barWidth / 2.0f;
		    float yTop  = e.y + r + 6.0f;
		    // fondo
		    glColor3f(0.2f, 0.0f, 0.0f);
		    glBegin(GL_QUADS);
		    glVertex2f(xLeft, yTop);
		    glVertex2f(xLeft + barWidth, yTop);
		    glVertex2f(xLeft + barWidth, yTop - barHeight);
		    glVertex2f(xLeft, yTop - barHeight);
		    glEnd();
		    // actual
		    float healthRatio = (float)e.health / e.maxHealth;
		    glColor3f(0.0f, 1.0f, 0.0f);
		    glBegin(GL_QUADS);
		    glVertex2f(xLeft, yTop);
		    glVertex2f(xLeft + barWidth * healthRatio, yTop);
		    glVertex2f(xLeft + barWidth * healthRatio, yTop - barHeight);
		    glVertex2f(xLeft, yTop - barHeight);
		    glEnd();
		}
    }
}


void cleanupInvalidEnemies() {
    const int tile = 40;
    const int H = (int)mapData.size();
    const int W = H ? (int)mapData[0].size() : 0;
    const float maxX = (W/2.0f + 2) * tile; // margen
    const float maxY = (H/2.0f + 2) * tile;

    enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
        [&](const Enemy& e) {
            return std::isnan(e.x) || std::isnan(e.y) ||
                   e.x < -maxX || e.x > maxX ||
                   e.y < -maxY || e.y > maxY ||
                   e.health <= 0 || e.health > 100 ||
                   e.type >= ENEMY_INVALID;
        }),
    enemies.end());
}
