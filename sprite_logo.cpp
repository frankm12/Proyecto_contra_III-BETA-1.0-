#include "sprite_logo.h"
#include <vector>
#include <string>
#include <fstream>
#include <GL/freeglut.h>

static std::vector<std::string> g_logo;

void cargarSpriteLogo(const char* path) {
    g_logo.clear();
    std::ifstream f(path);
    std::string line;
    while (std::getline(f, line)) {
        g_logo.push_back(line);
    }
}

static void setColorForChar(char c) {
    // === Mapeo de colores EXACTO al que usas en tus sprites ===
    if (c == 'X') glColor3f(0.0f, 0.0f, 0.0f);           // Negro
    else if (c == 'H') glColor3f(0.23f, 0.16f, 0.15f);   // Marrón oscuro
    else if (c == 'C') glColor3f(0.69f, 0.43f, 0.24f);   // Marrón claro
    else if (c == 'V') glColor3f(0.95f, 0.74f, 0.64f);   // Piel clara
    else if (c == 'P') glColor3f(0.70f, 0.49f, 0.49f);   // Sombra de piel
    else if (c == 'G') glColor3f(0.27f, 0.27f, 0.27f);   // Gris oscuro

    // Colores adicionales
    else if (c == 'W') glColor3f(1.0f, 1.0f, 1.0f);       // Blanco
    else if (c == 'R') glColor3f(1.0f, 0.0f, 0.0f);       // Rojo
    else if (c == 'B') glColor3f(0.0f, 0.3f, 1.0f);       // Azul
    else if (c == 'L') glColor3f(0.7f, 0.7f, 0.7f);       // Gris claro
    else if (c == 'Y') glColor3f(1.0f, 1.0f, 0.0f);       // Amarillo
    else if (c == 'M') glColor3f(1.0f, 0.0f, 1.0f);       // Magenta
    else if (c == 'T') glColor3f(0.0f, 0.5f, 0.0f);       // Verde oscuro
    else if (c == 'Z') glColor3f(0.5f, 1.0f, 0.5f);       // Verde claro
    else if (c == 'O') glColor3f(1.0f, 0.5f, 0.0f);       // Naranja
    else if (c == 'A') glColor3f(0.0f, 0.0f, 0.5f);       // Azul oscuro
    else glColor3f(1.0f, 1.0f, 1.0f);                     // Default
}

void drawSpriteLogoCentered(int centerX, int bottomY, int maxWidthPx, int maxHeightPx) {
    if (g_logo.empty()) return;

    const int rows = (int)g_logo.size();
    const int cols = (int)g_logo[0].size();
    if (rows == 0 || cols == 0) return;

    // Calcula el tamaño de celda para que quepa dentro de (maxWidthPx x maxHeightPx)
    float cellSize = (float)maxWidthPx / (float)cols;
    float cellSizeH = (float)maxHeightPx / (float)rows;
    if (cellSizeH < cellSize) cellSize = cellSizeH; // respetar la más restrictiva

    // Tamaño final del sprite en píxeles de pantalla
    float spriteW = cols * cellSize;
    float spriteH = rows * cellSize;

    // Queremos centrar horizontalmente y apoyar la base en bottomY
    float x0 = centerX - spriteW / 2.0f;
    float y0 = (float)bottomY; // base
    float yTop = y0 + spriteH; // comenzamos a dibujar desde la fila superior

    // Dibuja en coordenadas de HUD (ya deben estar activadas fuera)
    for (int r = 0; r < rows; ++r) {
        float y = yTop - r * cellSize;
        for (int c = 0; c < cols; ++c) {
            char ch = g_logo[r][c];
            if (ch == '.') continue; // transparente

            setColorForChar(ch);

            float x = x0 + c * cellSize;
            glBegin(GL_QUADS);
            glVertex2f(x,        y);
            glVertex2f(x+cellSize, y);
            glVertex2f(x+cellSize, y - cellSize);
            glVertex2f(x,        y - cellSize);
            glEnd();
        }
    }
}

