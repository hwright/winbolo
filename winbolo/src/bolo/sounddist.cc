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
 *Name:          Sound Distancing
 *Filename:      sounddist.c
 *Author:        John Morrison
 *Creation Date: 19/01/99
 *Last Modified: 05/05/01
 *Purpose:
 *  Responsible for differentiating between playing soft
 *  sound effects and loud sound effects.
 *********************************************************/

#include "sounddist.h"

#include "backend.h"
#include "frontend.h"
#include "global.h"
#include "log.h"
#include "screen.h"

/*********************************************************
 *NAME:          clientSoundDist
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/01/99
 *LAST MODIFIED: 05/05/01
 *PURPOSE:
 *  Calculates whether a soft sound of a loud sound should
 *  be played and passes paremeters to frontend
 *
 *ARGUMENTS:
 *  value - Sound effect to be played
 *  mx    - Map X co-ordinatate for the sound origin
 *  my    - Map Y co-ordinatate for the sound origin
 *********************************************************/
void clientSoundDist(bolo::sndEffects value, BYTE mx, BYTE my) {
  BYTE tankX; /* Tank X Map Co-ordinate */
  BYTE tankY; /* Tank Y Map Co-ordinate */
  BYTE gapX;  /* Distance from tank to sound */
  BYTE gapY;

  if (logIsRecording()) {
    soundDistLog(value, mx, my);
  }
  if (bolo::sndEffects::mineExplosionNear == value) {
    gapY = 0;
  }
  screenGetTankMapCoord(&tankX, &tankY);
  /* Get gap */
  if ((tankX - mx) < 0) {
    gapX = mx - tankX;
  } else {
    gapX = tankX - mx;
  }

  if ((tankY - my) < 0) {
    gapY = my - tankY;
  } else {
    gapY = tankY - my;
  }

  if (gapY < SDIST_NONE && gapX < SDIST_NONE) {
    /* Determine whether loud/soft sound should be played */
    switch (value) {
      case bolo::sndEffects::shootNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::shootFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::shootNear);
        }
        break;
      case bolo::sndEffects::shotTreeNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::shotTreeFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::shotTreeNear);
        }
        break;
      case bolo::sndEffects::shotBuildingNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::shotBuildingFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::shotBuildingNear);
        }
        break;
      case bolo::sndEffects::hitTankNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::hitTankFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::hitTankNear);
        }
        break;
      case bolo::sndEffects::tankSinkNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::tankSinkFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::tankSinkNear);
        }
        break;
      case bolo::sndEffects::bigExplosionNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::bigExplosionFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::bigExplosionNear);
        }
        break;
      case bolo::sndEffects::farmingTreeNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::farmingTreeFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::farmingTreeNear);
        }
        break;
      case bolo::sndEffects::manBuildingNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::manBuildingFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::manBuildingNear);
        }
        break;
      case bolo::sndEffects::manDyingNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::manDyingFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::manDyingNear);
        }
        break;
      case bolo::sndEffects::mineExplosionNear:
        if (gapX > SDIST_SOFT || gapY > SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::mineExplosionFar);
        } else {
          screenGetFrontend()->playSound(bolo::sndEffects::mineExplosionNear);
        }
        break;
      case bolo::sndEffects::manLayingMineNear:
        if (gapX <= SDIST_SOFT || gapY <= SDIST_SOFT) {
          screenGetFrontend()->playSound(bolo::sndEffects::manLayingMineNear);
        }
        break;
      case bolo::sndEffects::shootSelf:
      case bolo::sndEffects::shotTreeFar:
      case bolo::sndEffects::shotBuildingFar:
      case bolo::sndEffects::hitTankFar:
      case bolo::sndEffects::hitTankSelf:
      case bolo::sndEffects::manDyingFar:
      case bolo::sndEffects::mineExplosionFar:
      case bolo::sndEffects::bubbles:
      case bolo::sndEffects::tankSinkFar:
      case bolo::sndEffects::bigExplosionFar:
      case bolo::sndEffects::farmingTreeFar:
      case bolo::sndEffects::manBuildingFar:
      case bolo::sndEffects::shootFar:
        break;
    }
  }
}

/*********************************************************
 *NAME:          soundDistLog
 *AUTHOR:        John Morrison
 *CREATION DATE: 05/05/01
 *LAST MODIFIED: 05/05/01
 *PURPOSE:
 *  Calculates the item to be logged
 *
 *ARGUMENTS:
 *  value - Sound effect to be played
 *  mx    - Map X co-ordinatate for the sound origin
 *  my    - Map Y co-ordinatate for the sound origin
 *********************************************************/
void soundDistLog(bolo::sndEffects value, BYTE mx, BYTE my) {
  BYTE logMessageType; /* Log item type */
  bool wantLog;        /* Do we want to log this? */

  wantLog = true;
  switch (value) {
    case bolo::sndEffects::shootSelf:
    case bolo::sndEffects::shootNear:
    case bolo::sndEffects::shootFar:
      logMessageType = log_SoundShoot;
      break;

    case bolo::sndEffects::shotTreeNear:
    case bolo::sndEffects::shotTreeFar:
      logMessageType = log_SoundFarm;
      break;

    case bolo::sndEffects::shotBuildingNear:
    case bolo::sndEffects::shotBuildingFar:
      //    logMessageType = log_SoundHit;
      break;

    case bolo::sndEffects::hitTankNear:
    case bolo::sndEffects::hitTankFar:
    case bolo::sndEffects::hitTankSelf:
      wantLog = false;
      break;
    case bolo::sndEffects::bubbles:
    case bolo::sndEffects::tankSinkNear:
    case bolo::sndEffects::tankSinkFar:
      wantLog = false;
      break;
    case bolo::sndEffects::bigExplosionNear:
      logMessageType = log_SoundExplosion;
      break;
    case bolo::sndEffects::bigExplosionFar:
      logMessageType = log_SoundExplosion;
      break;
    case bolo::sndEffects::farmingTreeNear:
    case bolo::sndEffects::farmingTreeFar:
      logMessageType = log_SoundFarm;
      break;
    case bolo::sndEffects::manBuildingNear:
    case bolo::sndEffects::manBuildingFar:
      logMessageType = log_SoundBuild;
      break;
    case bolo::sndEffects::manDyingNear:
    case bolo::sndEffects::manDyingFar:
      logMessageType = log_SoundManDie;
      break;

    case bolo::sndEffects::manLayingMineNear:
      logMessageType = log_SoundMineLay;
      break;

    case bolo::sndEffects::mineExplosionNear:
    case bolo::sndEffects::mineExplosionFar:
      logMessageType = log_SoundMineExplode;
      break;
  }

  if (wantLog) {
    logAddEvent((logitem)logMessageType, mx, my, 0, 0, 0, nullptr);
  }
}
