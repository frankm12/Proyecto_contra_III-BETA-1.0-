#include <GL/freeglut.h>
#include "globals.h"
#include <string>

extern int nivelActual;
extern bool cambioNivelPendiente;

void drawTutorialText() {
    if (tutorialPaso >= 3) return;

    const char* mensaje = NULL;

    if (tutorialPaso == 1)
        mensaje = "Presiona WASD para moverte";
    else if (tutorialPaso == 2)
        mensaje = "Usa clic para disparar";

    if (!mensaje) return;

    // Proyección 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, tutorialAlpha); // Usar alpha

	    // Calcular ancho real del texto (en píxeles) usando ancho de fuente
	int textWidth = 0;
	for (int i = 0; mensaje[i] != '\0'; ++i)
	    textWidth += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, mensaje[i]);
	
	// Posicionar el texto centrado horizontalmente
	glRasterPos2i((currentWindowWidth - textWidth) / 2, currentWindowHeight / 2 + 200);


    for (int i = 0; mensaje[i] != '\0'; ++i)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, mensaje[i]);

    glDisable(GL_BLEND);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawGameOverScreen() {
    if (!gameOverActivo) return;

    // Fade-in
    if (gameOverAlpha < 1.0f)
        gameOverAlpha += 0.01f;

    // Proyección 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Fondo negro con transparencia
    glColor4f(0, 0, 0, gameOverAlpha * 0.7f);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(currentWindowWidth, 0);
    glVertex2i(currentWindowWidth, currentWindowHeight);
    glVertex2i(0, currentWindowHeight);
    glEnd();

    // Texto "GAME OVER"
    const char* texto = "GAME OVER";
    int ancho = 0;
    for (int i = 0; texto[i]; ++i)
        ancho += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, texto[i]);

    glColor4f(1, 1, 1, gameOverAlpha);
    glRasterPos2i((currentWindowWidth - ancho) / 2, currentWindowHeight / 2 + 80);
    for (int i = 0; texto[i]; ++i)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[i]);

    // Opciones
    const char* opciones[] = { "Reintentar", "Salir" };
    for (int i = 0; i < 2; ++i) {
        if (i == opcionGameOver)
            glColor4f(1.0f, 1.0f, 0.0f, gameOverAlpha); // Amarillo
        else
            glColor4f(1.0f, 1.0f, 1.0f, gameOverAlpha);

        int anchoOp = 0;
        for (int j = 0; opciones[i][j]; ++j)
            anchoOp += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, opciones[i][j]);

        glRasterPos2i((currentWindowWidth - anchoOp) / 2, currentWindowHeight / 2 - i * 40);
        for (int j = 0; opciones[i][j]; ++j)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, opciones[i][j]);
    }

    glDisable(GL_BLEND);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawLoadingLevelText() {
    if (!cambioNivelPendiente) return;

    std::string texto = "1-" + std::to_string(nivelActual);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4f(1, 1, 1, 1);

    int ancho = 0;
    for (char c : texto)
        ancho += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, c);

    glRasterPos2i((currentWindowWidth - ancho) / 2, currentWindowHeight / 2);
    for (char c : texto)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// Draw.cpp (al final, similar a las otras pantallas)
void drawCreditsScreen() {
    // Fondo 2D
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // fondo
    glColor4f(0, 0, 0, 0.85f);
    glBegin(GL_QUADS);
    glVertex2i(0,0); glVertex2i(currentWindowWidth,0);
    glVertex2i(currentWindowWidth,currentWindowHeight); glVertex2i(0,currentWindowHeight);
    glEnd();

    // título
    const char* titulo = "CREDITOS";
    int w=0; for (const char* p=titulo; *p; ++p) w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *p);
    glColor3f(1,1,1);
    glRasterPos2i((currentWindowWidth - w)/2, currentWindowHeight - 80);
    for (const char* p=titulo; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);

    // lista (pon lo tuyo)
    const char* lines[] = {
        "Franklyn D.",
        "Computacion Grafica",
        "Maestro: Richard Rijo",
        "Motor: FreeGLUT + OpenGL",
        "Agradecimientos: Familia & Amigos",
        "",
        "Presiona ENTER o ESC para volver al Menu"
    };
    int y = currentWindowHeight - 140;
    for (int i=0;i<7;++i) {
        int ww=0; for (const char* p=lines[i]; *p; ++p) ww += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *p);
        glRasterPos2i((currentWindowWidth - ww)/2, y - i*30);
        for (const char* p=lines[i]; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }

    glDisable(GL_BLEND);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


