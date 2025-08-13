#ifndef INPUT_H
#define INPUT_H

void procesarTecla(unsigned char key, int x, int y);
void procesarTeclaUp(unsigned char key, int x, int y);
void procesarTeclaEspecial(int key, int x, int y);
void procesarMouseClick(int button, int state, int x, int y);
void procesarMouseMove(int x, int y);

void pauseMenuKeys(unsigned char key);

// ===== NUEVO: handlers de menús =====

void settingsMenuKeys(unsigned char key);

#endif

