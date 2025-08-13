#include <GL/freeglut.h>
#include "player.h"
#include "globals.h"
#include <cmath>
#include <fstream>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

extern float cameraX;
extern float cameraY;
extern int mouseX;
extern int mouseY;
std::vector<std::string> spriteJugador;

void cargarSpriteJugador(const char* path) {
    spriteJugador.clear();
    std::ifstream archivo(path);
    std::string linea;
    while (std::getline(archivo, linea)) {
        spriteJugador.push_back(linea);
    }
}


void drawPlayer() {
    if (invulnerableTimer > 0 && (invulnerableTimer / 5) % 2 == 0)
        return;

    float px = cameraX;
    float py = cameraY;
    float angle = playerAngle * 180.0f / M_PI;
    float recoilOffset = (recoilTimer > 0) ? -3.0f : 0.0f;

    glPushMatrix();
    glTranslatef(px, py, 0);
    glRotatef(playerAngle * 180.0f / M_PI, 0, 0, 1);

    glTranslatef(0, recoilOffset, 0);  // aplicar recoil hacia atrás
    
    const int CENTRO_FILA = 8;   // línea 9
	const int CENTRO_COLUMNA = 12; // columna 13

    float cellSize = 1.6f; // tamaño de cada celda en unidades del mundo
    float ancho = spriteJugador[0].length();
    float alto = spriteJugador.size();
 
	float origenX = -CENTRO_COLUMNA * cellSize;
	float origenY = CENTRO_FILA * cellSize;

    for (size_t fila = 0; fila < spriteJugador.size(); ++fila) {
        for (size_t col = 0; col < spriteJugador[fila].length(); ++col) {
            char c = spriteJugador[fila][col];
            if (c == '.') continue;

            // --- COLORES ---
            if (c == 'X') glColor3f(0.0f, 0.0f, 0.0f);
            else if (c == 'H') glColor3f(0.23f, 0.16f, 0.15f);
            else if (c == 'C') glColor3f(0.69f, 0.43f, 0.24f);
            else if (c == 'V') glColor3f(0.95f, 0.74f, 0.64f);
            else if (c == 'P') glColor3f(0.70f, 0.49f, 0.49f);
            else if (c == 'G') glColor3f(0.27f, 0.27f, 0.27f);
            else if (c == 'W') glColor3f(1.0f, 1.0f, 1.0f);
            else if (c == 'R') glColor3f(1.0f, 0.0f, 0.0f);
            else if (c == 'B') glColor3f(0.0f, 0.3f, 1.0f);
            else if (c == 'L') glColor3f(0.7f, 0.7f, 0.7f);
            else if (c == 'Y') glColor3f(1.0f, 1.0f, 0.0f);
            else if (c == 'M') glColor3f(1.0f, 0.0f, 1.0f);
            else if (c == 'T') glColor3f(0.0f, 0.5f, 0.0f);
            else if (c == 'Z') glColor3f(0.5f, 1.0f, 0.5f);
            else if (c == 'O') glColor3f(1.0f, 0.5f, 0.0f);
            else if (c == 'A') glColor3f(0.0f, 0.0f, 0.5f);
            else glColor3f(1.0f, 1.0f, 1.0f); // por defecto

            float x = origenX + col * cellSize;
            float y = origenY - fila * cellSize;

            glBegin(GL_QUADS);
            glVertex2f(x, y);
            glVertex2f(x + cellSize, y);
            glVertex2f(x + cellSize, y - cellSize);
            glVertex2f(x, y - cellSize);
            glEnd();
        }
    }

    glPopMatrix();
}




void drawHealthBar() {
    float barWidth = 30.0f;
    float barHeight = 5.0f;
    float px = cameraX;
    float py = cameraY;

    float xLeft = px - barWidth / 2.0f;
    float yTop = py - 20.0f; // Justo debajo del jugador

    // Fondo de la barra (rojo)
    glColor3f(0.3f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(xLeft, yTop);
    glVertex2f(xLeft + barWidth, yTop);
    glVertex2f(xLeft + barWidth, yTop - barHeight);
    glVertex2f(xLeft, yTop - barHeight);
    glEnd();

    // Parte verde proporcional a la vida
    float healthRatio = playerHealth / 3.0f; // 3 es la vida máxima
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(xLeft, yTop);
    glVertex2f(xLeft + barWidth * healthRatio, yTop);
    glVertex2f(xLeft + barWidth * healthRatio, yTop - barHeight);
    glVertex2f(xLeft, yTop - barHeight);
    glEnd();
}

