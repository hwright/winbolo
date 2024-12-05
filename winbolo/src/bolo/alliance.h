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
 *Name:          Allience
 *Filename:      Allience.h
 *Author:        John Morrison
 *Creation Date: 18/02/99
 *Last Modified: 25/07/04
 *Purpose:
 *  Handles alliance. Who is allied to who etc. Uses a
 *  simple set
 *********************************************************/

#ifndef ALLIANCE_H
#define ALLIANCE_H

#include <unordered_set>

#include "global.h"

/* Alliance */

typedef struct allianceObj *alliance;
struct allianceObj {
  std::unordered_set<BYTE> players;
};

/* Prototypes */

/*********************************************************
 *NAME:          allianceCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Creates an alliance struncture
 *
 *ARGUMENTS:
 *
 *********************************************************/
alliance allianceCreate(void);

/*********************************************************
 *NAME:          allianceDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Destroys a alliance structure
 *
 *ARGUMENTS:
 *  value - The alliance structure to destroy
 *********************************************************/
void allianceDestroy(alliance *value);

/*********************************************************
 *NAME:          allianceAdd
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Adds an player to an alliance (if doesn't exist already)
 *
 *ARGUMENTS:
 *  value     - The alliance structure to add to
 *  playerNum - The player number to add
 *********************************************************/
void allianceAdd(alliance *value, BYTE playerNum);

/*********************************************************
 *NAME:          allianceRemove
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Removes an player from an alliance
 *
 *ARGUMENTS:
 *  value     - The alliance structure to remove from
 *  playerNum - The player number to add
 *********************************************************/
void allianceRemove(alliance *value, BYTE playerNum);

/*********************************************************
 *NAME:          allianceExist
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Returns whether a player number exist in this alliance
 *
 *ARGUMENTS:
 *  value     - The alliance structure to remove from
 *  playerNum - The player number to add
 *********************************************************/
bool allianceExist(alliance *value, BYTE playerNum);

/*********************************************************
 *NAME:          allianceNumAllies
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Returns the number of allies a player has
 *
 *ARGUMENTS:
 *  value     - The alliance structure to remove from
 *********************************************************/
BYTE allianceNumAllies(alliance *value);

std::unordered_set<BYTE> allianceGetAllies(alliance *value);

#endif /* ALLIANCE_H */
