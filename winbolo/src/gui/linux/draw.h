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
 *Name:          Draw
 *Filename:      draw.c
 *Author:        John Morrison
 *Creation Date: 13/12/98
 *Last Modified:  29/4/00
 *Purpose:
 *  System Specific Drawing routines (Uses Direct Draw)
 *********************************************************/

#ifndef _DRAW_H
#define _DRAW_H

#include <string.h>

#include "../../bolo/backend.h"
#include "../../bolo/global.h"
#include "SDL.h"

/*********************************************************
 *NAME:          drawSetup
 *AUTHOR:        John Morrison
 *CREATION DATE: 13/10/98
 *LAST MODIFIED:  29/4/00
 *PURPOSE:
 *  Sets up drawing systems, direct draw structures etc.
 *  Returns whether the operation was successful or not
 *
 *ARGUMENTS:
 *********************************************************/
bool drawSetup();

/*********************************************************
 *NAME:          drawCleanup
 *AUTHOR:        John Morrison
 *CREATION DATE: 13/12/98
 *LAST MODIFIED: 13/2/98
 *PURPOSE:
 *  Destroys and cleans up drawing systems, direct draw
 *  structures etc.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void drawCleanup(void);

/*********************************************************
 *NAME:          drawMainScreen
 *AUTHOR:        John Morrison
 *CREATION DATE: 31/10/98
 *LAST MODIFIED: 27/05/00
 *PURPOSE:
 *  Updates the Main Window View
 *
 *ARGUMENTS:
 *  value    - Pointer to the sceen structure
 *  tks      - Pointer to the screen tank structure
 *  gs       - Pointer to the screen gunsight structure
 *  sBullets - The screen Bullets structure
 *  lgms     - Screen Builder structure
 *  rcWindow - Window region
 *  showPillLabels - Show the pillbox labels?
 *  showBaseLabels - Show the base labels?
 *  srtDelay       - The start delay in ticks.
 *                  If greater then 0 should draw countdown
 *  isPillView     - TRUE if we are in pillbox view
 *  edgeX          - Edge X offset for smooth scrolling
 *  edgeY          - Edge Y offset for smooth scrolling
 *  useCursor      - True if to draw the cursor
 *  cursorLeft     - Cursor left position
 *  cursorTop      - Cursor Top position
 *********************************************************/
void drawMainScreen(const bolo::ScreenTiles &tiles,
                    const bolo::ScreenTankList &tks,
                    const std::optional<bolo::ScreenGunsight> &gunsight,
                    const bolo::ScreenBulletList &sBullets,
                    const bolo::ScreenLgmList &lgms, bool showPillLabels,
                    bool showBaseLabels, long srtDelay, bool isPillView,
                    int edgeX, int edgeY, bool useCursor, BYTE cursorLeft,
                    BYTE cursorTop);

/*********************************************************
 *NAME:          drawRedrawAll
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/12/98
 *LAST MODIFIED: 22/11/98
 *PURPOSE:
 *  Redraws everything (except the main view)
 *
 *ARGUMENTS:
 *  appInst         - Application Instance
 *  appWnd          - Application Window
 *  value           - The currently selected build icon
 *  rcWindow        - The window co-ords
 *  showPillsStatus - Should the the pill status be shown
 *  showBasesStatus - Should the the base status be shown
 *********************************************************/
void drawRedrawAll(int width, int height, buildSelect value,
                   const std::vector<baseAlliance> &bas,
                   const std::vector<pillAlliance> &pas,
                   const std::vector<bolo::tankAlliance> &tas,
                   bool showPillsStatus, bool showBasesStatus);

/*********************************************************
 *NAME:          drawSetupArrays
 *AUTHOR:        John Morrison
 *CREATION DATE: 28/5/00
 *LAST MODIFIED: 28/5/00
 *PURPOSE:
 *  Sets up the drawing arrays to improve efficeny and
 *  remove the giant switch statement every drawing loop
 *
 *ARGUMENTS:
 *********************************************************/
void drawSetupArrays();

void drawDownloadScreen(bool justBlack);

#endif
