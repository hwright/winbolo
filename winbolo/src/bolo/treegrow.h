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
 *Name:          Tree Growth
 *Filename:      treegrow.h
 *Author:        John Morrison
 *Creation Date: 22/1/99
 *Last Modified: 24/12/99
 *Purpose:
 *  Responsible for storing and calculating tree growth
 *********************************************************/

#ifndef TREEGROW_H
#define TREEGROW_H

#include <random>

#include "global.h"
#include "types.h"

/* The time a tree exist before growing unless we find a better spot */
#define TREEGROW_TIME 3000 /* 3700 */

/* Initial time delay before we build a tree. This will be overwritten
   before it gets called */
#define TREEGROW_INITIAL_TIME 30000
/* Initial scores to beat */
#define TREEGROW_INITIAL_SCORE -10000

/* Weights of the items */
#define TREE_GROW_FOREST 100        /* 67  100 */
#define TREE_GROW_GRASS 25          /* 10 17  25 */
#define TREE_GROW_RIVER 2           /* 7  10 */
#define TREE_GROW_BOAT 1            /* 3  5 */
#define TREE_GROW_DEEP_SEA 0        /* 1 */
#define TREE_GROW_DEEP_SWAMP 2      /* 1 */
#define TREE_GROW_DEEP_RUBBLE -2    /* -2  -1 */
#define TREE_GROW_BUILDING -20      /* -7  -5 */
#define TREE_GROW_HALF_BUILDING -15 /* -7  -5 */
#define TREE_GROW_CRATER -2         /* -2  -1  */
#define TREE_GROW_ROAD -100         /* -18 * -14  -10 */
#define TREE_GROW_MINE -7           /* -7 -5 */

/* Score must be greater then 0 for it to grow */
#define TREE_NO_GROW 0

class TreeGrowState {
 public:
  TreeGrowState();
  virtual ~TreeGrowState() = default;

  //  Game tick has happened. Calculate the growth scores
  //  Build new trees etc.
  //
  // ARGUMENTS:
  //  mp - Pointer to the map structure
  //  pb - Pointer to the pillboxes structure
  //  bs - Pointer to the bases structure
  void Update(map *mp, pillboxes *pb, bases *bs);

 private:
  // Add or update an item in the treeGrow data structure.
  // Replace the score if the score is greater then the
  // one that is already there and resets the timer to zero
  //
  // ARGUMENTS:
  //  x     - X co-ord
  //  y     - Y co-ord
  //  score - The score of the item
  void addItem(MapPoint point, int score);

  // Calculate the tree growth score for the a specific
  // map location.
  //
  // ARGUMENTS:
  //  mp - Pointer to the map structure
  //  pb - Pointer to the pillboxes structure
  //  bs - Pointer to the bases structure
  //  map_point - Map position
  int calcScore(map *mp, pillboxes *pb, bases *bs, MapPoint candidate);

  // Called once every tick to see if a tree should be grown
  // and removed from the data structure.
  //
  // ARGUMENTS:
  //  mp - Pointer to the map structure
  //  pb - Pointer to the pillboxes structure
  //  bs - Pointer to the bases structure
  void checkGrowTree(map *mp, pillboxes *pb, bases *bs);

  MapPoint seedling_;
  int time_;
  int score_;

  std::default_random_engine generator_;
  std::uniform_int_distribution<uint8_t> distribution_;
};

#endif /* TREEGROW_H */
