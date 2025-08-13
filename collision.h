#ifndef COLLISION_H
#define COLLISION_H

// Verifica si hay una pared en esa posici�n
bool isWall(float worldX, float worldY);

// Verifica si el jugador puede moverse a la posici�n (considerando tama�o)
bool canMoveTo(float centerX, float centerY);

// Colisi�n bala-enemigo (con tama�o del enemigo)
bool bulletHitsEnemy(float bulletX, float bulletY, float enemyX, float enemyY, float enemySize);

// �est� sobre un portal?
bool isOnPortal(float worldX, float worldY);

#endif


