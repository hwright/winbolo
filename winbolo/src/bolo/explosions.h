/*
 * $Id$
 *
 * Copyright (c) 1998-2008 John Morrison.
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

/*********************************************************
 *Name:          Explosion
 *Filename:      explosion.h
 *Author:        John Morrison
 *Creation Date:  1/1/99
 *Last Modified: 15/1/98
 *Purpose:
 *  Responsable for Explosions
 *********************************************************/

#ifndef EXPLOSIONS_H
#define EXPLOSIONS_H

#include "global.h"
#include "screentypes.h"

#define EXPLOSION_START 8
#define EXPLODE_DEATH 1
/* Explosions should only be updated every 3 ticks */
#define EXPLOAD_UPDATE_TIME 3

namespace bolo {

struct Explosion {
  MapPoint pos;
  uint8_t px;
  uint8_t py;
  uint8_t length;
};

class ExplosionTracker {
 public:
  ExplosionTracker() = default;

  // Move-only
  ExplosionTracker(ExplosionTracker &) = delete;
  ExplosionTracker &operator=(ExplosionTracker &) = delete;

  // Adds an item to the explosions data structure.
  //
  // ARGUMENTS:
  //  mx     - Map X co-ord of the explosion
  //  my     - Map Y co-ord of the explosion
  //  px     - Pixel X co-ord of the explosion
  //  py     - Pixel Y co-ord of the explosion
  //  startPos  - How far through the explosion does it start
  void addItem(MapPoint pos, uint8_t px, uint8_t py, uint8_t startPos);

  //  Updates each explosion position
  void Update();

  //  Adds items to the sceenBullets data structure if they
  //  are on screen
  //
  // ARGUMENTS:
  //  sBullet    - The screenBullets Data structure
  //  leftPos    - X Map offset start
  //  rightPos   - X Map offset end
  //  topPos     - Y Map offset end
  //  bottomPos  - Y Map offset end
  void calcScreenBullets(bolo::ScreenBulletList *sBullets, uint8_t leftPos,
                         uint8_t rightPos, uint8_t topPos, uint8_t bottomPos);

 private:
  std::vector<Explosion> explosions_;
};

}  // namespace bolo

#endif /* EXPLOSIONS_H */
