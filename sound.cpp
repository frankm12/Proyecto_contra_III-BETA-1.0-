#include "sound.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>

Mix_Chunk* sonidoDisparo = NULL;
Mix_Music* musicaFondo = NULL;   // in-game
Mix_Music* musicaMenu = NULL;    // ===== NUEVO: música del menú

// default 25% al iniciar; lo ajustamos luego desde main
static int g_masterVolumePercent = 25;

static int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
static int toSDLVol(int percent) { return (MIX_MAX_VOLUME * clamp(percent,0,100)) / 100; }

bool initSounds() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cout << "Error al iniciar SDL: " << SDL_GetError() << std::endl;
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "Error al iniciar SDL_mixer: " << Mix_GetError() << std::endl;
        return false;
    }

    sonidoDisparo = Mix_LoadWAV("music/pistol.ogg");
    if (!sonidoDisparo) {
        std::cout << "Error al cargar sonido de disparo: " << Mix_GetError() << std::endl;
        return false;
    }

    // ===== CAMBIO: pista del juego (ya la tenías)
    musicaFondo = Mix_LoadMUS("music/Contra III_song.MP3");
    if (!musicaFondo) {
        std::cout << "Error al cargar música de juego: " << Mix_GetError() << std::endl;
        return false;
    }

    // ===== NUEVO: pista del menú principal
    musicaMenu = Mix_LoadMUS("music/DARKWOOD.MP3"); // <-- agrega este archivo
    if (!musicaMenu) {
        std::cout << "Error al cargar música de menú: " << Mix_GetError() << std::endl;
        return false;
    }

    setMasterVolume(g_masterVolumePercent); // aplica % inicial
    return true;
}

void setMasterVolume(int percent) {
    g_masterVolumePercent = clamp(percent,0,100);
    int vol = toSDLVol(g_masterVolumePercent);
    Mix_VolumeMusic(vol);
    // Ajusta los chunks (disparo) ligeramente más bajo que la música
    Mix_VolumeChunk(sonidoDisparo, (vol * 3) / 4);
}

void playDisparo() {
    Mix_PlayChannel(-1, sonidoDisparo, 0);
}

void stopMusic() {
    Mix_HaltMusic();
}

void playMenuMusic() {
    if (musicaMenu) {
        Mix_HaltMusic();
        Mix_PlayMusic(musicaMenu, -1);
    }
}

void playGameMusic() {
    if (musicaFondo) {
        Mix_HaltMusic();
        Mix_PlayMusic(musicaFondo, -1);
    }
}

void cleanSounds() {
    Mix_FreeChunk(sonidoDisparo);
    Mix_FreeMusic(musicaFondo);
    Mix_FreeMusic(musicaMenu); // ===== NUEVO
    Mix_CloseAudio();
    SDL_Quit();
}


