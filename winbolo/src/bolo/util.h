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
 *Name:          Util
 *Filename:      util.h
 *Author:        John Morrison
 *Creation Date: 25/12/98
 *Last Modified: 30/01/02
 *Purpose:
 *  Provides misc functions
 *********************************************************/

#ifndef UTILS_H
#define UTILS_H

#define BRADIAN_ADD8 8
#define NIBBLE_SHIFT_SIZE 4

/* Includes */
#include <cstdint>
#include <string>

#include "global.h"
#include "types.h"

/*********************************************************
 *NAME:          tankCalcDistance
 *AUTHOR:        John Morrison
 *CREATION DATE: 25/12/98
 *LAST MODIFIED: 25/12/98
 *PURPOSE:
 * Calculates the X and Y distance an object
 * should move from a given speed and angle
 *
 *ARGUMENTS:
 *  xAmount - The amount to add in the X direction
 *  yAmount - The amount to add in the Y direction
 *  angle   - The angle the tank is facing
 *  speed   - The speed of the tank
 *********************************************************/
void utilCalcDistance(int *xAmount, int *yAmount, TURNTYPE angle, int speed);

/*********************************************************
 *NAME:          utilGetDir
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/98
 *LAST MODIFIED: 26/11/98
 *PURPOSE:
 *  Converts BRadians to 16 direction (0-16)
 *
 *ARGUMENTS:
 *  value - The angle in brandians
 *********************************************************/
BYTE utilGetDir(TURNTYPE value);

/*********************************************************
 *NAME:          utilGet16Dir
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/11/98
 *LAST MODIFIED: 27/11/98
 *PURPOSE:
 *  Converts BRadians to nearest 16 direction
 *
 *ARGUMENTS:
 *  value - The angle in brandians
 *********************************************************/
BYTE utilGet16Dir(TURNTYPE value);

/*********************************************************
 *NAME:          tankGetWorld
 *AUTHOR:        John Morrison
 *CREATION DATE:  2/1/99
 *LAST MODIFIED:  2/1/99
 *PURPOSE:
 *  Returns whether the tank has been hit or not. If it
 *
 *ARGUMENTS:
 *  xTank     - X Co-ordinate of the tank
 *  yTank     - Y Co-ordinate of the tank
 *  tankAngle - Angle of the tank
 *  x         - X co-ord of shell
 *  y         - Y co-ord of shell
 *  angle     - The direction the shell came from
 *********************************************************/
bool utilIsTankHit(WORLD xTank, WORLD yTank, TURNTYPE tankAngle, WORLD x,
                   WORLD y, TURNTYPE angle);

/*********************************************************
 *NAME:          utilCalcAngle
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/99
 *LAST MODIFIED: 18/1/99
 *PURPOSE:
 *  Returns the angle from object 1 to object 2
 *
 *ARGUMENTS:
 *  object1X - X Location of object1
 *  object1Y - Y Location of object1
 *  object2X - X Location of object2
 *  object2Y - Y Location of object1
 *********************************************************/
TURNTYPE utilCalcAngle(WORLD object1X, WORLD object1Y, WORLD object2X,
                       WORLD object2Y);

/*********************************************************
 *NAME:          utilIsItemInRange
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/12/98
 *LAST MODIFIED:  23/9/00
 *PURPOSE:
 * Returns whether an item is in range of range.
 * (Remained from utilIsTankInRange)
 *
 *ARGUMENTS:
 *  x      - Pillbox world X co-ord
 *  y      - Pillbox world X co-ord
 *  tankX  - Tank world X co-ord
 *  tankY  - Tank world Y co-ord
 *  range  - Range to check in range
 *  amount - Stores copy of the distance if in range
 *********************************************************/
bool utilIsItemInRange(WORLD x, WORLD y, WORLD tankX, WORLD tankY, WORLD range,
                       double *amount);

/*********************************************************
 *NAME:          utilIsTankInTrees
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/01/99
 *LAST MODIFIED: 22/04/01
 *PURPOSE:
 * Returns whether a tank is hidden (ie surrounded) by
 * trees
 *
 *ARGUMENTS:
 *  mp  - Pointer to the map structure
 *  pb  - Pointer to the pillbox structure
 *  bs  - Pointer to the bases structure
 *  wx  - Tank X world position
 *  wy  - Tank Y world position
 *********************************************************/
bool utilIsTankInTrees(map *mp, pillboxes *pb, bases *bs, WORLD wx, WORLD wy);

namespace bolo {

// Converts Bolo's network pascal string to C strings
//
// ARGUMENTS:
//  src  - Source string
//  dest - Destination string
//
// RETURNS:
//  The converted C string
std::string utilPtoCString(std::string_view src);

// Converts a C string to a Bolo's network pascal string
//
// ARGUMENTS:
//  src  - Source string
//  dest - Destination string
//
// RETURNS:
//  The converted Pascal string
std::string utilCtoPString(std::string_view src);

// Return the high and low nibbles out of a byte
//
// ARGUMENTS:
//  value - The byte the nibbles come from
//
// RETURNS:
//  A tuple whose first element is the high nibble, and second element
//  is the low nibble
inline std::tuple<uint8_t, uint8_t> utilGetNibbles(uint8_t value) {
  return std::make_tuple((value & 0xF0) >> 4, value & 0x0F);
}

// Return the high and low nibbles as a combined byte.
//
// ARGUMENTS:
//  high - High nibble
//  low  - Low nibble
//
// RETURNS:
//  The combined byte
inline uint8_t utilPutNibble(uint8_t high, uint8_t low) {
  return (high << 4) & low;
}

// Return the map name from a file name and path.
//
// ARGUMENTS:
//  fileName - Map File name and path.
//  mapName  - Stores the Map Name.
//
// RETURNS:
//  The filename without path.  If the filename ends in `.map`, it is removed.
std::string utilExtractMapName(std::string_view fileName);

}  // namespace bolo

/*********************************************************
 *NAME:          utilStripNameReplace
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/01/02
 *LAST MODIFIED: 30/01/02
 *PURPOSE:
 * Strips tab and space charectors from the start/end of
 * the string name. If resulting string is empty copies
 * in "Me"
 *
 *ARGUMENTS:
 *  name - String to remove from
 *********************************************************/
void utilStripNameReplace(char *name);

/*********************************************************
 *NAME:          utilStripName
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/01/02
 *LAST MODIFIED: 30/01/02
 *PURPOSE:
 * Strips tab and space charectors from the start/end of
 * the string name
 *
 *ARGUMENTS:
 *  name - String to remove from
 *********************************************************/
void utilStripName(char *name);

#endif /* UTILS_H */
