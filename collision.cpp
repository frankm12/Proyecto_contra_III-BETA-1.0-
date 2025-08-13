#include "collision.h"
#include "map.h"

extern std::vector<std::string> mapData;

bool isWall(float worldX, float worldY) {
    int tileSize = 40;

    int mapHeight = mapData.size();
    int mapWidth = mapData[0].size();

    int col = static_cast<int>((worldX / tileSize) + mapWidth / 2.0f);
    int row = static_cast<int>((mapHeight / 2.0f) - (worldY / tileSize));

    if (row >= 0 && row < mapHeight && col >= 0 && col < mapWidth) {
        return mapData[row][col] == '#';
    }

    return false;
}


// Verifica las 4 esquinas del jugador como un cuadrado de 30x30
bool canMoveTo(float centerX, float centerY) {
    float halfSize = 15.0f; // Radio del jugador

    return
        !isWall(centerX - halfSize, centerY - halfSize) &&
        !isWall(centerX + halfSize, centerY - halfSize) &&
        !isWall(centerX - halfSize, centerY + halfSize) &&
        !isWall(centerX + halfSize, centerY + halfSize);
}

// Collision.cpp
bool bulletHitsEnemy(float bulletX, float bulletY, float enemyX, float enemyY, float enemySize) {
    float bulletHalf = 2.0f;
    // enemySize es radio (mitad del ancho/alto)
    return
        bulletX + bulletHalf > enemyX - enemySize &&
        bulletX - bulletHalf < enemyX + enemySize &&
        bulletY + bulletHalf > enemyY - enemySize &&
        bulletY - bulletHalf < enemyY + enemySize;
}




bool isOnPortal(float worldX, float worldY) {
    int tileSize = 40;
    int mapHeight = mapData.size();
    int mapWidth = mapData[0].size();

    int col = static_cast<int>((worldX / tileSize) + mapWidth / 2.0f);
    int row = static_cast<int>((mapHeight / 2.0f) - (worldY / tileSize));

    if (row >= 0 && row < mapHeight && col >= 0 && col < mapWidth) {
        return mapData[row][col] == 'P';
    }
    return false;
}

