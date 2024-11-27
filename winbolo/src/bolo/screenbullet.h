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
 *Name:          Screen Bullets
 *Filename:      screenbullet.h
 *Author:        John Morrison
 *Creation Date: 25/12/98
 *Last Modified: 25/12/98
 *Purpose:
 *  Responsable for Shells tracking/collision detect etc.
 *********************************************************/

#ifndef SCREENBULLETS_H
#define SCREENBULLETS_H

#include "global.h"

/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) == NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define ScreenBulletHeadMX(list) ((list)->mx);
#define ScreenBulletHeadMY(list) ((list)->my);
#define ScreenBulletHeadPX(list) ((list)->px);
#define ScreenBulletHeadPY(list) ((list)->py);
#define ScreenBulletHeadDir(list) ((list)->dir);
#define ScreenBulletTail(list) ((list)->next);

namespace bolo {

struct ScreenBullet {
  // Map point of the bullet (mapped to screen)
  MapPoint pos;
  uint8_t px;     // X Pixel offset of the bullet
  uint8_t py;     // Y Pixel offset of the bullet
  uint8_t frame;  // Frame identifier type
};

using ScreenBulletList = std::vector<ScreenBullet>;

}  // namespace bolo

#endif /* SCREENBULLETS_H */
