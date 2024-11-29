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

#ifndef _TYPES_H
#define _TYPES_H

#include "global.h"
/* Defines */
#define MAX_BASES 16

/* Typedefs */

#define SIZEOF_BASES 260 /* 196 */
typedef struct {
  BYTE x; /* Co-ordinates on the map */
  BYTE y;
  BYTE owner;      /* should be 0xFF except in speciality maps */
  BYTE armour;     /* initial stocks of base. Maximum value 90 */
  BYTE shells;     /* initial stocks of base. Maximum value 90 */
  BYTE mines;      /* initial stocks of base. Maximum value 90 */
  BYTE refuelTime; /* Refuelling Time */
  int baseTime;    /* Time between stock updates */
  bool
      justStopped; /* Is this the first time the tank has stopped on the base */
} base;

typedef struct basesObj *bases;

struct basesObj {
  base item[MAX_BASES];
  BYTE numBases;
};

#define MAP_ARRAY_SIZE 256 /* maps are 256x256 units square */

typedef struct mapNetObj *mapNet;
struct mapNetObj {
  mapNet next;     /* Next item */
  mapNet prev;     /* Next item */
  BYTE mx;         /* X Map position */
  BYTE my;         /* Y Map position */
  BYTE terrain;    /* New terrain at that position */
  BYTE oldTerrain; /* The old terrain at that position */
  bool needSend;   /* Do we need to send this?         */
  BYTE length;     /* How long have we been waiting for the server? */
};

typedef struct mapObj *map;

/* Defines */
#define MAX_PILLS 16
#define SIZEOF_PILLS 145 /* 129 */

typedef struct {
  BYTE x; /* Co-ordinates on the map */
  BYTE y;
  BYTE owner;    /* should be 0xFF except in speciality maps */
  BYTE armour;   /* range 0-15 (dead pillbox = 0, full strength = 15) */
  BYTE speed;    /* typically 50. Time between shots, in 20ms units */
                 /* Lower values makes the pillbox start off 'angry' */
  bool inTank;   /* Is the pillbox in the tank? */
  BYTE reload;   /* Reload timer - When shoots goes to zero then counts back to
                    speed to fire again */
  BYTE coolDown; /* When hit it is set. When the cool down rate = 0 the speed is
                    notched up and cool down rate is set again */
  bool justSeen; /* True if this is the first time we have seen a tank */
} pillbox;

typedef struct pillsObj *pillboxes;

struct pillsObj {
  pillbox item[MAX_PILLS];
  BYTE numPills;
};

/* 25B = 25x8 = 200b needed to be allocated */
typedef struct vectorBodyObj *vectorBody;
struct vectorBodyObj {
  BYTE mass;             /* unsigned char, 1B */
  TURNTYPE angle;        /* float, 4B */
  TURNTYPE angle_prev;   /* float, 4B */
  SPEEDTYPE speedX;      /* float, 4B */
  SPEEDTYPE speedY;      /* float, 4B */
  SPEEDTYPE speedX_prev; /* float, 4B */
  SPEEDTYPE speedY_prev; /* float, 4B */
};

typedef struct tankCarrypbObj *tankCarryPb;
struct tankCarrypbObj {
  tankCarryPb next; /* Next item */
  BYTE pillNum;     /* The pill box number that is being carried */
};

/*
typedef struct {
    BYTE mass;
        TURNTYPE  angle;
        TURNTYPE  angle_prev;
        SPEEDTYPE speedX;
        SPEEDTYPE speedY;
        SPEEDTYPE speedX_prev;
        SPEEDTYPE speedY_prev;
        //TODO: add a factor to mean how 'bouncy' the object is
} vectorBodyObj;
*/

typedef struct tankObj *tank;

