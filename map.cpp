// === FILE: Map.cpp ===
#include "map.h"
#include "enemy.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <GL/freeglut.h>
#include <cmath>
#include "globals.h"
#include <algorithm>
#include <cctype>

// Activa esto una sola vez si quieres forzar un boss cerca del spawn cuando no se encontró ninguno.
// #define DEBUG_FORCE_BOSS_IF_MISSING

const int tileSize = 40;

float nextSpawnX = POS_INICIAL_X;
float nextSpawnY = POS_INICIAL_Y;

std::vector<std::string> mapData;
int maxColumns = 0;

static int countInRow(const std::string& row, char c) {
    return (int)std::count(row.begin(), row.end(), c);
}

static void dumpEnemiesSummary(const char* who) {
    int total = (int)enemies.size();
    int bosses = 0, melee=0, basic=0, sniper=0, fast=0, ranged=0;
    for (auto &e : enemies) {
        if (e.type == ENEMY_BOSS) bosses++;
        else if (e.type == ENEMY_MELEE) melee++;
        else if (e.type == ENEMY_BASIC) basic++;
        else if (e.type == ENEMY_SNIPER) sniper++;
        else if (e.type == ENEMY_FAST) fast++;
        else if (e.type == ENEMY_RANGED) ranged++;
    }
    std::cout << "[SUMMARY " << who << "] enemies=" << total
              << " boss=" << bosses
              << " melee=" << melee
              << " basic=" << basic
              << " sniper=" << sniper
              << " fast=" << fast
              << " ranged=" << ranged
              << std::endl;
}

void loadMap(const char* filename) {
    mapData.clear();

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "ERROR: No se pudo abrir el archivo " << filename << std::endl;
        return;
    }

    std::string line;
    size_t maxLength = 0;
    std::vector<std::string> temp;

    int spawnRow = -1, spawnCol = -1;

    while (std::getline(file, line)) {
        // Elimina un posible '\r' final (archivos Windows)
        if (!line.empty() && line.back() == '\r') line.pop_back();

        if (line.length() > maxLength) maxLength = line.length();
        temp.push_back(line);
    }
    file.close();

    for (int row = 0; row < (int)temp.size(); ++row) {
        while (temp[row].length() < maxLength)
            temp[row] += '.';

        for (int col = 0; col < (int)maxLength; ++col) {
            if (temp[row][col] == '@') {
                spawnRow = row;
                spawnCol = col;
                temp[row][col] = '.';
            }
        }
        mapData.push_back(temp[row]);
    }

    maxColumns = (int)maxLength;
    std::cout << "Mapa " << filename << " cargado con " << mapData.size()
              << " filas y " << maxColumns << " columnas.\n";

    if (spawnRow != -1 && spawnCol != -1) {
        nextSpawnX = (spawnCol - maxColumns / 2.0f) * tileSize;
        nextSpawnY = ((int)mapData.size() / 2.0f - spawnRow) * tileSize;
        cameraX = nextSpawnX;
        cameraY = nextSpawnY;
        std::cout << "[DEBUG] Punto de aparición: (" << nextSpawnX << ", " << nextSpawnY << ")\n";
    } else {
        std::cout << "[ADVERTENCIA] No se encontró '@' en el mapa. Usando spawn por defecto.\n";
    }

    // Contadores debug (antes de convertir nada)
    int countK=0, countE=0, countB=0, countS=0, countF=0, countP=0;
    for (auto &row : mapData) {
        countK += countInRow(row, 'K');
        countE += countInRow(row, 'E');
        countB += countInRow(row, 'B');
        countS += countInRow(row, 'S');
        countF += countInRow(row, 'F');
        countP += countInRow(row, 'P');
    }
    std::cout << "[DEBUG] Enemigos en mapa (raw) - K:" << countK
              << " E:" << countE << " B:" << countB
              << " S:" << countS << " F:" << countF
              << " | Portales P:" << countP << std::endl;
}

