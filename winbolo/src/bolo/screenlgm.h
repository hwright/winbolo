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
 *Name:          Screen LGM
 *Filename:      screenLgm.h
 *Author:        John Morrison
 *Creation Date: 19/2/99
 *Last Modified: 19/2/99
 *Purpose:
 *  Responsable for Lgms on the screen
 *********************************************************/

#ifndef SCREENLGMS_H
#define SCREENLGMS_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) == NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ScreenLgmsHeadMX(list) ((list)->mx);
#define ScreenLgmsHeadMY(list) ((list)->my);
#define ScreenLgmsHeadPX(list) ((list)->px);
#define ScreenLgmsHeadPY(list) ((list)->py);
#define ScreenLgmsHeadFrame(list) ((list)->frame);
#define ScreenLgmsTail(list) ((list)->next);

/* Type structure */

namespace bolo {

struct ScreenLgm {
  // The map co-ordinate it is on
  MapPoint pos;
  uint8_t px;     // The pixel offset from the left it is on
  uint8_t py;     // The pixel offset from the top it is on
  uint8_t frame;  // The direction it is facing
};

struct ScreenLgmList {
  std::vector<ScreenLgm> lgms_;

  //  Prepares the screenLgms data structure prior to displaying
  //
  // ARGUMENTS:
  //  leftPos   - Left bounds of the screen
  //  rightPos  - Right bounds of the screen
  //  top    - Top bounds of the screen
  //  bottom - Bottom bounds of the screen
  void prepare(uint8_t leftPos, uint8_t rightPos, uint8_t top, uint8_t bottom);
};

}  // namespace bolo

#endif /* SCREENLGMS_H */
