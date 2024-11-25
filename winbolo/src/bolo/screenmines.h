/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024      Hyrum Wright.
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

// Screen Mines - Array of boolean values that report whether
// a tile on the screen whold have a mine on it
typedef struct screenMineObj *screenMines;
struct screenMineObj {
  bool mineItem[MAIN_BACK_BUFFER_SIZE_X][MAIN_BACK_BUFFER_SIZE_Y];
};
