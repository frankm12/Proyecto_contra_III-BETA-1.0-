#include <GL/freeglut.h>

float cameraX = 0.0f;
float cameraY = 0.0f;

void drawPlayer() {
    glColor3f(1, 0, 0); // rojo
    glRectf(-10, -10, 10, 10); // centrado
}

void drawWorld() {
    // Dibujar fondo
    glColor3f(0.2f, 0.2f, 0.2f); // gris oscuro
    for (int x = -1000; x < 1000; x += 100) {
        for (int y = -1000; y < 1000; y += 100) {
            glRectf(x, y, x + 90, y + 90);
        }
    }

    // Dibujar un enemigo (ejemplo)
    glColor3f(0, 1, 0); // verde
    glRectf(100, 100, 120, 120);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Mover el mundo en base a la cámara
    glPushMatrix();
    glTranslatef(-cameraX, -cameraY, 0);
    drawWorld();
    glPopMatrix();

    // Dibujar el jugador centrado en la pantalla
    drawPlayer();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'w') cameraY -= 10;
    if (key == 's') cameraY += 10;
    if (key == 'a') cameraX -= 10;
    if (key == 'd') cameraX += 10;
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(800, 600);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutCreateWindow("Contra Top-Down Prototype");
    glOrtho(-400, 400, -300, 300, -1, 1); // cámara ortográfica
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