#define CRC_WORLDX_OFFSET 0
#define CRC_WORLDY_OFFSET (CRC_WORLDX_OFFSET + sizeof(WORLD))
#define CRC_ARMOUR_OFFSET (CRC_WORLDY_OFFSET + sizeof(WORLD))
#define CRC_SHELLS_OFFSET (CRC_ARMOUR_OFFSET + 1)
#define CRC_MINES_OFFSET (CRC_SHELLS_OFFSET + 1)
#define CRC_SPEED_OFFSET (CRC_MINES_OFFSET + 1)
#define CRC_TREES_OFFSET (CRC_SPEED_OFFSET + sizeof(SPEEDTYPE))
#define CRC_ANGLE_OFFSET (CRC_TREES_OFFSET + 1)
#define CRC_RELOAD_OFFSET (CRC_ANGLE_OFFSET + sizeof(TURNTYPE))
#define CRC_ONBOAT_OFFSET (CRC_RELOAD_OFFSET + 1)
#define CRC_SHOWSIGHT_OFFSET (CRC_ONBOAT_OFFSET + 1)
#define CRC_SIGHTLEN_OFFSET (CRC_SHOWSIGHT_OFFSET + 1)
#define CRC_NUMKILLS_OFFSET (CRC_SIGHTLEN_OFFSET + 1)
#define CRC_NUMDEATHS_OFFSET (CRC_NUMKILLS_OFFSET + sizeof(int))
#define CRC_DEATHWAIT_OFFSET (CRC_NUMDEATHS_OFFSET + sizeof(int))
#define CRC_WATERCOUNT_OFFSET (CRC_DEATHWAIT_OFFSET + 1)
#define CRC_OBSTRUCTED_OFFSET (CRC_WATERCOUNT_OFFSET + 1)
#define CRC_NEWTANK_OFFSET (CRC_OBSTRUCTED_OFFSET + 1)
#define CRC_AUTOSLOWDOWN_OFFSET (CRC_NEWTANK_OFFSET + 1)
#define CRC_AUTOHIDE_OFFSET (CRC_AUTOSLOWDOWN_OFFSET + 1)
#define CRC_JUSTFIRED_OFFSET (CRC_AUTOHIDE_OFFSET + 1)
#define CRC_TANK_SIZE (CRC_JUSTFIRED_OFFSET + 1)
/*
#define CRC_WORLDXPREV_OFFSET (CRC_TANK_SIZE + sizeof(WORLD))
#define CRC_WORLDYPREV_OFFSET (CRC_WORLDXPREV_SIZE + sizeof(WORLD))
#define CRC_WORLDXPREVPREV_OFFSET (CRC_WORLDYPREV_SIZE + sizeof(WORLD))
#define CRC_WORLDYPREVPREV_OFFSET (CRC_WORLDXPREVPREV_SIZE + sizeof(WORLD))
#define CRC_CARRYPILLS_OFFSET (CRC_WORLDYPREVPREV_SIZE + sizeof(tankCarryPb))
#define CRC_TANKSLIDETIMER_OFFSET (CRC_CARRYPILLS_SIZE + sizeof(BYTE))
#define CRC_VECTORBODY_OFFSET (CRC_TANKSLIDETIMER_SIZE + sizeof(vectorBody))
#define CRC_VECTORBODYCOLLIDE_OFFSET (CRC_VECTORBODY_SIZE + sizeof(vectorBody))
*/

#pragma pack(push, 1)

struct tankObj {
  WORLD x; /* World Co-ordinates */
  WORLD y;
  BYTE armour;     /* Amount of armour in tank. Maximum value 45 */
  BYTE shells;     /* Amount of shells in tank. Maximum value 45 */
  BYTE mines;      /* Amount of mines in tank. Maximum value 45 */
  SPEEDTYPE speed; /* Tank speed */
  BYTE trees;      /* Amount of trees in tank */
  TURNTYPE angle;  /* The angle the tank is pointing on 0-256 */
  BYTE reload;     /* Reload tick 0 is OK to shoot else counts back */
  bool onBoat;     /* Is the tank on a boat? */
  bool showSight;  /* Is the gunsight on or not */
  BYTE sightLen;   /* Length of the gunsight measured in map units */
  int numKills;    /* Number of kills the tank has had */
  int numDeaths;   /* Number of deaths the tank has had */
  BYTE deathWait;  /* How long it is going to be on the screen till it refreshes
                    */
  BYTE waterCount; /* Count for bubbles */
  bool obstructed; /* Used by brains. Did the tank hit anything */
  bool newTank;    /* Is this a new tank or not (ie just died */
  bool autoSlowdown;     /* Do we use autoslowdown or not */
  bool autoHideGunsight; /* Auto show/hide of gunsight enabled/disabled */
  bool justFired;        /* Did the tank just fire */
  BYTE tankHitCount; /* Number of times a tank has been hit to determine if they
                        are cheating */
  int crc;           /* CRC used to detect memory cheats */
  WORLD x_prev;      /* World Coordinates at last tick */
  WORLD y_prev;
  WORLD x_prev_prev;
  WORLD y_prev_prev;
  tankCarryPb carryPills;  /* The Pillboxes being carried */
  BYTE tankSlideTimer;     /* This will be used to determine how long the tank
                              should slide when hit */
  TURNTYPE tankSlideAngle; /* This holds the angle at which the tank was hit
                              with a shell */
  BYTE lastTankDeath; /* How did the most recent death to the tank occur? */
  vectorBody vectorBodyTank;    /* Holds tank's actual moving direction and
                                   component vectors (x and y axis speed) */
  vectorBody vectorBodyCollide; /* Holds physics stuff for what hit the tank */
};

#pragma pack(pop)

/* Defines */
#define MAX_STARTS 16
#define SIZEOF_STARTS 49

/* Typedefs */

typedef struct {
  BYTE x; /* Co-ordinates on the map */
  BYTE y;
  BYTE dir; /* Direction towards land from this start. Range 0-15 */
} start;

typedef struct startsObj *starts;

struct startsObj {
  start item[MAX_STARTS];
  BYTE numStarts;
};

typedef enum { lblNone, lblShort, lblLong } labelLen;

typedef enum { left, right, up, down } updateType;

typedef enum { aiNone, aiYes, aiYesAdvantage } aiType;

typedef enum { BsTrees, BsRoad, BsBuilding, BsPillbox, BsMine } buildSelect;

#endif
