#ifndef SPRITE_LOGO_H
#define SPRITE_LOGO_H

#include <string>

// Carga el sprite desde un .txt (con letras de color)
void cargarSpriteLogo(const char* path);

// Dibuja el sprite centrado en pantalla usando coordenadas de HUD (0..ancho, 0..alto).
// - centerX: centro horizontal (normalmente currentWindowWidth/2)
// - bottomY: coordenada Y donde se apoyará la base del sprite (píxeles de pantalla)
// - maxWidthPx / maxHeightPx: tamaño máximo permitido (para escalar el sprite y que quepa)
void drawSpriteLogoCentered(int centerX, int bottomY, int maxWidthPx, int maxHeightPx);

#endif

