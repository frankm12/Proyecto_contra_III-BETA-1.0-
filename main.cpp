	#include <GL/freeglut.h>
	#include "player.h"
	#include "map.h"
	#include "bullet.h"
	#include <iostream>
	#include "collision.h"
	#include "enemy.h"
	#include "globals.h"
	#include <cmath>
	#include <SDL.h>
	#include <SDL_mixer.h> // o solo <SDL_mixer.h> si lo dejaste en include raíz
	#include "sound.h"
	#include "draw.h"
	#include "input.h" 
	#include "enemy_bullet.h"
	#include "sprite_logo.h"
	
	
	void ReiniciarJuego(); // ?? Declaración anticipada
	void drawGameOverMenu(); 
	
	int score = 0;
	int playerHealth = 3; // Comienza con 3 vidas
	int invulnerableTimer = 0;
	int recoilTimer = 0;
	bool enPausa = false;
	int opcionSeleccionada = 0; // 0 = Continuar, 1 = Reiniciar, etc.
	const char* opcionesMenu[] = {
	    "Continuar",
	    "Reiniciar nivel",
	    "Musica: Activada",
	    "Salir del juego"
	};
	const int totalOpciones = 4;
	float fadeAlpha = 0.0f;
	bool enFade = false;
	FadeEstado estadoFade = NINGUNO;
	int tutorialPaso = 1;   // Comienza mostrando el primer mensaje
	int tutorialTimer = 0;  // Empieza en 0
	float tutorialAlpha = 0.0f;
	int tutorialEstado = 0;
	bool gameOverActivo = false;
	float gameOverAlpha = 0.0f;
	int opcionGameOver = 0;
	int gameOverTimer = 0;
	bool reinicioPendiente = false;
	
	bool bossAlive = false;
	bool bossExploding = false;
	int  bossExplodeTimer = 0;
	float bossDeathX = 0.0f, bossDeathY = 0.0f;
	
	bool creditsActive = false;
	int  creditsTimer = 0;
	
	bool portalSpawned = false;
	bool goToCreditsPending = false;
	
	GameState gameState = STATE_MENU;
	int mainMenuIndex = 0;      // 0 Iniciar, 1 Ajustes, 2 Salir
	int settingsMenuIndex = 0;  // 0 Volumen, 1 Volver
	int masterVolumePercent = 25;
	bool startGamePending = false;
	
	int nivelActual = 1;
	const int TOTAL_NIVELES = 3;
	bool cambioNivelPendiente = false;

	bool isFullScreen = true;
	int windowedWidth = 800;
	int windowedHeight = 600;
	int windowPosX = 100;
	int windowPosY = 100;
	int currentWindowWidth = 800;
	int currentWindowHeight = 600;
	
	
	// Cámara
	float cameraX = 0.0f;
	float cameraY = 0.0f;
	float playerSpeed = 4.0f;
	float playerAngle = 0.0f;
	const float POS_INICIAL_X = -1512;
	const float POS_INICIAL_Y = 164;
	
	
	// Teclas presionadas
	bool keys[256] = { false };
	
	int mouseX = 0;
	int mouseY = 0;
	
	
	void drawFadeOverlay() {
	    if (!enFade) return;
	
	    glDisable(GL_TEXTURE_2D);
	    
	    glMatrixMode(GL_PROJECTION);
	    glPushMatrix();
	    glLoadIdentity();
	    gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
	    
	    glMatrixMode(GL_MODELVIEW);
	    glPushMatrix();
	    glLoadIdentity();
	
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	    glColor4f(0.0f, 0.0f, 0.0f, fadeAlpha);
	
	    glBegin(GL_QUADS);
	        glVertex2f(0, 0);
	        glVertex2f(currentWindowWidth, 0);
	        glVertex2f(currentWindowWidth, currentWindowHeight);
	        glVertex2f(0, currentWindowHeight);
	    glEnd();
	
	    glDisable(GL_BLEND);
	
	    glPopMatrix();
	    glMatrixMode(GL_PROJECTION);
	    glPopMatrix();
	    glMatrixMode(GL_MODELVIEW);
	
	    glEnable(GL_TEXTURE_2D);
	}
	
	
	void reshape(int width, int height) {
	    currentWindowWidth = width;
	    currentWindowHeight = height;
	
	    glViewport(0, 0, width, height);
	
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	
	    // Mantener la relación 4:3 sin deformar (800x600)
	    float aspect = width / (float)height;
	    if (aspect >= (800.0f / 600.0f)) {
	        // Pantalla más ancha que 4:3
	        glOrtho(-400.0f * aspect * (600.0f / 800.0f), 400.0f * aspect * (600.0f / 800.0f),
	                -300.0f, 300.0f, -1.0f, 1.0f);
	    } else {
	        // Pantalla más alta que 4:3
	        glOrtho(-400.0f, 400.0f,
	                -300.0f / aspect * (800.0f / 600.0f), 300.0f / aspect * (800.0f / 600.0f),
	                -1.0f, 1.0f);
	    }
	
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	}
	
	void menuKeys(unsigned char key) {
	    if (key == 'w') {
	        opcionSeleccionada--;
	        if (opcionSeleccionada < 0) opcionSeleccionada = totalOpciones - 1;
	    } else if (key == 's') {
	        opcionSeleccionada++;
	        if (opcionSeleccionada >= totalOpciones) opcionSeleccionada = 0;
	    } else if (key == 13) { // ENTER
	        if (opcionSeleccionada == 0) {
	            enPausa = false; // Continuar
	        } else if (opcionSeleccionada == 1) {
				    reinicioPendiente = true;  // <- FALTABA ESTO
				    enFade = true;
				    estadoFade = FADE_OUT;
				    fadeAlpha = 0.0f;
				}
				
				else if (opcionSeleccionada == 2) {
	            if (Mix_PausedMusic()) {
	                Mix_ResumeMusic();
	                opcionesMenu[2] = "Musica: Activada";
	            } else {
	                Mix_PauseMusic();
	                opcionesMenu[2] = "Musica: Desactivada";
	            }
	        } else if (opcionSeleccionada == 3) {
	            exit(0);
	        }
	    }
	}
	
	void drawHUD() {
	    // Cambia a proyección 2D para HUD
	    glMatrixMode(GL_PROJECTION);
	    glPushMatrix();
	    glLoadIdentity();
	    gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight); // Coordenadas de pantalla
	    glMatrixMode(GL_MODELVIEW);
	    glPushMatrix();
	    glLoadIdentity();
	
	    // Dibujar el HUD en la esquina superior izquierda
	    glColor3f(1, 1, 1); // Blanco
	    glRasterPos2i(10, currentWindowHeight - 20); // 10 px a la derecha, 20 px debajo del top
	
	    std::string hudText = "Score: " + std::to_string(score);
	    for (char c : hudText)
	        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
	
	    // Restaurar la proyección original
	    glPopMatrix(); // MODELVIEW
	    glMatrixMode(GL_PROJECTION);
	    glPopMatrix();
	    glMatrixMode(GL_MODELVIEW); // Volver a modo normal
	}
		
	// FPS / Lógica de movimiento

	void timer(int value) {
	    // 1) PAUSA (solo aplica en juego y si no hay fade)
	    if (gameState == STATE_GAME && enPausa && !enFade) {
	        glutPostRedisplay();
	        glutTimerFunc(16, timer, 0);
	        return;
	    }
	
	    // 2) REINICIO / CAMBIO DE NIVEL (cuando el fade está en negro)
	    if (reinicioPendiente && fadeAlpha >= 1.0f) {
	        ReiniciarJuego();
	        reinicioPendiente = false;
	        estadoFade = FADE_IN;
	    }
		else if (gameState == STATE_GAME && cambioNivelPendiente && fadeAlpha >= 1.0f) {
		
		    if (goToCreditsPending) {
		        // Final del juego: créditos
		        std::cout << "[END] Mostrando CREDITOS.\n";
		        creditsActive = true;
		        gameState = STATE_MENU;     // El dibujado de créditos tiene prioridad en display()
		        playMenuMusic();
		
		        // limpiar flags
		        goToCreditsPending = false;
		        cambioNivelPendiente = false;
		        portalSpawned = false;
		
		        estadoFade = FADE_IN;       // regreso desde negro
		    } else {
		        if (nivelActual > TOTAL_NIVELES) nivelActual = 1;
		
		        std::string nombreMapa = (nivelActual == 1)
		            ? "map.txt"
		            : "map" + std::to_string(nivelActual) + ".txt";
		
		        std::cout << "[DEBUG] Cargando nivel " << nivelActual << ": " << nombreMapa << std::endl;
		
		        enemies.clear();
		        bullets.clear();
		        enemyBullets.clear();
		
		        loadMap(nombreMapa.c_str());
		        initEnemiesFromMap();
		
		        playerHealth = 3;
		
		        estadoFade = FADE_IN;
		        cambioNivelPendiente = false;
		        portalSpawned = false; // por si venías de un boss previo
		    }
		}

	
	    // 3) FADES (menu<->juego y transiciones)
	    if (enFade) {
	        if (estadoFade == FADE_OUT) {
	            fadeAlpha += 0.02f;
	            if (fadeAlpha >= 1.0f) {
	                fadeAlpha = 1.0f;
	
	                // MENÚ -> JUEGO
	                if (startGamePending && gameState == STATE_MENU) {
	                    nivelActual = 1;
	
	                    enemies.clear();
	                    bullets.clear();
	                    enemyBullets.clear();
	
	                    loadMap("map.txt");
	                    initEnemiesFromMap();
	                    playerHealth = 3;
	
	                    // NO resetees boss aquí
	                    playGameMusic();
	                    gameState = STATE_GAME;
	                    startGamePending = false;
	                }
	
	                estadoFade = FADE_IN;
	            }
	        } else if (estadoFade == FADE_IN) {
	            fadeAlpha -= 0.02f;
	            if (fadeAlpha <= 0.0f) {
	                fadeAlpha = 0.0f;
	                enFade = false;
	                estadoFade = NINGUNO;
	                if (gameState == STATE_GAME) enPausa = false;
	            }
	        }
	    }
	
	    // ============================================================
	    // 4) LÓGICA DE JUEGO POR FRAME (solo cuando estás jugando)
	    // ============================================================
	    if (gameState == STATE_GAME && !enPausa) {
	        // --- Movimiento WASD ---
	        float mx = 0.0f, my = 0.0f;
	        auto down = [&](int c){ return keys[c] != 0; };
	
	        if (down('w') || down('W')) my += playerSpeed;
	        if (down('s') || down('S')) my -= playerSpeed;
	        if (down('a') || down('A')) mx -= playerSpeed;
	        if (down('d') || down('D')) mx += playerSpeed;
	
	        // Normalizar para diagonales
	        float len = std::sqrt(mx*mx + my*my);
	        if (len > 0.0f) {
	            mx = (mx / len) * playerSpeed;
	            my = (my / len) * playerSpeed;
	        }
	
	        // Movimiento con colisión (separado por ejes)
	        if (mx != 0.0f) {
	            float nx = cameraX + mx;
	            if (canMoveTo(nx, cameraY)) cameraX = nx;
	        }
	        if (my != 0.0f) {
	            float ny = cameraY + my;
	            if (canMoveTo(cameraX, ny)) cameraY = ny;
	        }
	
	        // Timers del jugador
	        if (invulnerableTimer > 0) invulnerableTimer--;
	        if (recoilTimer > 0)       recoilTimer--;
	
	        // Actualizaciones de juego
	        updateBullets();
	        updateEnemyBullets();
	        updateEnemies(cameraX, cameraY);
	
	        // (Opcional) game over
	        if (!gameOverActivo && playerHealth <= 0) {
	            gameOverActivo = true;
	            gameOverAlpha = 0.0f;
	            opcionGameOver = 0;
	        }
	
			if (bossExploding) {
			    if (bossExplodeTimer > 0) {
			        bossExplodeTimer--;
			    } else {
			        // terminó la explosión
			        bossExploding = false;
			
			        // Sólo spawnea portal al matar boss del nivel final
			        if (nivelActual == TOTAL_NIVELES && !portalSpawned) {
			            bool ok = placePortalAtWorld(bossDeathX, bossDeathY);
			            portalSpawned = ok;
			            std::cout << (ok ? "[BOSS] Portal 'P' colocado para ir a CREDITOS.\n"
			                              : "[BOSS] No pude colocar 'P' cerca, revisa el terreno.\n");
			        }
			    }
			}

	    }
	    
		if (gameState == STATE_GAME && !enPausa && !enFade && !cambioNivelPendiente) {
		    if (isOnPortal(cameraX, cameraY)) {
		        std::cout << "[PORTAL] Jugador sobre 'P'\n";
		        enFade = true;
		        estadoFade = FADE_OUT;
		        fadeAlpha = 0.0f;
		        cambioNivelPendiente = true;
		
		        if (nivelActual < TOTAL_NIVELES) {
		            nivelActual++;               // seguir al próximo mapa
		        } else {
		            goToCreditsPending = true;   // en lugar de cargar mapa, mostrar créditos
		        }
		    }
		}

	
	    // --- FRAME TICK ---
	    glutPostRedisplay();
	    glutTimerFunc(16, timer, 0);  // ~60 FPS
	}
	

	void drawCenteredText(const char* txt, int y) {
	    int w = 0;
	    for (const char* p = txt; *p; ++p) w += glutBitmapWidth(GLUT_BITMAP_HELVETICA_18, *p);
	    glRasterPos2i((currentWindowWidth - w) / 2, y);
	    for (const char* p = txt; *p; ++p) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
	}
	
	void drawMainMenu() {
	    glMatrixMode(GL_PROJECTION);
	    glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
	    glMatrixMode(GL_MODELVIEW);
	    glPushMatrix(); glLoadIdentity();
	    
	    
	    // 1) DIBUJAR LOGO CENTRADO ARRIBA
	    //    - Queremos que el sprite aparezca centrado, y su BASE esté ~200 px por encima del centro
	    //    - Ajusta estos números si quieres el logo más cerca/lejos de las opciones
	    int centerX = currentWindowWidth / 2;
	    int baseY   = currentWindowHeight - 600; // base del logo
	    int maxW    = (int)(currentWindowWidth * 1.2f); // ancho máximo 60% pantalla
	    int maxH    = (int)(currentWindowHeight * 0.90f); // alto máximo 35% pantalla
	
	    drawSpriteLogoCentered(centerX, baseY, maxW, maxH);
	
	    // Título
	    //glColor3f(1,1,1);
	    //drawCenteredText("== CONTRA TOP-DOWN ==", currentWindowHeight - 120);
	
	    const char* items[] = {"Iniciar juego", "Ajustes", "Salir"};
	    for (int i=0;i<3;++i) {
	        if (i == mainMenuIndex) glColor3f(1,1,0);
	        else glColor3f(1,1,1);
	        drawCenteredText(items[i], currentWindowHeight/2 - i*30);
	    }
	
	    glPopMatrix();
	    glMatrixMode(GL_PROJECTION);
	    glPopMatrix();
	    glMatrixMode(GL_MODELVIEW);
	}
	
	void drawSettingsMenu() {
	    glMatrixMode(GL_PROJECTION);
	    glPushMatrix(); glLoadIdentity(); gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
	    glMatrixMode(GL_MODELVIEW);
	    glPushMatrix(); glLoadIdentity();
	
	    glColor3f(1,1,1);
	    drawCenteredText("== AJUSTES ==", currentWindowHeight - 120);
	
	    // Opción 0: Volumen
	    if (settingsMenuIndex == 0) glColor3f(1,1,0); else glColor3f(1,1,1);
	    std::string vol = "Volumen: " + std::to_string(masterVolumePercent) + "%  (A/D o ?/?)";
	    drawCenteredText(vol.c_str(), currentWindowHeight/2);
	
	    // Opción 1: Volver
	    if (settingsMenuIndex == 1) glColor3f(1,1,0); else glColor3f(1,1,1);
	    drawCenteredText("Volver", currentWindowHeight/2 - 40);
	
	    glPopMatrix();
	    glMatrixMode(GL_PROJECTION);
	    glPopMatrix();
	    glMatrixMode(GL_MODELVIEW);
	}
	
		static void drawBossExplosion() {
	    if (!bossExploding) return;
	
	    // Proyección mundo (ya estás en mundo al llamar)
	    int rings = 5;
	    float t = (float)bossExplodeTimer / 90.0f; // 1 -> 0
	    float maxR = 180.0f;
	
	    for (int i=0;i<rings;i++) {
	        float f = (i+1)/(float)rings;
	        float r = (1.0f - t) * maxR * f;
	        glColor4f(1.0f, 0.6f * (1.0f - f), 0.1f, 0.8f * (1.0f - t));
	        glBegin(GL_LINE_LOOP);
	        const int seg = 36;
	        for (int k=0;k<seg;k++){
	            float ang = (2.0f*3.1415926f*k)/seg;
	            glVertex2f(bossDeathX + cos(ang)*r, bossDeathY + sin(ang)*r);
	        }
	        glEnd();
	    }
	}

	
	void display() {
	    glClear(GL_COLOR_BUFFER_BIT);
	// --- Créditos en pantalla (tiene prioridad sobre el juego) ---
	if (creditsActive) {
	    drawCreditsScreen();
	    drawFadeOverlay();
	    glutSwapBuffers();
	    return;
	}

	    
	if (gameState == STATE_MENU) {
        drawMainMenu();
        drawFadeOverlay();
        glutSwapBuffers();
        return;
    } else if (gameState == STATE_SETTINGS) {
        drawSettingsMenu();
        drawFadeOverlay();
        glutSwapBuffers();
        return;
    }
	
	    // --- Siempre dibujar el mundo ---
	    glPushMatrix();
	    glTranslatef(-cameraX, -cameraY, 0); // Mover el mundo
	
	    drawMap();        // Suelo y paredes
	    drawBullets();    // Disparos
	    drawEnemyBullets();
	    drawPlayer();     // Jugador
	    drawHealthBar();  // Barra de vida
	    drawEnemies();    // Enemigos
	    drawBossExplosion();
	    drawHUD();        // Puntaje
	    drawGameOverScreen(); // ya lo tienes
		drawGameOverMenu();   // <-- agrégalo justo después
	
	    glPopMatrix(); // Fin del modo mundo
	
	    // --- Mostrar menú de pausa solo si no estamos en fade ---
	    if (enPausa && !enFade) {
	        glMatrixMode(GL_PROJECTION);
	        glPushMatrix();
	        glLoadIdentity();
	        gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
	
	        glMatrixMode(GL_MODELVIEW);
	        glPushMatrix();
	        glLoadIdentity();
	
	        int anchoRecuadro = 300;
	        int altoRecuadro = totalOpciones * 40 + 40;
	        int xCentro = currentWindowWidth / 2;
	        int yCentro = currentWindowHeight / 2;
	
	        int xRecuadro = xCentro - anchoRecuadro / 2;
	        int yRecuadro = yCentro + altoRecuadro / 2;
	
	        // Fondo del recuadro
	        glColor3f(0.05f, 0.05f, 0.05f);
	        glBegin(GL_QUADS);
	        glVertex2i(xRecuadro, yRecuadro);
	        glVertex2i(xRecuadro + anchoRecuadro, yRecuadro);
	        glVertex2i(xRecuadro + anchoRecuadro, yRecuadro - altoRecuadro);
	        glVertex2i(xRecuadro, yRecuadro - altoRecuadro);
	        glEnd();
	
	        // Borde
	        glColor3f(0.6f, 0.6f, 0.6f);
	        glBegin(GL_LINE_LOOP);
	        glVertex2i(xRecuadro, yRecuadro);
	        glVertex2i(xRecuadro + anchoRecuadro, yRecuadro);
	        glVertex2i(xRecuadro + anchoRecuadro, yRecuadro - altoRecuadro);
	        glVertex2i(xRecuadro, yRecuadro - altoRecuadro);
	        glEnd();
	
	        // Título
	        glColor3f(1.0f, 1.0f, 1.0f);
	        glRasterPos2i(xCentro - 60, yRecuadro - 30);
	        const char* titulo = "== PAUSA ==";
	        for (int i = 0; titulo[i] != '\0'; ++i)
	            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, titulo[i]);
	
	        // Opciones
	        int yTexto = yRecuadro - 70;
	        for (int i = 0; i < totalOpciones; i++) {
	            if (i == opcionSeleccionada)
	                glColor3f(1.0f, 1.0f, 0.0f); // Amarillo
	            else
	                glColor3f(1.0f, 1.0f, 1.0f); // Blanco
	
	            glRasterPos2i(xCentro - 100, yTexto - i * 30);
	            const char* texto = opcionesMenu[i];
	            for (int j = 0; texto[j] != '\0'; ++j)
	                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, texto[j]);
	        }
	
	        glPopMatrix(); // MODELVIEW
	        glMatrixMode(GL_PROJECTION);
	        glPopMatrix();
	        glMatrixMode(GL_MODELVIEW);
	    }
	
	    // --- Dibujar overlay de fade si está activo ---
	    drawFadeOverlay();
	    drawLoadingLevelText();
	    drawTutorialText();
	    glutSwapBuffers();
	}
	
	
	void drawGameOverMenu() {
	    if (!gameOverActivo) return;
	
	    glMatrixMode(GL_PROJECTION);
	    glPushMatrix();
	    glLoadIdentity();
	    gluOrtho2D(0, currentWindowWidth, 0, currentWindowHeight);
	
	    glMatrixMode(GL_MODELVIEW);
	    glPushMatrix();
	    glLoadIdentity();
	
	    int xCentro = currentWindowWidth / 2;
	    int yCentro = currentWindowHeight / 2;
	
	    const char* titulo = "GAME OVER";
	    const char* opciones[] = { "Reintentar", "Salir" };
	
	    // Fondo oscuro del menú
	    glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
	    glBegin(GL_QUADS);
	        glVertex2f(0, 0);
	        glVertex2f(currentWindowWidth, 0);
	        glVertex2f(currentWindowWidth, currentWindowHeight);
	        glVertex2f(0, currentWindowHeight);
	    glEnd();
	
	    // Texto de título
	    glColor3f(1, 0, 0);
	    glRasterPos2i(xCentro - 50, yCentro + 50);
	    for (const char* c = titulo; *c; c++)
	        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	
	    // Opciones
	    for (int i = 0; i < 2; i++) {
	        if (i == opcionGameOver)
	            glColor3f(1, 1, 0); // Amarillo para opción seleccionada
	        else
	            glColor3f(1, 1, 1); // Blanco normal
	
	        glRasterPos2i(xCentro - 40, yCentro - i * 30);
	        for (const char* c = opciones[i]; *c; c++)
	            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
	    }
	
	    glPopMatrix();
	    glMatrixMode(GL_PROJECTION);
	    glPopMatrix();
	    glMatrixMode(GL_MODELVIEW);
	}
	
	
	void ReiniciarJuego() {
	    // ?? Limpiar enemigos y balas antes de cargar
	    enemies.clear();
	    bullets.clear();
	    enemyBullets.clear();
	
	    cameraX = POS_INICIAL_X;
	    cameraY = POS_INICIAL_Y;
	    playerHealth = 3;
	    score = 0;
	
	    // Cargar mapa correcto
	    std::string nombreMapa;
	    if (nivelActual == 1) {
	        nombreMapa = "map.txt";
	    } else {
	        nombreMapa = "map" + std::to_string(nivelActual) + ".txt";
	    }
	
	    std::cout << "[DEBUG] Reiniciando nivel " << nivelActual << ": " << nombreMapa << std::endl;
	
	    // Cargar mapa y enemigos
	    loadMap(nombreMapa.c_str());
	    initEnemiesFromMap();
	
	    // Resetear estado del boss
	    bossAlive = false;
	    bossExploding = false;
	    bossExplodeTimer = 0;
	
	    // Reiniciar HUD y tutorial
	    gameOverAlpha = 0.0f;
	    opcionGameOver = 0;
	    tutorialPaso = 1;
	    tutorialAlpha = 0.0f;
	    tutorialEstado = 0;
	    tutorialTimer = 0;
	
	    // Música
	    Mix_ResumeMusic();
	    gameOverActivo = false;
	}

	// Al soltar una tecla
	void keyboardUp(unsigned char key, int x, int y) {
	    keys[key] = false;
	}
	
	int main(int argc, char** argv) {
	    glutInit(&argc, argv);
	    glutInitWindowSize(800, 600);
	    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	    glutCreateWindow("Contra Top-Down");
	    glutFullScreen(); // ?? Esto activa pantalla completa
	
	    // Proyección ortográfica
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
		glOrtho(-400, 400, -300, 300, -1, 1);
	
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	
	    // Callbacks
	    glutDisplayFunc(display);
	    // Ruta al .txt del logo del menú (ajústala a tu archivo)
		cargarSpriteLogo("Sprite_inicio/contra3_logo_300x300_transparent_bg_redwhite.txt");
		glutKeyboardFunc(procesarTecla);
		glutKeyboardUpFunc(procesarTeclaUp);
		glutSpecialFunc(procesarTeclaEspecial);
		glutMouseFunc(procesarMouseClick);
		glutPassiveMotionFunc(procesarMouseMove);
	    glutReshapeFunc(reshape);
	    glutTimerFunc(0, timer, 0);

		cargarSpriteJugador("personajes/Personaje_principal.txt");
	    // Cargar mapa


	    //loadMap("map.txt");
	    //initEnemiesFromMap();

	    //std::cout << "Tile inicial: " << isWall(cameraX, cameraY) << std::endl;
	
	    // Centrar la cámara
	    //cameraX = POS_INICIAL_X;
		//cameraY = POS_INICIAL_Y;

	    if (!initSounds()) return 1;
	    
	       // ===== NUEVO: arrancar en MENÚ con música de menú
	    gameState = STATE_MENU;
	    masterVolumePercent = 25;   // volumen inicial
	    setMasterVolume(masterVolumePercent);
	    playMenuMusic();
	    
		//playMusic();
	
	    glutMainLoop();
		cleanSounds();
	
	    return 0;
	}

