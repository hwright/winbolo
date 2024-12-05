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

/* Defines */
/* Empty / Non Empty / Head / Tail Macros */
#define IsEmpty(list) ((list) == NULL)
#define NonEmpty(list) (!IsEmpty(list))
#define AllienceHead(list) ((list)->playerNum);
#define AllienceTail(list) ((list)->next);

/* Allience */

typedef struct allianceObj *alliance;
struct allianceObj {
  alliance next;  /* Next item */
  BYTE playerNum; /* The player this person is allied to */
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

/*********************************************************
 *NAME:          allianceMakeLogAlliance
 *AUTHOR:        John Morrison
 *CREATION DATE: 25/07/04
 *LAST MODIFIED: 25/07/04
 *PURPOSE:
 * Creates the alliance log buffer. Returns the length
 * of the buffer. Format is buff[0] number of allies
 * each byte after is the alliance number.
 *
 *
 *ARGUMENTS:
 *  value - The alliance structure to remove from
 *  buff  - Buffer to write into
 *********************************************************/
BYTE allianceMakeLogAlliance(alliance *value, BYTE *buff);

std::unordered_set<BYTE> allianceGetAllies(alliance *value);

#endif /* ALLIANCE_H */
