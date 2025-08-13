#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>

extern std::vector<std::string> mapData;

void loadMap(const char* filename);
void drawMap();
void initEnemiesFromMap();
bool placePortalAtWorld(float worldX, float worldY);


#endif

