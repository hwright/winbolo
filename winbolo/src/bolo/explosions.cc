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
 *Filename:      explosion.c
 *Author:        John Morrison
 *Creation Date: 01/01/99
 *Last Modified: 04/10/03
 *Purpose:
 *  Responsable for Explosions
 *********************************************************/

#include "explosions.h"

#include "frontend.h"
#include "global.h"
#include "rubble.h"
#include "screen.h"
#include "screentypes.h"

/*********************************************************
 *NAME:          explosionsCreate
 *AUTHOR:        John Morrison
 *CREATION DATE:  1/1/99
 *LAST MODIFIED:  1/1/99
 *PURPOSE:
 *  Sets up the explosions data structure
 *
 *ARGUMENTS:
 *  expl - Pointer to the explosions object
 *********************************************************/
void explosionsCreate(explosions *expl) { *expl = new explosionsObj; }

/*********************************************************
 *NAME:          explosionsDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE:  1/1/99
 *LAST MODIFIED:  1/1/99
 *PURPOSE:
 *  Destroys and frees memory for the explosions data
 *  structure
 *
 *ARGUMENTS:
 *  expl - Pointer to the explosions object
 *********************************************************/
void explosionsDestroy(explosions *expl) { delete *expl; }

/*********************************************************
 *NAME:          explosionsAddItem
 *AUTHOR:        John Morrison
 *CREATION DATE:  1/1/99
 *LAST MODIFIED: 15/1/99
 *PURPOSE:
 *  Adds an item to the explosions data structure.
 *
 *ARGUMENTS:
 *  expl   - Pointer to the explosions object
 *  value  - Pointer to the shells data structure
 *  mx     - Map X co-ord of the explosion
 *  my     - Map Y co-ord of the explosion
 *  px     - Pixel X co-ord of the explosion
 *  py     - Pixel Y co-ord of the explosion
 *  startPos  - How far through the explosion does it start
 *********************************************************/
void explosionsAddItem(explosions *expl, BYTE mx, BYTE my, BYTE px, BYTE py,
                       BYTE startPos) {
  (*expl)->explosions_.push_back(
      {.pos = {.x = mx, .y = my}, .px = px, .py = py, .length = startPos});
}

/*********************************************************
 *NAME:          explosionsUpdate
 *AUTHOR:        John Morrison
 *CREATION DATE:  1/1/99
 *LAST MODIFIED:  1/1/99
 *PURPOSE:
 *  Updates each explosion position
 *
 *ARGUMENTS:
 *  expl - Pointer to the explosions object
 *********************************************************/
void explosionsUpdate(explosions *expl) {
  static BYTE update = 0; /* The update time */

  update++;
  if (update != EXPLOAD_UPDATE_TIME) {
    return;
  } else {
    update = 0;
  }

  std::vector<int> to_remove;

  for (int i = 0; i < (*expl)->explosions_.size(); ++i) {
    auto &exp = (*expl)->explosions_[i];
    if (exp.length > EXPLODE_DEATH) {
      exp.length--;
    } else {
      // Remove from data structure
      to_remove.push_back(i);
    }
  }

  // Reverse iterate, because forward iterating would change the indicies
  // of future members
  for (auto it = to_remove.rbegin(); it != to_remove.rend(); ++it) {
    (*expl)->explosions_.erase((*expl)->explosions_.begin() + *it);
  }
}

/*********************************************************
 *NAME:          explosionCalcScreenBullets
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/1/98
 *LAST MODIFIED: 1/1/98
 *PURPOSE:
 *  Adds items to the sceenBullets data structure if they
 *  are on screen
 *
 *ARGUMENTS:
 *  expl       - Pointer to the explosions object
 *  sBullet    - The screenBullets Data structure
 *  leftPos    - X Map offset start
 *  rightPos   - X Map offset end
 *  topPos     - Y Map offset end
 *  bottomPos  - Y Map offset end
 *********************************************************/
void explosionsCalcScreenBullets(explosions *expl,
                                 bolo::ScreenBulletList *sBullets, BYTE leftPos,
                                 BYTE rightPos, BYTE topPos, BYTE bottomPos) {
  for (auto &exp : (*expl)->explosions_) {
    if (exp.pos.x >= leftPos && exp.pos.x < rightPos && exp.pos.y >= topPos &&
        exp.pos.y < bottomPos) {
      sBullets->push_back(bolo::ScreenBullet{
          .pos = {.x = static_cast<uint8_t>(exp.pos.x - leftPos),
                  .y = static_cast<uint8_t>(exp.pos.y - topPos)},
          .px = exp.px,
          .py = exp.py,
          .frame = exp.length});
    }
  }
}
