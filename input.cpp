// input.cpp
#include <GL/freeglut.h>
#include "input.h"
#include "globals.h"
#include "bullet.h"
#include "sound.h"
#include "collision.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>   // <-- necesario para std::max / std::min
#include "enemy.h"
#include <SDL.h>
#include <SDL_mixer.h>

// Está definida en Main.cpp
extern bool keys[256];
extern int mouseX;
extern int mouseY;

// ---------------------- MENÚ PRINCIPAL ----------------------
void mainMenuKeys(unsigned char key) {
    if (key == 'w') {
        mainMenuIndex = (mainMenuIndex + 3 - 1) % 3; // 0..2
    } else if (key == 's') {
        mainMenuIndex = (mainMenuIndex + 1) % 3;
    } else if (key == 13) { // ENTER
        if (mainMenuIndex == 0) {          // Iniciar juego
            startGamePending = true;
            enFade = true;
            estadoFade = FADE_OUT;
            fadeAlpha = 0.0f;
        } else if (mainMenuIndex == 1) {   // Ajustes
            gameState = STATE_SETTINGS;
        } else if (mainMenuIndex == 2) {   // Salir
            exit(0);
        }
    } else if (key == 27) { // ESC en menú: salir
        exit(0);
    }
}

// ---------------------- AJUSTES ----------------------
void settingsMenuKeys(unsigned char key) {
    if (key == 'w') {
        settingsMenuIndex = (settingsMenuIndex + 2 - 1) % 2; // 0..1
    } else if (key == 's') {
        settingsMenuIndex = (settingsMenuIndex + 1) % 2;
    } else if (key == 'a' && settingsMenuIndex == 0) {
        masterVolumePercent = std::max(0, masterVolumePercent - 5);
        setMasterVolume(masterVolumePercent);
    } else if (key == 'd' && settingsMenuIndex == 0) {
        masterVolumePercent = std::min(100, masterVolumePercent + 5);
        setMasterVolume(masterVolumePercent);
    } else if (key == 13) { // ENTER
        if (settingsMenuIndex == 1) { // Volver
            gameState = STATE_MENU;
        }
    } else if (key == 27) { // ESC = volver
        gameState = STATE_MENU;
    }
}

void procesarTecla(unsigned char key, int x, int y) {
    // --- Créditos: solo se manejan aquí (Enter/Esc normales) ---
    if (creditsActive) {
        if (key == 13 || key == 27) { // ENTER o ESC
            creditsActive = false;
            gameState = STATE_MENU;
            enFade = true;
            estadoFade = FADE_OUT;
            fadeAlpha = 0.0f; // transición suave
            playMenuMusic();
        }
        return;
    }

    // ----- Ruteo por estado global -----
    if (gameState == STATE_MENU) {
        mainMenuKeys(key);
        return;
    } else if (gameState == STATE_SETTINGS) {
        settingsMenuKeys(key);
        return;
    }

    // ===== De aquí en adelante: STATE_GAME =====
    if (gameOverActivo) {
        if (key == 'w') {
            opcionGameOver = (opcionGameOver - 1 + 2) % 2;
        } else if (key == 's') {
            opcionGameOver = (opcionGameOver + 1) % 2;
        } else if (key == 13) { // Enter
            if (opcionGameOver == 0) { // Reintentar
                reinicioPendiente = true;
                fadeAlpha = 0.0f;
                enFade = true;
                estadoFade = FADE_OUT;
            } else {
                exit(0);
            }
        }
        return;
    }

    if (enPausa) {
        // Menú de pausa
        pauseMenuKeys(key);
        return;
    }

    keys[key] = true;

    if (key == 32) { // SPACE: disparo
        float worldMouseX = (mouseX - currentWindowWidth / 2.0f) + cameraX;
        float worldMouseY = -(mouseY - currentWindowHeight / 2.0f) + cameraY;

        float offset = 65.0f * 0.6f;
        float bulletX = cameraX + cos(playerAngle) * offset;
        float bulletY = cameraY + sin(playerAngle) * offset;

        recoilTimer = 5;
        shootBullet(bulletX, bulletY, worldMouseX, worldMouseY);
        playDisparo();
    }

    if (key == 27) { // ESC: toggle pausa SOLO en juego
        enPausa = !enPausa;
        return;
    }
}

