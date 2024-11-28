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
 *Name:          Labels
 *Filename:      label.c
 *Author:        John Morrison
 *Creation Date:  2/2/99
 *Last Modified:  2/2/99
 *Purpose:
 *  Responsable for message labels (short/long etc).
 *********************************************************/

#include "labels.h"

#include <string.h>

#include <format>

#include "global.h"

static bool labelOwnTank = true;           /* Should own tank be labeled? */
static labelLen labelMessage = lblShort;   /* Should message labels be short? */
static labelLen labelTankLabel = lblShort; /* Should tank labels be short? */

/*********************************************************
 *NAME:          labelSetSenderLength
 *AUTHOR:        John Morrison
 *CREATION DATE:  2/2/99
 *LAST MODIFIED:  2/2/99
 *PURPOSE:
 * Sets the message sender length item (short/long)
 *
 *ARGUMENTS:
 *  isLengthShort - true if the length is to be short
 *********************************************************/
void labelSetSenderLength(labelLen isLengthShort) {
  labelMessage = isLengthShort;
}

/*********************************************************
 *NAME:          labelSetTankLength
 *AUTHOR:        John Morrison
 *CREATION DATE:  2/2/99
 *LAST MODIFIED:  2/2/99
 *PURPOSE:
 * Sets the tank label length (short/long)
 *
 *ARGUMENTS:
 *  isLengthShort - true if the length is to be short
 *********************************************************/
void labelSetTankLength(labelLen isLengthShort) {
  labelTankLabel = isLengthShort;
}

/*********************************************************
 *NAME:          labelSetLabelOwnTank
 *AUTHOR:        John Morrison
 *CREATION DATE:  2/2/99
 *LAST MODIFIED:  2/2/99
 *PURPOSE:
 * Sets the tank label length (short/long)
 *
 *ARGUMENTS:
 *  labelOwn - true if you should label your own tank
 *********************************************************/
void labelSetLabelOwnTank(bool labelOwn) { labelOwnTank = labelOwn; }

std::string labelMakeMessage(std::string_view name, std::string_view loc) {
  std::string res;
  if (labelMessage != lblNone) {
    res = name;
    if (labelMessage == lblLong) {
      res.append(std::format("@{}", loc));
    }
  }
  return res;
}

std::string labelMakeTankLabel(std::string_view name, std::string_view loc,
                               bool isOwn) {
  std::string res;
  if (labelTankLabel != lblNone && (!isOwn || (isOwn && labelOwnTank))) {
    res = name;
    if (labelTankLabel == lblLong) {
      res.append(std::format("@{}", loc));
    }
  }
  return res;
}