void initEnemiesFromMap() {
    enemies.clear();

    if (mapData.empty()) {
        std::cerr << "Error: Mapa vacío al generar enemigos\n";
        return;
    }

    const int mapHeight = (int)mapData.size();
    const int mapWidth  = (int)mapData[0].size();

    int bossesFound = 0;
    int cellsVisited = 0;

    for (int row = 0; row < mapHeight; ++row) {
        for (int col = 0; col < mapWidth; ++col) {
            char symbol = mapData[row][col];
            float worldX = (col - mapWidth / 2.0f) * tileSize;
            float worldY = (mapHeight / 2.0f - row) * tileSize;

            // Llama SIEMPRE: addEnemyFromMapChar internamente decide si es válido
            if (addEnemyFromMapChar(symbol, worldX, worldY)) {
                if (std::toupper((unsigned char)symbol) == 'K') {
                    bossesFound++;
                    std::cout << "[DEBUG] Boss agregado desde el mapa en ("
                              << worldX << "," << worldY << ")\n";
                }
                mapData[row][col] = '.'; // marcar procesado
            }
            cellsVisited++;
        }
    }

    cleanupInvalidEnemies();

    std::cout << "[DEBUG] initEnemiesFromMap visitó " << cellsVisited
              << " celdas; bossesFound=" << bossesFound << "\n";

    dumpEnemiesSummary("POST_INIT");

#ifdef DEBUG_FORCE_BOSS_IF_MISSING
    if (bossesFound == 0) {
        std::cout << "[FORCE] No se encontró boss en el mapa; creando uno cerca del spawn\n";
        addEnemy(nextSpawnX + 200.0f, nextSpawnY, ENEMY_BOSS);
        bossAlive = true;
        dumpEnemiesSummary("POST_FORCE");
    }
#endif
}

void drawMap() {
    int mapHeight = (int)mapData.size();
    int mapWidth  = (int)mapData[0].size();

    for (int row = 0; row < mapHeight; ++row) {
        for (int col = 0; col < mapWidth; ++col) {
            char tile = mapData[row][col];
            float posX = (col - mapWidth / 2.0f) * tileSize;
            float posY = (mapHeight / 2.0f - row) * tileSize;

            // Piso
            if (tile != '#') {
                glColor3f(0.15f, 0.15f, 0.15f);
                glBegin(GL_QUADS);
                glVertex2f(posX, posY);
                glVertex2f(posX + tileSize, posY);
                glVertex2f(posX + tileSize, posY - tileSize);
                glVertex2f(posX, posY - tileSize);
                glEnd();
            }

            // Pared
            if (tile == '#') {
                glColor3f(0.4f, 0.4f, 0.4f);
                glBegin(GL_QUADS);
                glVertex2f(posX, posY);
                glVertex2f(posX + tileSize, posY);
                glVertex2f(posX + tileSize, posY - tileSize);
                glVertex2f(posX, posY - tileSize);
                glEnd();
            }

            // Portal
            if (tile == 'P') {
                glColor3f(1.0f, 0.5f, 0.0f);
                glBegin(GL_QUADS);
                glVertex2f(posX, posY);
                glVertex2f(posX + tileSize, posY);
                glVertex2f(posX + tileSize, posY - tileSize);
                glVertex2f(posX, posY - tileSize);
                glEnd();
            }

            // (Opcional) Mostrar K si aún no ha sido procesada (raro, pero útil en debug)
            if (tile == 'K') {
                glColor3f(0.7f, 0.0f, 0.7f);
                glBegin(GL_QUADS);
                glVertex2f(posX, posY);
                glVertex2f(posX + tileSize, posY);
                glVertex2f(posX + tileSize, posY - tileSize);
                glVertex2f(posX, posY - tileSize);
                glEnd();
            }
        }
    }
}


bool placePortalAtWorld(float worldX, float worldY) {
    if (mapData.empty()) return false;
    const int mapHeight = (int)mapData.size();
    const int mapWidth  = (int)mapData[0].size();

    auto toRC = [&](float wx, float wy, int& r, int& c) {
        c = (int)((wx / tileSize) + mapWidth / 2.0f);
        r = (int)((mapHeight / 2.0f) - (wy / tileSize));
    };

    auto tryPut = [&](int r, int c)->bool {
        if (r < 0 || r >= mapHeight || c < 0 || c >= mapWidth) return false;
        if (mapData[r][c] == '#') return false; // no sobre pared
        mapData[r][c] = 'P';
        return true;
    };

    // 1) intenta en la celda exacta
    int row, col; toRC(worldX, worldY, row, col);
    if (tryPut(row, col)) return true;

    // 2) busca cerca (radio pequeño)
    static const int OFFS[][2] = {
        {0,1},{0,-1},{1,0},{-1,0},{1,1},{1,-1},{-1,1},{-1,-1},
        {0,2},{0,-2},{2,0},{-2,0}
    };
    for (auto &o : OFFS) {
        if (tryPut(row + o[0], col + o[1])) return true;
    }
    return false;
}

