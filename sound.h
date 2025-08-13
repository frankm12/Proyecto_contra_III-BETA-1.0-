#ifndef SOUND_H
#define SOUND_H

bool initSounds();
void playDisparo();

void playMenuMusic();
void playGameMusic();
void stopMusic();

void setMasterVolume(int percent);

void cleanSounds();

#endif

