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
 *Name:          bolo_map
 *Filename:      bolo_map.c
 *Author:        John Morrison
 *Creation Date: 21/10/98
 *Last Modified: 15/01/02
 *Purpose:
 *  Provides operations for read and writing of
 *  Bolo map files
 *********************************************************/

#include "bolo_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lzw/lzw.h"
#include "backend.h"
#include "bases.h"
#include "floodfill.h"
#include "global.h"
#include "log.h"
#include "messages.h"
#include "mines.h"
#include "network.h"
#include "pillbox.h"
#include "players.h"
#include "screen.h"
#include "screenbrainmap.h"
#include "sounddist.h"
#include "starts.h"

struct mapObj {
  BYTE mapItem[MAP_ARRAY_SIZE][MAP_ARRAY_SIZE]; /* The actual map */
  mapNet mn;
  mapNet mninc;
};

#undef MAP_MAX_SERVER_WAIT
#define MAP_MAX_SERVER_WAIT 200

/*********************************************************
 *NAME:          mapCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 21/10/98
 *PURPOSE:
 *  Creates and initilises the map structure. Sets all
 *  map squares to be deep sea
 *
 *ARGUMENTS:
 *  value - Pointer to the map file
 *********************************************************/
void mapCreate(map *value) {
  int count;  /* Looping variable */
  int count2; /* Looping variable */

  *value = new mapObj;
  for (count = 0; count < MAP_ARRAY_SIZE; count++) {
    for (count2 = 0; count2 < MAP_ARRAY_SIZE; count2++) {
      ((*value)->mapItem[count][count2]) = DEEP_SEA;
    }
  }
  (*value)->mn = nullptr;
  (*value)->mninc = nullptr;
}

/*********************************************************
 *NAME:          mapDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 23/2/99
 *PURPOSE:
 *  Destroys the map data structure. Also frees memory.
 *
 *ARGUMENTS:
 *  value - Pointer to the map file
 *********************************************************/
void mapDestroy(map *value) {
  mapNet q;

  if (*value != nullptr) {
    q = (*value)->mn;
    while (NonEmpty((*value)->mn)) {
      (*value)->mn = MapNetTail(q);
      delete q;
      q = (*value)->mn;
    }

    q = (*value)->mninc;
    while (NonEmpty((*value)->mninc)) {
      (*value)->mninc = MapNetTail(q);
      delete q;
      q = (*value)->mninc;
    }
    (*value)->mn = nullptr;
    (*value)->mninc = nullptr;
    delete *value;
  }
  *value = nullptr;
}

/*********************************************************
 *NAME:          mapReadPills
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 21/10/98
 *PURPOSE:
 *  Reads the pill information from a map file
 *  into the pill structure. Returns if the
 *  operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - Pointer to the file being read from
 *  value - Pointer to the pillbox structure
 *********************************************************/
