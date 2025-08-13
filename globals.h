// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

extern int playerHealth; // ?? Declaración externa
extern float cameraX;
extern float cameraY;
extern int score;
extern int invulnerableTimer; // En frames (~60 por segundo)
extern float playerAngle;
extern int currentWindowWidth;
extern int currentWindowHeight;
extern int recoilTimer;
extern bool enPausa;
extern int opcionSeleccionada;
extern const float POS_INICIAL_X;
extern const float POS_INICIAL_Y;
extern float fadeAlpha;
extern bool enFade;
enum FadeEstado { NINGUNO, FADE_OUT, FADE_IN };
extern FadeEstado estadoFade;
extern int tutorialPaso;       // 1 = primer texto, 2 = segundo texto, 3 = terminado
extern float tutorialAlpha;    // transparencia del texto (0.0 a 1.0)
extern int tutorialEstado;     // 0 = fade-in, 1 = espera, 2 = fade-out
extern int tutorialTimer;      // cuenta frames para la espera
extern bool gameOverActivo;
extern float gameOverAlpha;
extern int opcionGameOver; // 0 = Reintentar, 1 = Salir
extern int gameOverTimer;
extern bool reinicioPendiente;
extern bool isFullScreen;
extern int windowedWidth;
extern int windowedHeight;
extern int windowPosX;
extern int windowPosY;
extern int nivelActual;          // 1, 2, 3, etc.
extern const int TOTAL_NIVELES;  // Cantidad total de niveles
extern bool cambioNivelPendiente;

enum GameState { STATE_MENU = 0, STATE_SETTINGS = 1, STATE_GAME = 2 };
extern GameState gameState;

extern int mainMenuIndex;        // 0..2 (Iniciar, Ajustes, Salir)
extern int settingsMenuIndex;    // 0..1 (Volumen, Volver)
extern int masterVolumePercent;  // 0..100

extern bool startGamePending;    // para hacer fade del menú -> juego

// globals.h (agrega estas extern nuevas)
extern bool bossAlive;            // ¿existe boss en este nivel?
extern bool bossExploding;        // ¿mostrando animación de explosión?
extern int  bossExplodeTimer;     // frames de explosión
extern float bossDeathX, bossDeathY; // dónde murió

// estado de créditos
extern bool creditsActive;
extern int  creditsTimer;

extern bool portalSpawned;        // ¿ya puse la P post-boss?
extern bool goToCreditsPending;   // ¿en lugar de cargar siguiente nivel, voy a créditos?




#endif

