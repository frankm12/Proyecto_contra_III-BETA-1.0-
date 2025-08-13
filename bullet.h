#ifndef BULLET_H
#define BULLET_H

#include <vector>

struct Bullet {
    float x, y;
    float vx, vy;
    float speed = 15.0f;
};

void shootBullet(float startX, float startY, float targetX, float targetY);
void updateBullets();
void drawBullets();
extern std::vector<Bullet> bullets;


#endif