bool mapReadPills(std::istream &input, pillboxes *value) {
  int total;        /* Total number of pills to be read in */
  int count;        /* Looping variable */
  bool returnValue; /* Value to return */
  pillbox readInto; /* The pillbox being read into */

  count = 1;
  total = pillsGetNumPills(value);
  returnValue = true;
  readInto.inTank = false;

  while (count <= total && returnValue && !input.eof()) {
    input.read((char *)&readInto, SIZEOFBMAP_PILL_INFO);
    if (input.gcount() != SIZEOFBMAP_PILL_INFO) {
      returnValue = false;
    } else {
      readInto.justSeen = false;
      pillsSetPill(value, &readInto, (BYTE)count);
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapReadBases
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 21/10/98
 *PURPOSE:
 *  Reads the base information from a map file
 *  into the base structure. Returns if the
 *  operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - Pointer to the file being read from
 *  value - Pointer to the pillbox structure
 *********************************************************/
bool mapReadBases(std::istream &input, bases *value) {
  int count;        /* Looping variable */
  int total;        /* Total number of bases to read */
  bool returnValue; /* Value to return */
  base readInto;    /* The base structure to read into */

  count = 1;
  total = basesGetNumBases(value);
  returnValue = true;

  while (count <= total && returnValue && !input.eof()) {
    input.read((char *)&readInto, SIZEOFBAMP_BASE_INFO);
    if (input.gcount() != SIZEOFBAMP_BASE_INFO) {
      returnValue = false;
    } else {
      readInto.baseTime = 0; /* Time between stock updates */
      basesSetBase(value, &readInto, (BYTE)count);
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapReadStarts
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 21/10/98
 *PURPOSE:
 *  Reads the player start information from a map file
 *  into the player starts structure. Returns if the
 *  operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - Pointer to the file being read from
 *  value - Pointer to the pillbox structure
 *********************************************************/
bool mapReadStarts(std::istream &input, starts *value) {
  int total;        /* Total number of entries to read */
  int count;        /* Looping variable */
  bool returnValue; /* Value to return */
  start readInto;   /* Item to read into */

  count = 1;
  total = startsGetNumStarts(value);
  returnValue = true;
  while (count <= total && returnValue && !input.eof()) {
    input.read((char *)&readInto, SIZEOFBMAP_START_INFO);
    if (input.gcount() != SIZEOFBMAP_START_INFO) {
      returnValue = false;
    } else {
      startsSetStart(value, &readInto, (BYTE)count);
    }
    count++;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapProcessRun
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 21/10/98
 *PURPOSE:
 *  Process a single map run and puts values into map
 *  data structure.
 *  Returns whether operation was successful or not
 *
 *ARGUMENTS:
 *  fp     - Pointer to the file being read from
 *  value  - Pointer to the map data structure
 *  elems  - Number of elements in the run
 *  yValue - The y Map co-ordinate
 *  startX - The start x co-ordinate
 *  endX   - The end x co-ordinate
 *********************************************************/
bool mapProcessRun(std::istream &input, map *value, BYTE elems, MAP_Y yValue,
                   BYTE startX, BYTE endX) {
  bool returnValue;  /* Value to return */
  bool needRead;     /* State variable - Do we need to read the next byte */
  mapRunState state; /* Current run state */
  BYTE item;         /* Item being worked on */
  BYTE highNibble;   /* The low nibble of a byte */
  BYTE lowNibble;    /* The low nibble of a byte */
  BYTE len;          /* Length of the row */
  BYTE mapPos;       /* Possision on map */
  int count;         /* Looping variables */
  int count2;

  count = 0;
  returnValue = true;
  state = highLen;
  mapPos = startX;
  len = 0;

  item = (BYTE)input.get();
  while (count < elems && input.good()) {
    needRead = false;
    highNibble = lowNibble = item;
    /* Extract Nibbles */
    highNibble >>= MAP_SHIFT_SIZE;
    lowNibble <<= MAP_SHIFT_SIZE;
    lowNibble >>= MAP_SHIFT_SIZE;

    while (!needRead) {
      switch (state) {
        case highLen:
          len = highNibble;
          if (len < MAP_RUN_DIFF) {
            state = lowDiff;
            len++;
          } else {
            state = lowSame;
          }
          break;

        case lowLen:
          len = lowNibble;
          if (len < MAP_RUN_DIFF) {
            state = highDiff;
            len++;
          } else {
            state = highSame;
          }
          needRead = true;
          break;

        case lowDiff:
          ((*value)->mapItem[mapPos][yValue]) = lowNibble;
          mapPos++;
          len--;
          if (len == 0) {
            state = highLen;
          } else {
            state = highDiff;
          }
          needRead = true;
          break;

        case highDiff:
          ((*value)->mapItem[mapPos][yValue]) = highNibble;
          mapPos++;
          len--;
          if (len == 0) {
            state = lowLen;
          } else {
            state = lowDiff;
          }
          break;

        case lowSame:
          count2 = 0;
          while (count2 < (len - MAP_RUN_SAME)) {
            ((*value)->mapItem[mapPos + count2][yValue]) = lowNibble;
            count2++;
          }
          mapPos = (BYTE)(mapPos + count2);
          state = highLen;
          needRead = true;
          break;

        case highSame:
          count2 = 0;
          while (count2 < (len - MAP_RUN_SAME)) {
            ((*value)->mapItem[mapPos + count2][yValue]) = highNibble;
            count2++;
          }
          mapPos = (BYTE)(mapPos + count2);
          state = lowLen;
          break;
      }
    }
    count++;
    item = (BYTE)input.get();
  }
  input.unget();
  /* Check all read correctly */
  if (count != elems || mapPos != endX) {
    returnValue = false;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapReadRuns
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED: 21/10/98
 *PURPOSE:
 *  Reads the map runs into the map data structure
 *  Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - Pointer to the file being read from
 *  value - Pointer to the map data structure
 *********************************************************/
bool mapReadRuns(std::istream &input, map *value) {
  bmapRunHeader runHead; /* The header of each run */
  size_t bytesRead;      /* The number of bytes read from the header */
  bool returnValue;      /* Value to return */
  bool done;             /* Is all the runs read */
  bool ret;              /* Function return Value */

  returnValue = true;
  done = false;

  input.read((char *)&runHead, SIZEOFBMAP_RUN_HEADER);
  bytesRead = input.gcount();

  while (!input.eof() && !done) {
    if (bytesRead != SIZEOFBMAP_RUN_HEADER) {
      /* Something bad happened reading */
      done = true;
      returnValue = false;
    } else if (runHead.datalen == 4 && runHead.y == MAP_ARRAY_LAST &&
               runHead.startx == MAP_ARRAY_LAST &&
               runHead.endx == MAP_ARRAY_LAST) {
      /* Finished reading */
      done = true;
      returnValue = true;
    } else {
      ret = mapProcessRun(input, value,
                          (BYTE)((runHead.datalen) - SIZEOFBMAP_RUN_HEADER),
                          runHead.y, runHead.startx, runHead.endx);
      if (!ret) {
        /* Function return failed */
        done = true;
        returnValue = false;
      }
    }
    input.read((char *)&runHead, SIZEOFBMAP_RUN_HEADER);
    bytesRead = input.gcount();
  }

  return returnValue;
}

/*********************************************************
 *NAME:          mapRead
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/10/98
 *LAST MODIFIED:  13/6/00
 *PURPOSE:
 *  Reads a map in.
 * Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fileNam - Pointer to a string containing the file name
 *  value   - Pointer to the map data structure
 *  ss      - Pointer to the starts structure
 *  bs      - Pointer to the bases structure
 *  pb      - Pointer to the pillbox structure
 *********************************************************/
bool mapRead(std::istream &input, map *value, pillboxes *pb, bases *bs,
             starts *ss) {
  if (!input.good()) {
    return false;
  }

  bool returnValue;       /* Value to return */
  char id[LENGTH_ID + 1]; /* The map ID Should read "BMAPBOLO" */
  BYTE mapVersion;        /* Version of the map file */
  BYTE current;           /* Item being read */

  returnValue = true;
  if (returnValue) {
    input.get(id, LENGTH_ID + 1);
    if (!input.good() || strcmp(id, MAP_HEADER) != 0) {
      returnValue = false;
    }
  }
  if (returnValue) {
    mapVersion = input.get();
    if (mapVersion != CURRENT_MAP_VERSION) {
      returnValue = false;
    }
  }
  if (returnValue) {
    current = input.get();
    pillsSetNumPills(pb, current);
    current = input.get();
    basesSetNumBases(bs, current);
    current = input.get();
    startsSetNumStarts(ss, current);
    if (pillsGetNumPills(pb) > MAX_PILLS || basesGetNumBases(bs) > MAX_BASES ||
        startsGetNumStarts(ss) > MAX_STARTS) {
      returnValue = false;
    }
  }
  if (returnValue) {
    returnValue = mapReadPills(input, pb);
  }
  if (returnValue) {
    returnValue = mapReadBases(input, bs);
  }
  if (returnValue) {
    returnValue = mapReadStarts(input, ss);
  }
  if (returnValue) {
    returnValue = mapReadRuns(input, value);
  }

  if (returnValue) {
    mapCenter(value, pb, bs, ss);
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapGetPos
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/10/98
 *LAST MODIFIED: 30/12/2008
 *PURPOSE:
 * Returns The value of a square in a map. Return
 * RIVER if value out of range
 *
 *ARGUMENTS:
 *  value  - Pointer to the map data structure
 *  xValue - The x co-ordinate
 *  yValue - The y co-ordinate
 *********************************************************/
BYTE mapGetPos(map *value, BYTE xValue, BYTE yValue) {
  BYTE returnValue = DEEP_SEA; /* Value to return */

  if (xValue > MAP_MINE_EDGE_LEFT && xValue < MAP_MINE_EDGE_RIGHT &&
      yValue > MAP_MINE_EDGE_TOP && yValue < MAP_MINE_EDGE_BOTTOM) {
    returnValue = (*value)->mapItem[xValue][yValue];
  }

  return returnValue;
}

/*********************************************************
 *NAME:          mapGetSpeed
 *AUTHOR:        John Morrison
 *CREATION DATE:  7/11/98
 *LAST MODIFIED: 15/01/02
 *PURPOSE:
 * Returns the speed of the tank for a given map square
 *
 *ARGUMENTS:
 *  value     - Pointer to the map data structure
 *  pb        - Pointer to the pillboxes structure
 *  bs        - Pointer to the bases structures
 *  xValue    - The x co-ordinate
 *  yValue    - The y co-ordinate
 *  onBoat    - Is the tank on a boat or not?
 *  playerNum - Player Number of this player
 *********************************************************/
BYTE mapGetSpeed(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue,
                 bool onBoat, BYTE playerNum) {
  BYTE returnValue; /* Value to return */
  BYTE terrain;     /* The current Terrain */
  bool done;        /* Are we done ? */

  returnValue = MAP_SPEED_TDEEPSEA;
  done = false;
  if (pillsExistPos(pb, xValue, yValue)) {
    /* Check for PB */
    if (!pillsDeadPos(pb, xValue, yValue)) {
      returnValue = MAP_SPEED_TPILLBOX;
      done = true;
    }
  } else if (basesExistPos(bs, xValue, yValue)) {
    /* Check for owned base */
    if (!basesCantDrive(bs, xValue, yValue, playerNum)) {
      returnValue = MAP_MANSPEED_TREFBASE;
    } else {
      returnValue = 0;
    }
    done = true;
  }
  if (!done) {
    terrain = (*value)->mapItem[xValue][yValue];
    if (terrain >= MINE_START && terrain <= MINE_END) {
      terrain = terrain - MINE_SUBTRACT;
    }
    switch (terrain) {
      case DEEP_SEA:
        returnValue = MAP_SPEED_TDEEPSEA;
        if (onBoat) {
          returnValue = MAP_SPEED_TBOAT;
        }
        break;
      case BUILDING:
        returnValue = MAP_SPEED_TBUILDING;
        break;
      case RIVER:
        returnValue = MAP_SPEED_TRIVER;
        if (onBoat) {
          returnValue = MAP_SPEED_TBOAT;
        }
        break;
      case SWAMP:
        returnValue = MAP_SPEED_TSWAMP;
        break;
      case CRATER:
        returnValue = MAP_SPEED_TCRATER;
        break;
      case ROAD:
        returnValue = MAP_SPEED_TROAD;
        break;
      case FOREST:
        returnValue = MAP_SPEED_TFOREST;
        break;
      case RUBBLE:
        returnValue = MAP_SPEED_TRUBBLE;
        break;
      case GRASS:
        returnValue = MAP_SPEED_TGRASS;
        break;
      case HALFBUILDING:
        returnValue = MAP_SPEED_THALFBUILDING;
        break;
      case BOAT:
        returnValue = MAP_SPEED_TBOAT;
        break;
      default:
        /* Fall through */
        break;
    }
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapGetManSpeed
 *AUTHOR:        John Morrison
 *CREATION DATE:  7/11/98
 *LAST MODIFIED: 15/01/02
 *PURPOSE:
 * Returns The speed of the tank for a given map square
 *
 *ARGUMENTS:
 *  value     - Pointer to the map data structure
 *  pb        - Pointer to the pillboxes structure
 *  bs        - Pointer to the bases structures
 *  xValue    - The x co-ordinate
 *  yValue    - The y co-ordinate
 *  playerNum - Player Number of this player
 *********************************************************/
BYTE mapGetManSpeed(map *value, pillboxes *pb, bases *bs, BYTE xValue,
                    BYTE yValue, BYTE playerNum) {
  BYTE returnValue; /* Value to return */
  BYTE terrain;     /* The current Terrain */
  bool done;        /* Are we done ? */

  returnValue = MAP_MANSPEED_TDEEPSEA;
  done = false;
  if (pillsExistPos(pb, xValue, yValue)) {
    /* Check for PB */
    if (!pillsDeadPos(pb, xValue, yValue)) {
      returnValue = MAP_MANSPEED_TPILLBOX;
      done = true;
    }
  } else if (basesExistPos(bs, xValue, yValue)) {
    /* Check for owned base */
    if (!basesCantDrive(bs, xValue, yValue, playerNum)) {
      returnValue = MAP_MANSPEED_TREFBASE;
    } else {
      returnValue = 0;
    }
    done = true;
  }
  if (!done) {
    terrain = (*value)->mapItem[xValue][yValue];
    if (terrain >= MINE_START && terrain <= MINE_END) {
      terrain = terrain - MINE_SUBTRACT;
    }
    switch (terrain) {
      case DEEP_SEA:
        returnValue = MAP_MANSPEED_TDEEPSEA;
        break;
      case BUILDING:
        returnValue = MAP_MANSPEED_TBUILDING;
        break;
      case RIVER:
        returnValue = MAP_MANSPEED_TRIVER;
        break;
      case SWAMP:
        returnValue = MAP_MANSPEED_TSWAMP;
        break;
      case CRATER:
        returnValue = MAP_MANSPEED_TCRATER;
        break;
      case ROAD:
        returnValue = MAP_MANSPEED_TROAD;
        break;
      case FOREST:
        returnValue = MAP_MANSPEED_TFOREST;
        break;
      case RUBBLE:
        returnValue = MAP_MANSPEED_TRUBBLE;
        break;
      case GRASS:
        returnValue = MAP_MANSPEED_TGRASS;
        break;
      case HALFBUILDING:
        returnValue = MAP_MANSPEED_THALFBUILDING;
        break;
      case BOAT:
        returnValue = MAP_MANSPEED_TBOAT;
        break;
    }
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapGetTurnRate
 *AUTHOR:        John Morrison
 *CREATION DATE:  7/11/98
 *LAST MODIFIED: 15/1/02
 *PURPOSE:
 * Returns The turn rate of the tank for a given
 * map square
 *
 *ARGUMENTS:
 *  value  - Pointer to the map data structure
 *  pb     - Pointer to the pillboxes structure
 *  bs     - Pointer to the bases structures
 *  xValue - The x co-ordinate
 *  yValue - The y co-ordinate
 *  onBoat - Is the tank on a boat or not?
 *  playerNum - Player Number who is requesting the turn
 *********************************************************/
TURNTYPE mapGetTurnRate(map *value, pillboxes *pb, bases *bs, BYTE xValue,
                        BYTE yValue, bool onBoat, BYTE playerNum) {
  TURNTYPE returnValue; /* Value to return */
  BYTE terrain;         /* The current Terrain */
  bool done;            /* Are we done ? */

  returnValue = MAP_TURN_TDEEPSEA;
  done = false;
  if (pillsExistPos(pb, xValue, yValue)) {
    /* Check for PB */
    if (!pillsDeadPos(pb, xValue, yValue)) {
      returnValue = MAP_TURN_TPILLBOX;
      done = true;
    }
  } else if (basesExistPos(bs, xValue, yValue)) {
    /* Check for owned base */
    if (!basesCantDrive(bs, xValue, yValue, playerNum)) {
      returnValue = MAP_TURN_TREFBASE;
    } else {
      returnValue = 0;
    }
    done = true;
  }
  if (!done) {
    terrain = (*value)->mapItem[xValue][yValue];
    if (terrain >= MINE_START && terrain <= MINE_END) {
      terrain = terrain - MINE_SUBTRACT;
    }
    switch (terrain) {
      case DEEP_SEA:
        returnValue = MAP_TURN_TDEEPSEA;
        if (onBoat) {
          returnValue = MAP_TURN_TBOAT;
        }
        break;
      case BUILDING:
        returnValue = MAP_TURN_TBUILDING;
        break;
      case RIVER:
        returnValue = MAP_TURN_TRIVER;
        if (onBoat) {
          returnValue = MAP_TURN_TBOAT;
        }
        break;
      case SWAMP:
        returnValue = MAP_TURN_TSWAMP;
        break;
      case CRATER:
        returnValue = MAP_TURN_TCRATER;
        break;
      case ROAD:
        returnValue = MAP_TURN_TROAD;
        break;
      case FOREST:
        returnValue = MAP_TURN_TFOREST;
        break;
      case RUBBLE:
        returnValue = MAP_TURN_TRUBBLE;
        break;
      case GRASS:
        returnValue = MAP_TURN_TGRASS;
        break;
      case HALFBUILDING:
        returnValue = MAP_TURN_THALFBUILDING;
        break;
      case BOAT:
        returnValue = MAP_TURN_TBOAT;
        break;
    }
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapIsPassable
 *AUTHOR:        John Morrison
 *CREATION DATE:  29/12/98
 *LAST MODIFIED:  29/12/98
 *PURPOSE:
 * Returns whether the map square is passable or not
 *
 *ARGUMENTS:
 *  value  - Pointer to the map data structure
 *  xValue - The x co-ordinate
 *  yValue - The y co-ordinate
 *  onBoat - Is the item on a boat
 *********************************************************/
bool mapIsPassable(map *value, BYTE xValue, BYTE yValue, bool onBoat) {
  bool returnValue; /* Value to return */
  BYTE pos;         /* The current position */

  returnValue = false;

  pos = (*value)->mapItem[xValue][yValue];
  if (pos >= MINE_START && pos <= MINE_END) {
    pos -= MINE_SUBTRACT;
  }
  switch (pos) {
    case DEEP_SEA:
      returnValue = true;
      break;
    case BUILDING:
      returnValue = false;
      break;
    case RIVER:
      returnValue = true;
      break;
    case SWAMP:
      if (!onBoat) {
        returnValue = true;
      }
      break;
    case CRATER:
      returnValue = true;
      break;
    case ROAD:
      if (!onBoat) {
        returnValue = true;
      }
      break;
    case FOREST:
      returnValue = false;
      break;
    case RUBBLE:
      if (!onBoat) {
        returnValue = true;
      }
      break;
    case GRASS:
      if (!onBoat) {
        returnValue = true;
      }
      break;
    case HALFBUILDING:
      returnValue = false;
      break;
    case BOAT:
      returnValue = false;
      break;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapSetPos
 *AUTHOR:        John Morrison
 *CREATION DATE: 30/12/98
 *LAST MODIFIED: 05/05/01
 *PURPOSE:
 * Sets a position on the map
 *
 *ARGUMENTS:
 *  value    - Pointer to the map data structure
 *  xValue   - The x co-ordinate
 *  yValue   - The y co-ordinate
 *  terrain  - The new terrain type
 *  needSend - Used in network. True if we should
 *             request the server make this change
 *             False indicates the server should make it
 * mineClear - Set to true if we should just set the map
 *             to the terrain. This is to remove the mines
 *             from under bases on start up
 *********************************************************/
void mapSetPos(map *value, BYTE xValue, BYTE yValue, BYTE terrain,
               bool needSend, bool mineClear) {
  if (netGetType() == netSingle || mineClear) {
    /* Single player game */
    (*value)->mapItem[xValue][yValue] = terrain;
    screenBrainMapSetPos(xValue, yValue, terrain,
                         minesExistPos(screenGetMines(), xValue, yValue));
  } else {
    /* Multiplayer game */
    mapNetAdd(value, xValue, yValue, terrain, needSend);
  }
  logAddEvent(log_MapChange, xValue, yValue, terrain, 0, 0, nullptr);
}

/*********************************************************
 *NAME:          mapIsLand
 *AUTHOR:        John Morrison
 *CREATION DATE: 6/1/99
 *LAST MODIFIED: 6/1/99
 *PURPOSE:
 * Returns whether a map square is land or not
 *
 *ARGUMENTS:
 *  value   - Pointer to the map data structure
 *  pb      - Pointer to the pillboxs structure
 *  bs      - Pointer to the bases structure
 *  xValue  - The x co-ordinate
 *  yValue  - The y co-ordinate
 *********************************************************/
bool mapIsLand(map *value, pillboxes *pb, bases *bs, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */

  returnValue = true;

  if ((((*value)->mapItem[xValue][yValue]) == RIVER) ||
      (*value)->mapItem[xValue][yValue] == DEEP_SEA) {
    if ((!basesExistPos(
            bs, xValue,
            yValue))) { /* (pillsExistPos(pb, xValue, yValue) == false) &&  */
      returnValue = false;
    }
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapIsMine
 *AUTHOR:        John Morrison
 *CREATION DATE: 22/1/99
 *LAST MODIFIED: 13/3/99
 *PURPOSE:
 * Returns whether a map square is mines or not
 *
 *ARGUMENTS:
 *  value   - Pointer to the map data structure
 *  xValue  - The x co-ordinate
 *  yValue  - The y co-ordinate
 *********************************************************/
bool mapIsMine(map *value, BYTE xValue, BYTE yValue) {
  bool returnValue; /* Value to return */

  returnValue = false;

  if (xValue <= MAP_MINE_EDGE_LEFT || xValue >= MAP_MINE_EDGE_RIGHT ||
      yValue <= MAP_MINE_EDGE_TOP || yValue >= MAP_MINE_EDGE_BOTTOM) {
    returnValue = true;
  } else if ((*value)->mapItem[xValue][yValue] >= MINE_START &&
             (*value)->mapItem[xValue][yValue] <= MINE_END) {
    returnValue = true;
  }
  /* Check network game if required */
  if (!returnValue) {
    if (netGetType() != netSingle) {
      /* Is a network game - Check its not waiting to be placed */
      //        returnValue = mapNetIsMine(xValue, yValue);
    }
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapWrite
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/2/99
 *LAST MODIFIED: 9/2/99
 *PURPOSE:
 * Writes a map to the filename given
 * Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fileName - Pointer to a string containing the file name
 *  value    - Pointer to the map data structure
 *  ss       - Pointer to the starts structure
 *  bs       - Pointer to the bases structure
 *  pb       - Pointer to the pillbox structure
 *********************************************************/
bool mapWrite(char *fileName, map *value, pillboxes *pb, bases *bs,
              starts *ss) {
  FILE *fp;         /* File pointer */
  bool returnValue; /* Value to return */
  int ret;          /* Function return value */
  BYTE numPills;    /* Number of pills on the map */
  BYTE numBases;    /* Number of bases on the map */
  BYTE numStarts;   /* Number of starts on the map */

  returnValue = true;
  numPills = pillsGetNumPills(pb);
  numBases = basesGetNumBases(bs);
  numStarts = startsGetNumStarts(ss);

  fp = fopen(fileName, "wb");
  if (fp == nullptr) {
    returnValue = false;
  }
  /* Write header */
  if (returnValue && fp) {
    ret = fputs(MAP_HEADER, fp);
    if (ret == EOF) {
      returnValue = false;
    }
  }

  /* Write map version */
  if (returnValue && fp) {
    ret = fputc(CURRENT_MAP_VERSION, fp);
    if (ret == EOF) {
      returnValue = false;
    }
  }

  /* Put number of pills */
  if (returnValue && fp) {
    ret = fputc(numPills, fp);
    if (ret == EOF) {
      returnValue = false;
    }
  }

  /* Put number of bases */
  if (returnValue && fp) {
    ret = fputc(numBases, fp);
    if (ret == EOF) {
      returnValue = false;
    }
  }

  /* Put number of starts */
  if (returnValue && fp) {
    ret = fputc(numStarts, fp);
    if (ret == EOF) {
      returnValue = false;
    }
  }

  /* Write pill locations */
  if (returnValue && fp) {
    returnValue = mapWritePills(fp, pb, numPills);
  }
  /* Write bases locations */
  if (returnValue && fp) {
    returnValue = mapWriteBases(fp, bs, numBases);
  }
  /* Write starts locations */
  if (returnValue && fp) {
    returnValue = mapWriteStarts(fp, ss, numStarts);
  }

  if (returnValue && fp) {
    returnValue = mapWriteRuns(fp, value);
  }
  if (fp) {
    fclose(fp);
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapWritePills
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/2/99
 *LAST MODIFIED: 9/2/99
 *PURPOSE:
 * Writes the pillbox locations out
 * Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - File pointer
 *  pb    - Pointer to the pillbox structure
 *  total - Total number of pills to write
 *********************************************************/
bool mapWritePills(FILE *fp, pillboxes *pb, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  pillbox item;     /* Pillbox information */
  int ret;          /* Function return Value */

  returnValue = true;
  count = 1;
  while (count <= total && returnValue) {
    pillsGetPill(pb, &item, count);
    /* Write each pill out */
    ret = fputc(item.x, fp);
    if (ret != item.x) {
      returnValue = false;
    }
    if (returnValue) {
      ret = fputc(item.y, fp);
      if (ret != item.y) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.owner, fp);
      if (ret != item.owner) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.armour, fp);
      if (ret != item.armour) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.speed, fp);
      if (ret != item.speed) {
        returnValue = false;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
 *NAME:          mapWriteBases
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/2/99
 *LAST MODIFIED: 9/2/99
 *PURPOSE:
 * Writes the bases locations out
 * Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - File pointer
 *  bs    - Pointer to the pillbox structure
 *  total - Total number of bases to write
 *********************************************************/
bool mapWriteBases(FILE *fp, bases *bs, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  base item;        /* The base item being saved */
  int ret;          /* Function return */

  returnValue = true;
  count = 1;
  while (count <= total && returnValue) {
    basesGetBase(bs, &item, count);
    /* Write each base out */
    ret = fputc(item.x, fp);
    if (ret != item.x) {
      returnValue = false;
    }
    if (returnValue) {
      ret = fputc(item.y, fp);
      if (ret != item.y) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.owner, fp);
      if (ret != item.owner) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.armour, fp);
      if (ret != item.armour) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.shells, fp);
      if (ret != item.shells) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.mines, fp);
      if (ret != item.mines) {
        returnValue = false;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
 *NAME:          mapWriteStarts
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/2/99
 *LAST MODIFIED: 9/2/99
 *PURPOSE:
 * Writes the starts locations out
 * Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - File pointer
 *  ss    - Pointer to the starts structure
 *  total - Total number of starts to write
 *********************************************************/
bool mapWriteStarts(FILE *fp, starts *ss, BYTE total) {
  bool returnValue; /* Value to return */
  BYTE count;       /* Looping variable */
  start item;       /* Item being saved */
  int ret;          /* Function return */

  returnValue = true;
  count = 1;

  while (count <= total && returnValue) {
    startsGetStartStruct(ss, &item, count);
    /* Write each start out */
    ret = fputc(item.x, fp);
    if (ret != item.x) {
      returnValue = false;
    }
    if (returnValue) {
      ret = fputc(item.y, fp);
      if (ret != item.y) {
        returnValue = false;
      }
    }
    if (returnValue) {
      ret = fputc(item.dir, fp);
      if (ret != item.dir) {
        returnValue = false;
      }
    }
    count++;
  }

  return returnValue;
}

/*********************************************************
 *NAME:          mapWriteRuns
 *AUTHOR:        John Morrison
 *CREATION DATE: 9/2/99
 *LAST MODIFIED: 9/2/99
 *PURPOSE:
 * Writes out the series of map runs
 * Returns if the operation was successful or not
 *
 *ARGUMENTS:
 *  fp    - File pointer
 *  value - Pointer to the map structure
 *********************************************************/
bool mapWriteRuns(FILE *fp, map *value) {
  bool returnValue; /* Value to return */
  bmapRun run;
  BYTE xPos; /* Current position on the map */
  BYTE yPos;
  long len;   /* Length of the run to write */
  size_t ret; /* Function return */

  returnValue = true;
  xPos = 0;
  yPos = 0;
  while (yPos < 0xFF && returnValue) {
    /* Process runs */
    len = mapPrepareRun(value, &run, &xPos, &yPos);
    /* Write the run out */
    ret = fwrite(&run, (size_t)len, 1, fp);
    if (ret != 1) {
      returnValue = false;
    }
  }

  return returnValue;
}

#define put_nibble(X)                                           \
  (!nibble_flag ? (nibble_flag = true, *nibble_data = (X) << 4) \
                : (nibble_flag = false, *nibble_data++ |= (X) & 0xF))

/*********************************************************
 *NAME:          mapPrepareRun
 *AUTHOR:        John Morrison
 *CREATION DATE:  9/2/99
 *LAST MODIFIED: 10/2/99
 *PURPOSE:
 * Prepares a map run to write out. Returns the length of
 * the run
 *
 *ARGUMENTS:
 *  value - Pointer to the map structure
 *  run   - Pointer to the map run structure
 *  xPos  - Pointer to our current X position
 *  yPos  - Pointer to our current Y position
 *********************************************************/
long mapPrepareRun(map *value, bmapRun *run, BYTE *xPos, BYTE *yPos) {
  BYTE terrain; /* Terrain under current Position */
  BYTE code;    /* Map code (ie identical/differnt etc) */
  BYTE x;       /* Temp variables to hold xPos and yPos */
  BYTE y;
  bool nibble_flag;  /* High/Low nibble filter */
  BYTE *nibble_data; /* Holds run->data for nibble calculations */

  nibble_flag = false;
  x = *xPos;
  y = *yPos;
  nibble_data = run->data;

  /* Search for non-DEEPSEA terrain */
  while (mapGetPos(value, x, y) == DEEP_SEA) {
    if (x < 0xFF) {
      x++;
    } else if (y < MAP_ARRAY_LAST) {
      x = 0;
      y++;
    } else {
      break;
    }
  }
  run->startx = x;
  if (y < MAP_ARRAY_LAST) {
    terrain = mapGetPos(value, x, y);
    while (terrain != DEEP_SEA) {
      if (terrain == mapGetPos(value, (BYTE)(x + 1), y)) {
        /* Two squares are the same */
        code = MAP_CODE_IDENTICAL_START;
        x += MAP_CODE_IDENTICAL_SKIP; /* skip over the two squares we have found
                                       */
        while (code < MAP_CODE_IDENTICAL_END &&
               mapGetPos(value, x, y) == terrain) {
          code++;
          x++;
        }
        put_nibble(code);
        put_nibble(terrain);
      } else {
        BYTE code = 0;  /* code 0 means 1 individual square */
        MAP_X ds = x++; /* record where the difference run starts */
        while (code < MAP_CODE_DIFFERENT_END &&
               mapGetPos(value, x, y) != DEEP_SEA &&
               mapGetPos(value, x, y) != mapGetPos(value, (BYTE)(x + 1), y)) {
          code++;
          x++;
        }
        put_nibble(code);
        while (ds < x) {
          put_nibble(mapGetPos(value, ds++, y));
        }
      }
      terrain = mapGetPos(value, x, y);
    }
    if (nibble_flag) {
      put_nibble(0); /* round it up to whole number of bytes */
    }
  }

  *xPos = run->endx = x;
  *yPos = run->y = y;
  return (run->datalen = (BYTE)(nibble_data - (BYTE *)run));
}

/*********************************************************
 *NAME:          mapNetAdd
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/2/99
 *LAST MODIFIED: 27/11/99
 *PURPOSE:
 * Adds a item to the mapNet structure. If an item already
 * exists at that position it repaces it with the new
 * terrain.
 *
 *ARGUMENTS:
 *  value    - Pointer to the map structure
 *  mx       - Current X position
 *  my       - Current Y position
 *  terrain  - Terrain to place
 *  needSend - Should we send this update?
 *********************************************************/
void mapNetAdd(map *value, BYTE mx, BYTE my, BYTE terrain, bool needSend) {
  mapNet q;
  bool done;

  done = false;
  /* Check to see it is in the incoming buffer */
  if (!threadsGetContext()) {
    q = (*value)->mninc;
    while (NonEmpty(q) && !done) {
      if (q->mx == mx && q->my == my && q->terrain == terrain) {
        /* Exists */
        (*value)->mapItem[mx][my] = terrain;
        screenBrainMapSetPos(mx, my, terrain,
                             minesExistPos(screenGetMines(), mx, my));
        if (q->prev != nullptr) {
          q->prev->next = q->next;
        } else {
          (*value)->mninc = MapNetTail((*value)->mninc);
          if (NonEmpty((*value)->mninc)) {
            (*value)->mninc->prev = nullptr;
          }
        }
        if (q->next != nullptr) {
          q->next->prev = q->prev;
        }
        delete q;
        done = true;
      } else {
        q = MapNetTail(q);
      }
    }
  } /*else if (terrain == RIVER || terrain == CRATER) {
      floodAddItem(mx, my);
  } */

  if (!done && (*value)->mapItem[mx][my] == RIVER &&
      (terrain == CRATER || terrain == MINE_CRATER)) {
    return;
  }

  if ((*value)->mapItem[mx][my] != terrain) {
    q = (*value)->mn;
    while (NonEmpty(q) && !done) {
      if (q->mx == mx && q->my == my) {
        q->terrain = terrain;
        q->length = 0;
        q->needSend = needSend;
        done = true;
      }
      q = MapNetTail(q);
    }
    /* If not found then add it */
    if (!done) {
      q = new mapNetObj;
      q->mx = mx;
      q->my = my;
      q->terrain = terrain;
      q->oldTerrain = (*value)->mapItem[mx][my];
      q->length = 0;
      q->needSend = needSend;
      q->next = (*value)->mn;
      q->prev = nullptr;
      if (NonEmpty((*value)->mn)) {
        (*value)->mn->prev = q;
      }
      (*value)->mn = q;
    }
    (*value)->mapItem[mx][my] = terrain;
    screenBrainMapSetPos(mx, my, terrain,
                         minesExistPos(screenGetMines(), mx, my));
  }
}

/*********************************************************
 *NAME:          mapNetUpdate
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/2/99
 *LAST MODIFIED: 30/10/99
 *PURPOSE:
 * Updates the time the items have been waiting for the
 * server to authenticate them. If it reaches the expiry
 * date then it is restored.
 *
 *ARGUMENTS:
 *  value   - Pointer to map structure
 *  pb      - Pointer to the pillboxes strucuture
 *  bs      - Pointer to the bases strucuture
 *********************************************************/
void mapNetUpdate(map *value, pillboxes *pb, bases *bs) {
  mapNet q;        /* Used in looping */
  bool needRedraw; /* Do we need a redraw */
  mapNet del;

  needRedraw = false;

  q = (*value)->mn;
  while (NonEmpty(q)) {
    q->length = q->length + 1;
    if (q->length > MAP_MAX_SERVER_WAIT && !q->needSend) {
      messageAdd(networkMessage, (char *)"\0", (char *)"at");
      if (q->oldTerrain >= MINE_START && q->oldTerrain <= MINE_END) {
        q->oldTerrain -= MINE_SUBTRACT;
      }
      (*value)->mapItem[q->mx][q->my] = q->oldTerrain;
      mapNetCheckWater(value, pb, bs, q->mx, q->my);
      screenBrainMapSetPos(q->mx, q->my, (*value)->mapItem[q->mx][q->my],
                           minesExistPos(screenGetMines(), q->mx, q->my));
      //        if (q->oldTerrain == CRATER) {
      //          floodAddItem(q->mx, q->my);
      //        }
      needRedraw = true;
      if (q->prev != nullptr) {
        q->prev->next = q->next;
      } else {
        (*value)->mn = MapNetTail((*value)->mn);
        if (NonEmpty((*value)->mn)) {
          (*value)->mn->prev = nullptr;
        }
      }
      if (q->next != nullptr) {
        q->next->prev = q->prev;
      }
      del = q;
      q = MapNetTail(q);
      delete del;
    } else {
      q = MapNetTail(q);
    }
  }

  q = (*value)->mninc;
  while (NonEmpty(q)) {
    q->length++;
    if (q->length > MAP_MAX_SERVER_WAIT) { /* Was 25 */
      messageAdd(networkMessage, (char *)"\0", (char *)"pt");
      (*value)->mapItem[q->mx][q->my] = q->terrain;
      mapNetCheckWater(value, pb, bs, q->mx, q->my);
      screenBrainMapSetPos(q->mx, q->my, (*value)->mapItem[q->mx][q->my],
                           minesExistPos(screenGetMines(), q->mx, q->my));
      needRedraw = true;
      if (q->prev != nullptr) {
        q->prev->next = q->next;
      } else {
        (*value)->mninc = MapNetTail((*value)->mninc);
        if (NonEmpty((*value)->mninc)) {
          (*value)->mninc->prev = nullptr;
        }
      }
      if (q->next != nullptr) {
        q->next->prev = q->prev;
      }
      del = q;
      q = MapNetTail(q);
      delete del;
    } else {
      q = MapNetTail(q);
    }
  }

  if (needRedraw) {
    screenReCalc();
  }
}

/*********************************************************
 *NAME:          mapNetIncomingItem
 *AUTHOR:        John Morrison
 *CREATION DATE: 3/11/99
 *LAST MODIFIED: 9/11/99
 *PURPOSE:
 * A incoming map item has come from the server. If it is
 * in the waitinf for confirmation buffer remove it, else
 * if it exists in the incoming buffer replace it with the
 * new value otherwise add it to the incoming buffer
 *
 *ARGUMENTS:
 *  value   - Pointer to the map structure
 *  mx      - X position to add
 *  my      - Y position
 *  terrain - Terrain to place
 *********************************************************/
void mapNetIncomingItem(map *value, BYTE mx, BYTE my, BYTE terrain) {
  mapNet q, del;
  bool done; /* Finished looping */

  /* Check for exists in waiting for confirmation buffer */
  q = (*value)->mn;
  done = false;

  while (NonEmpty(q) && !done) {
    if (q->mx == mx && q->my == my) { /* && q->terrain == terrain */
      /* Its in our structure */
      if (q->prev != nullptr) {
        q->prev->next = q->next;
      } else {
        (*value)->mn = MapNetTail((*value)->mn);
        if (NonEmpty((*value)->mn)) {
          (*value)->mn->prev = nullptr;
        }
      }
      if (q->next != nullptr) {
        q->next->prev = q->prev;
      }
      done = true;
      if ((*value)->mapItem[mx][my] == RIVER ||
          (*value)->mapItem[mx][my] == BOAT) {
        minesRemoveItem(screenGetMines(), mx, my);
        screenBrainMapSetPos(q->mx, q->my, (*value)->mapItem[mx][my], false);
      }
      del = q;
      q = MapNetTail(q);
      delete del;
    } else {
      q = MapNetTail(q);
    }
  }

  /* Check for exists in the incoming buffer packet */
  q = (*value)->mninc;

  while (NonEmpty(q) && !done) {
    if (q->mx == mx && q->my == my && q->terrain == terrain) {
      done = true;
    } else {
      q = MapNetTail(q);
    }
  }

  /* If it isn't added */
  if (!done) {
    //      if (terrain != BUILDING && terrain != BOAT && terrain != ROAD &&
    //      terrain < MINE_START) {
    q = new mapNetObj;
    q->mx = mx;
    q->my = my;
    q->terrain = terrain;
    q->length = 0;
    q->next = (*value)->mninc;
    q->prev = nullptr;
    if (NonEmpty((*value)->mninc)) {
      (*value)->mninc->prev = q;
    }
    (*value)->mninc = q;
    //    } else {
    //    (*value)->mapItem[mx][my] = terrain;
    //}
  }
}

/*********************************************************
 *NAME:          mapNetPacket
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/2/99
 *LAST MODIFIED: 31/10/99
 *PURPOSE:
 * A packet has arrived. Here is a peice of map info in it.
 *
 *ARGUMENTS:
 *  value   - Pointer to map structure
 *  mx      - X position to add
 *  my      - Y position
 *  terrain - Terrain to place
 *********************************************************/
void mapNetPacket(map *value, BYTE mx, BYTE my, BYTE terrain) {
  mapNet q, del;
  bool done; /* Finished looping */

  q = (*value)->mn;
  done = false;

  while (NonEmpty(q) && !done) {
    if (q->mx == mx && q->my == my && q->terrain == terrain) {
      (*value)->mapItem[mx][my] = terrain;
      screenBrainMapSetPos(mx, my, (*value)->mapItem[mx][my],
                           minesExistPos(screenGetMines(), mx, my));
      if (q->prev != nullptr) {
        q->prev->next = q->next;
      } else {
        (*value)->mn = MapNetTail((*value)->mn);
        if (NonEmpty((*value)->mn)) {
          (*value)->mn->prev = nullptr;
        }
      }
      if (q->next != nullptr) {
        q->next->prev = q->prev;
      }
      del = q;
      q = MapNetTail(q);
      delete del;
      done = true;
    } else {
      q = MapNetTail(q);
    }
  }

  if (!done) {
    (*value)->mapItem[mx][my] = terrain;
    screenBrainMapSetPos(mx, my, (*value)->mapItem[mx][my],
                         minesExistPos(screenGetMines(), mx, my));
    if (terrain == BUILDING || terrain == ROAD) {
      /* Play the building sound */
      soundDist(manBuildingNear, mx, my);
    } else if (terrain == CRATER) {
      floodAddItem(screenGetFloodFill(), mx, my);
    }
  }
}

/*********************************************************
 *NAME:          mapNetMakePacket
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/2/99
 *LAST MODIFIED: 31/10/99
 *PURPOSE:
 * Make the map part of the packet. Returns the data length.
 * Is destructive on the data
 *
 *ARGUMENTS:
 *  map  - Pointer to the map structure
 *  buff - Buffer to hold data
 *********************************************************/
BYTE mapNetMakePacket(map *value, BYTE *buff) {
  BYTE returnValue; /* Value to return */
  mapNet q;

  returnValue = 0;
  while (NonEmpty((*value)->mn)) {
    buff[returnValue] = (*value)->mn->mx;
    returnValue++;
    buff[returnValue] = (*value)->mn->my;
    returnValue++;
    buff[returnValue] = (*value)->mn->terrain;
    returnValue++;
    q = (*value)->mn;
    (*value)->mn = MapNetTail((*value)->mn);
    delete q;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapNetClientPacket
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/2/99
 *LAST MODIFIED: 31/10/99
 *PURPOSE:
 * Make the client map part of the packet. Returns the
 * data length.
 *
 *ARGUMENTS:
 *  map  - Pointer to the map structure
 *  buff - Buffer to hold data
 *********************************************************/
BYTE mapNetClientPacket(map *value, BYTE *buff) {
  BYTE returnValue; /* Value to return */
  mapNet q;

  returnValue = 0;
  q = (*value)->mn;
  while (NonEmpty(q)) {
    if (q->needSend) {
      q->needSend = false;
      q->length = 0;
      buff[returnValue] = (*value)->mn->mx;
      returnValue++;
      buff[returnValue] = (*value)->mn->my;
      returnValue++;
      buff[returnValue] = (*value)->mn->terrain;
      returnValue++;
    }
    q = MapNetTail(q);
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapMakeNetRun
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/2/99
 *LAST MODIFIED: 24/4/99
 *PURPOSE:
 * Makes a map network run at yPos. A network run is an
 * compress array of the bytes from 20 to 236 for the next
 * 5 vertical map rows. Returns the size of the compressed
 * data.
 *
 *ARGUMENTS:
 *  value - Pointer to the map structure
 *  buff  - Buffer to hold data
 *  yPos  - Y position of the run
 *********************************************************/
int mapMakeNetRun(map *value, BYTE *buff, BYTE yPos) {
  BYTE count; /* Looping variable */
  BYTE array[6 * (MAP_MINE_EDGE_RIGHT - (MAP_MINE_EDGE_LEFT + 1))];
  BYTE *arrayPtr;
  BYTE xPos;

  memset(array, 0, sizeof(array));
  count = 0;
  arrayPtr = array;
  /* Prepare it */
  while (count < 6) {
    for (xPos = MAP_MINE_EDGE_LEFT + 1; xPos < MAP_MINE_EDGE_RIGHT; xPos++) {
      *arrayPtr = (*value)->mapItem[xPos][yPos + count];
      arrayPtr++;
    }
    count++;
  }
  /* Compress it */
  return lzwencoding((char *)array, (char *)buff,
                     6 * (MAP_MINE_EDGE_RIGHT - (MAP_MINE_EDGE_LEFT + 1)));
}

/*********************************************************
 *NAME:          mapSetNetRun
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/2/99
 *LAST MODIFIED:  7/1/00
 *PURPOSE:
 * Sets the map to the network run at yPos. A network run
 * is an array of the bytes from 20 to 236
 *
 *ARGUMENTS:
 *  value   - Pointer to the map structure
 *  buff    - Buffer that contains data
 *  yPos    - Y position of the run
 *  dataLen - Length of the data
 *********************************************************/
void mapSetNetRun(map *value, BYTE *buff, BYTE yPos, int dataLen) {
  BYTE count; /* Looping variable */
  BYTE array[6 * (MAP_MINE_EDGE_RIGHT - (MAP_MINE_EDGE_LEFT + 1))];
  BYTE *arrayPtr;
  BYTE xPos;

  count = 0;
  arrayPtr = array;
  /* Compress it */
  lzwdecoding((char *)buff, (char *)array, dataLen);
  /* Store it */
  while (count < 6) {
    for (xPos = MAP_MINE_EDGE_LEFT + 1; xPos < MAP_MINE_EDGE_RIGHT; xPos++) {
      (*value)->mapItem[xPos][yPos + count] = *arrayPtr;
      arrayPtr++;
    }
    count++;
  }
}

/*********************************************************
 *NAME:          mapNetCheckWater
 *AUTHOR:        John Morrison
 *CREATION DATE: 19/11/99
 *LAST MODIFIED: 19/11/99
 *PURPOSE:
 * Checks an square updated through mapNetUpdate to see if
 * it should be filled to overcome the mines problem.
 *
 *ARGUMENTS:
 *  value  - Pointer to map structure
 *  pb     - Pointer to the pillboxes strucuture
 *  bs     - Pointer to the bases strucuture
 *  xValue - X Value to check
 *  yValue - Y Value to check
 *********************************************************/
void mapNetCheckWater(map *value, pillboxes *pb, bases *bs, BYTE xValue,
                      BYTE yValue) {
  BYTE above; /* Squares around */
  BYTE below;
  BYTE leftPos;
  BYTE rightPos;

  if ((*value)->mapItem[xValue][yValue] == CRATER ||
      (*value)->mapItem[xValue][yValue] == MINE_CRATER) {
    above = (*value)->mapItem[xValue][(BYTE)(yValue - 1)];
    below = (*value)->mapItem[xValue][(BYTE)(yValue + 1)];
    leftPos = (*value)->mapItem[(BYTE)(xValue - 1)][yValue];
    rightPos = (*value)->mapItem[(BYTE)(xValue + 1)][yValue];

    /* Check for pills, bases etc. If found change to non crater / water */
    if (pillsExistPos(pb, xValue, (BYTE)(yValue - 1))) {
      above = ROAD;
    } else if (basesExistPos(bs, xValue, (BYTE)(yValue - 1))) {
      above = ROAD;
    }

    if (pillsExistPos(pb, xValue, (BYTE)(yValue + 1))) {
      below = ROAD;
    } else if (basesExistPos(bs, xValue, (BYTE)(yValue + 1))) {
      below = ROAD;
    }

    if (pillsExistPos(pb, (BYTE)(xValue - 1), yValue)) {
      leftPos = ROAD;
    } else if (basesExistPos(bs, (BYTE)(xValue - 1), yValue)) {
      leftPos = ROAD;
    }

    if (pillsExistPos(pb, (BYTE)(xValue + 1), yValue)) {
      rightPos = ROAD;
    } else if (basesExistPos(bs, (BYTE)(xValue - 1), yValue)) {
      rightPos = ROAD;
    }

    if (leftPos == DEEP_SEA || leftPos == BOAT || leftPos == RIVER ||
        rightPos == DEEP_SEA || rightPos == BOAT || rightPos == RIVER ||
        above == DEEP_SEA || above == RIVER || above == BOAT ||
        below == DEEP_SEA || below == BOAT || below == RIVER) {
      /* Do fill */
      (*value)->mapItem[xValue][yValue] = RIVER;
      minesRemoveItem(screenGetMines(), xValue, yValue);
    }
  }
}

int lzwdecoding(char *src, char *dest, int len);
int lzwencoding(char *src, char *dest, int len);

/*********************************************************
 *NAME:          mapSaveCompressedMap
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/5/99
 *LAST MODIFIED: 1/5/99
 *PURPOSE:
 *  Saves a map to a compressed map structure. Returns
 *  compressed data length
 *
 *ARGUMENTS:
 *  value    - Pointer to the map data structure
 *  ss       - Pointer to the starts structure
 *  bs       - Pointer to the bases structure
 *  pb       - Pointer to the pillbox structure
 *  output   - Pointer to the data buffer
 *********************************************************/
int mapSaveCompressedMap(map *value, pillboxes *pb, bases *bs, starts *ss,
                         BYTE *output) {
  int returnValue; /* Value to return */
  BYTE *ptr;       /* Data pointer    */
  BYTE *ptr2;

  returnValue = 0;

  /* Bases */
  ptr = output;
  returnValue += basesGetBaseNetData(bs, ptr);
  ;
  ptr += SIZEOF_BASES;

  /* Pillboxes */
  pillsGetPillNetData(pb, ptr);
  returnValue += SIZEOF_PILLS;
  ptr += SIZEOF_PILLS;

  /* Starts */
  startsGetStartNetData(ss, ptr);
  returnValue += SIZEOF_STARTS;
  ptr += SIZEOF_STARTS;

  /* Map */
  ptr2 = (BYTE *)(*value)->mapItem;
  returnValue += lzwencoding((char *)(ptr2), (char *)ptr, sizeof(**value));
  return returnValue;
}

/*********************************************************
 *NAME:          mapLoadCompressedMap
 *AUTHOR:        John Morrison
 *CREATION DATE: 1/5/99
 *LAST MODIFIED: 1/5/99
 *PURPOSE:
 *  Reads a map in via a compressed map structure. Returns
 *  if the operation was successful or not
 *
 *ARGUMENTS:
 *  value    - Pointer to the map data structure
 *  ss       - Pointer to the starts structure
 *  bs       - Pointer to the bases structure
 *  pb       - Pointer to the pillbox structure
 *  input    - Pointer to the data buffer
 *  inputLen - Size of the buffer
 *********************************************************/
bool mapLoadCompressedMap(map *value, pillboxes *pb, bases *bs, starts *ss,
                          BYTE *input, int inputLen) {
  bool returnValue; /* Value to return */
  BYTE *ptr;        /* Data pointer    */
  int mapSize;
  BYTE *ptr2;

  returnValue = true;

  /* Bases */
  ptr = input;
  basesSetBaseCompressData(bs, ptr, SIZEOF_BASES);
  inputLen -= SIZEOF_BASES;
  ptr += SIZEOF_BASES;

  /* Pillboxes */
  pillsSetPillCompressData(pb, ptr, SIZEOF_PILLS);
  inputLen -= SIZEOF_PILLS;
  ptr += SIZEOF_PILLS;

  /* Starts */
  startsSetStartCompressData(ss, ptr, SIZEOF_STARTS);
  inputLen -= SIZEOF_STARTS;
  ptr += SIZEOF_STARTS;

  /* Map */
  ptr2 = (BYTE *)(*value)->mapItem;
  mapSize = lzwdecoding((char *)ptr, (char *)ptr2, inputLen);
  if (mapSize != sizeof(**value) - 2 * sizeof(mapNet)) {
    returnValue = false;
  }
  return returnValue;
}

/*********************************************************
 *NAME:          mapCenter
 *AUTHOR:        John Morrison
 *CREATION DATE: 13/6/00
 *LAST MODIFIED: 13/6/00
 *PURPOSE:
 *  Centers the map file and everything on it.
 *
 *ARGUMENTS:
 *  value   - Pointer to the map data structure
 *  ss      - Pointer to the starts structure
 *  bs      - Pointer to the bases structure
 *  pb      - Pointer to the pillbox structure
 *********************************************************/
void mapCenter(map *value, pillboxes *pb, bases *bs, starts *ss) {
  int bestTop; /* Our best gesses for centering */
  int bestBottom;
  int bestLeft;
  int bestRight;
  int guessTop; /* Our best gesses for centering */
  int guessBottom;
  int guessLeft;
  int guessRight;
  int addX; /* X and Y add amounts */
  int addY;
  int count1; /* Looping variable */
  int count2; /* Looping variable */
  map map2;   /* The new map to create */

  bestLeft = MAP_ARRAY_SIZE;
  bestRight = -1;
  bestTop = MAP_ARRAY_SIZE;
  bestBottom = -1;

  /* Pills */
  pillsGetMaxs(pb, &guessLeft, &guessRight, &guessTop, &guessBottom);
  if (guessLeft < bestLeft) {
    bestLeft = guessLeft;
  }
  if (guessRight < bestRight) {
    bestRight = guessRight;
  }
  if (guessTop < bestTop) {
    bestTop = guessTop;
  }
  if (guessBottom < bestBottom) {
    bestBottom = guessBottom;
  }
  /* Bases */
  basesGetMaxs(bs, &guessLeft, &guessRight, &guessTop, &guessBottom);
  if (guessLeft < bestLeft) {
    bestLeft = guessLeft;
  }
  if (guessRight < bestRight) {
    bestRight = guessRight;
  }
  if (guessTop < bestTop) {
    bestTop = guessTop;
  }
  if (guessBottom < bestBottom) {
    bestBottom = guessBottom;
  }
  /* Starts */
  startsGetMaxs(ss, &guessLeft, &guessRight, &guessTop, &guessBottom);
  if (guessLeft < bestLeft) {
    bestLeft = guessLeft;
  }
  if (guessRight < bestRight) {
    bestRight = guessRight;
  }
  if (guessTop < bestTop) {
    bestTop = guessTop;
  }
  if (guessBottom < bestBottom) {
    bestBottom = guessBottom;
  }
  for (count1 = 0; count1 <= MAP_ARRAY_LAST; count1++) {
    for (count2 = 0; count2 <= MAP_ARRAY_LAST; count2++) {
      if ((*value)->mapItem[count1][count2] != DEEP_SEA) {
        /* Center test */
        if (count1 < bestLeft) {
          bestLeft = count1;
        }
        if (count1 > bestRight) {
          bestRight = count1;
        }
        if (count2 < bestTop) {
          bestTop = count2;
        }
        if (count2 > bestBottom) {
          bestBottom = count2;
        }
      }
    }
  }

  /* Do the recentering */
  if (bestTop <= bestBottom && bestLeft <= bestRight) {
    addX = (255 / 2) - ((bestLeft + bestRight) / 2) - 1;
    addY = (255 / 2) - ((bestTop + bestBottom) / 2) - 1;
    if (addX != 0 && addY != 0) {
      /* It needs centering */
      mapCreate(&map2);
      for (count1 = bestLeft; count1 <= (BYTE)bestRight; count1++) {
        for (count2 = bestTop; count2 <= (BYTE)bestBottom; count2++) {
          map2->mapItem[(BYTE)(count1 + addX)][(BYTE)(count2 + addY)] =
              (*value)->mapItem[count1][count2];
        }
      }
      pillsMoveAll(pb, addX, addY);
      basesMoveAll(bs, addX, addY);
      startsMoveAll(ss, addX, addY);
      mapDestroy(value);
      *value = map2;
    }
  }
}
