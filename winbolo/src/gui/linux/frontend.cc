/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "frontend.h"

#include <SDL.h>

#include "draw.h"
#include "sound.h"

extern uint64_t dwSysFrame;
extern bool soundEffects;
extern bool showPillLabels;
extern bool showBaseLabels;

namespace bolo {

void LinuxFrontend::updateTankStatusBars(uint8_t shells, uint8_t mines,
                                         uint8_t armour, uint8_t trees) {
  uint64_t tick;

  tick = SDL_GetTicks();
  drawStatusTankBars(0, 0, shells, mines, armour, trees);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::updateBaseStatusBars(uint8_t shells, uint8_t mines,
                                         uint8_t armour) {
  uint64_t tick;

  tick = SDL_GetTicks();
  drawStatusBaseBars(0, 0, shells, mines, armour, FALSE);
  dwSysFrame += (SDL_GetTicks() - tick);
}

void LinuxFrontend::playSound(sndEffects value) {
  if (soundEffects) {
    soundPlayEffect(value);
  }
}

void LinuxFrontend::drawMainScreen(screen *value, screenMines *mineView,
                                   screenTanks *tks, screenGunsight *gs,
                                   screenBullets *sBullet, screenLgm *lgms,
                                   long srtDelay, bool isPillView, tank *tank,
                                   int edgeX, int edgeY) {
  BYTE cursorX;
  BYTE cursorY;
  bool showCursor;

  showCursor = screenGetCursorPos(&cursorX, &cursorY);
  ::drawMainScreen(value, mineView, tks, gs, sBullet, lgms, showPillLabels,
                   showBaseLabels, srtDelay, isPillView, edgeX, edgeY,
                   showCursor, cursorX, cursorY);
}

}  // namespace bolo
