#ifndef COLLISION_H
#define COLLISION_H

// Verifica si hay una pared en esa posición
bool isWall(float worldX, float worldY);

// Verifica si el jugador puede moverse a la posición (considerando tamaño)
bool canMoveTo(float centerX, float centerY);

// Colisión bala-enemigo (con tamaño del enemigo)
bool bulletHitsEnemy(float bulletX, float bulletY, float enemyX, float enemyY, float enemySize);

// ¿está sobre un portal?
bool isOnPortal(float worldX, float worldY);

#endif


