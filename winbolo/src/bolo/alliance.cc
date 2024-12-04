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
 *Filename:      Allience.c
 *Author:        John Morrison
 *Creation Date: 18/02/99
 *Last Modified: 25/07/04
 *Purpose:
 *  Handles alliance. Who is allied to who etc. Uses a
 *  simple set
 *********************************************************/

#include "alliance.h"

#include "global.h"

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
alliance allianceCreate(void) { return nullptr; }

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
void allianceDestroy(alliance *value) {
  alliance q;

  while (!IsEmpty(*value)) {
    q = *value;
    *value = AllienceTail(q);
    delete q;
  }
}

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
void allianceAdd(alliance *value, BYTE playerNum) {
  alliance q;

  if (!allianceExist(value, playerNum)) {
    /* Doesn't exist yet. Add */
    q = new allianceObj;
    q->playerNum = playerNum;
    q->next = *value;
    *value = q;
  }
}

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
void allianceRemove(alliance *value, BYTE playerNum) {
  alliance q;
  alliance prev;
  BYTE test;  /* Number we are testing */
  bool first; /* Is first item */

  first = true;
  if (allianceExist(value, playerNum)) {
    q = *value;
    prev = q;
    test = AllienceHead(q);
    while (test != playerNum) {
      first = false;
      prev = q;
      q = AllienceTail(q);
      test = AllienceHead(q);
    }
    if (!first) {
      prev->next = q->next;
    } else {
      (*value) = (*value)->next;
    }
    delete q;
  }
}

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
bool allianceExist(alliance *value, BYTE playerNum) {
  bool returnValue; /* Value to return */
  alliance q;
  BYTE test;

  q = *value;
  returnValue = false;
  while (!returnValue && NonEmpty(q)) {
    test = AllienceHead(q);
    if (test == playerNum) {
      returnValue = true;
    }
    q = AllienceTail(q);
  }
  return returnValue;
}

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
BYTE allianceNumAllies(alliance *value) {
  BYTE returnValue; /* Value to return */
  alliance q;

  q = *value;
  returnValue = 0;
  while (NonEmpty(q)) {
    returnValue++;
    q = AllienceTail(q);
  }

  return returnValue;
}

/*********************************************************
 *NAME:          allianceReturnNum
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/2/99
 *LAST MODIFIED: 18/2/99
 *PURPOSE:
 * Returns the player alliance is allied with at num
 *
 *ARGUMENTS:
 *  value - The alliance structure to remove from
 *  num   - Position in the structure to get
 *********************************************************/
BYTE allianceReturnNum(alliance *value, BYTE num) {
  BYTE returnValue; /* Value to return */
  BYTE count;
  alliance q;

  count = 0;
  returnValue = NEUTRAL;

  if (allianceNumAllies(value) >= num) {
    q = *value;
    while (NonEmpty(q) && count < num) {
      count++;
      q = AllienceTail(q);
    }
    if (NonEmpty(q)) {
      returnValue = q->playerNum;
    }
  }

  return returnValue;
}

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
BYTE allianceMakeLogAlliance(alliance *value, BYTE *buff) {
  BYTE returnValue = 1; /* Value to return */
  alliance q;

  q = *value;
  while (NonEmpty(q)) {
    buff[returnValue] = q->playerNum;
    returnValue++;
    q = AllienceTail(q);
  }

  buff[0] = returnValue - 1;
  return returnValue;
}