void procesarTeclaUp(unsigned char key, int x, int y) {
    keys[key] = false;
}

void procesarTeclaEspecial(int key, int x, int y) {
    // OJO: GLUT NO entrega Enter/Esc aquí. Solo flechas/F1..F12/etc.
    // Navegación con flechas en menús
    if (gameState == STATE_MENU) {
        if (key == GLUT_KEY_UP)   mainMenuIndex = (mainMenuIndex + 3 - 1) % 3;
        if (key == GLUT_KEY_DOWN) mainMenuIndex = (mainMenuIndex + 1) % 3;
        return;
    }
    if (gameState == STATE_SETTINGS) {
        if (key == GLUT_KEY_UP)   settingsMenuIndex = (settingsMenuIndex + 2 - 1) % 2;
        if (key == GLUT_KEY_DOWN) settingsMenuIndex = (settingsMenuIndex + 1) % 2;

        if (settingsMenuIndex == 0) {
            if (key == GLUT_KEY_LEFT) {
                masterVolumePercent = std::max(0, masterVolumePercent - 5);
                setMasterVolume(masterVolumePercent);
            } else if (key == GLUT_KEY_RIGHT) {
                masterVolumePercent = std::min(100, masterVolumePercent + 5);
                setMasterVolume(masterVolumePercent);
            }
        }
        return;
    }

    // ===== De aquí en adelante: STATE_GAME =====
    if (gameOverActivo) {
        if (key == GLUT_KEY_UP)   opcionGameOver = (opcionGameOver - 1 + 2) % 2;
        if (key == GLUT_KEY_DOWN) opcionGameOver = (opcionGameOver + 1) % 2;
        return;
    }

    if (key == GLUT_KEY_F12) {
        if (isFullScreen) {
            glutReshapeWindow(windowedWidth, windowedHeight);
            glutPositionWindow(windowPosX, windowPosY);
            isFullScreen = false;
        } else {
            glutFullScreen();
            isFullScreen = true;
        }
    }
}

void procesarMouseClick(int button, int state, int x, int y) {
    if (gameState != STATE_GAME) return;
    if (gameOverActivo) return;
    if (enPausa) return;

    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float worldMouseX = (x - currentWindowWidth / 2.0f) + cameraX;
        float worldMouseY = -(y - currentWindowHeight / 2.0f) + cameraY;

        float offset = 65.0f * 0.6f;
        float bulletX = cameraX + cos(playerAngle) * offset;
        float bulletY = cameraY + sin(playerAngle) * offset;

        recoilTimer = 5;
        shootBullet(bulletX, bulletY, worldMouseX, worldMouseY);
        playDisparo();
    }
}

void procesarMouseMove(int x, int y) {
    if (gameState != STATE_GAME) return;
    if (gameOverActivo) return;
    if (enPausa) return;

    mouseX = x;
    mouseY = y;

    float worldMouseX = (x - currentWindowWidth / 2.0f) + cameraX;
    float worldMouseY = -(y - currentWindowHeight / 2.0f) + cameraY;

    float dx = worldMouseX - cameraX;
    float dy = worldMouseY - cameraY;
    playerAngle = atan2(dy, dx);
}

void pauseMenuKeys(unsigned char key) {
    if (key == 'w') {
        opcionSeleccionada--;
        if (opcionSeleccionada < 0) opcionSeleccionada = 3; // totalOpciones-1
    } else if (key == 's') {
        opcionSeleccionada++;
        if (opcionSeleccionada > 3) opcionSeleccionada = 0; // wrap
    } else if (key == 13) { // ENTER
        if (opcionSeleccionada == 0) {          // Continuar
            enPausa = false;
        } else if (opcionSeleccionada == 1) {   // Reiniciar nivel
            reinicioPendiente = true;
            enFade = true;
            estadoFade = FADE_OUT;
            fadeAlpha = 0.0f;
        } else if (opcionSeleccionada == 2) {   // Música ON/OFF
            if (Mix_PausedMusic()) {
                Mix_ResumeMusic();
            } else {
                Mix_PauseMusic();
            }
        } else if (opcionSeleccionada == 3) {   // Salir
            exit(0);
        }
    }
}

