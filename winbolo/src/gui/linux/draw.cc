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

/* The size of the main window EXCLUDING Menus and Toolbar */
#define SCREEN_SIZE_X 515
#define SCREEN_SIZE_Y 325
#define MESSAGE_STRING_SIZE 68

#include "draw.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/xpm.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../bolo/backend.h"
#include "../../bolo/global.h"
#include "../../bolo/network.h"
#include "../../bolo/screentypes.h"
#include "../../bolo/tilenum.h"
#include "../../gui/lang.h"
#include "../../gui/linresource.h"
#include "../../gui/positions.h"
#include "../../lzw/lzw.h"
#include "../tiles.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "background.xph"
#include "messagebox.h"
#include "tiles.xph"

/* #include "..\winbolo.h"
#include "..\clientmutex.h"
#include "..\cursor.h"
#include "..\draw.h"
#include "..\font.h"
#include "..\lang.h"
*/
/* SDL Surfaces */
static SDL_Surface *lpScreen = nullptr;
static SDL_Surface *lpBackBuffer = nullptr;
static SDL_Surface *lpTiles = nullptr;
static SDL_Surface *lpBackground = nullptr;
static SDL_Surface *lpPillsStatus = nullptr;
static SDL_Surface *lpBasesStatus = nullptr;
static SDL_Surface *lpTankStatus = nullptr;
static TTF_Font *lpFont = nullptr;
static SDL_Color white = {0xFF, 0xFF, 0xFF, 0};
static SDL_Color black = {0, 0, 0, 0};

/* typedef int DWORD; */
/* Used for drawing the man status */
/* FIXME HBRUSH hManBrush = NULL;
HBRUSH hNarrowBrush = NULL;
HPEN hManPen = NULL;
*/
/* Used for storing time */
static DWORD g_dwFrameTime = 0;
/* Number of frames dispayed this second */
static DWORD g_dwFrameCount = 0;
/* The total frames per second for last second */
static DWORD g_dwFrameTotal = 0;

extern GtkWidget *drawingarea1;

static int drawPosX[255];
static int drawPosY[255];

void clientMutexWaitFor(void);
void clientMutexRelease(void);

int drawGetFrameRate() { return g_dwFrameTotal; }

namespace {

// These are from
// https://web.archive.org/web/20160326085538/http://content.gpwiki.org/index.php/SDL:Tutorials:Drawing_and_Filling_Circles
void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
  Uint8 *target_pixel = (Uint8 *)surface->pixels + y * surface->pitch +
                        x * surface->format->BytesPerPixel;
  *(Uint32 *)target_pixel = pixel;
}

// This is an implementation of the Midpoint Circle Algorithm
// found on Wikipedia at the following link:
//
//   http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
//
// The algorithm elegantly draws a circle quickly, using a
// set_pixel function for clarity.
void draw_circle(SDL_Surface *surface, int n_cx, int n_cy, int radius,
                 Uint32 pixel) {
  // if the first pixel in the screen is represented by (0,0) (which is in sdl)
  // remember that the beginning of the circle is not in the middle of the pixel
  // but to the left-top from it:

  double error = (double)-radius;
  double x = (double)radius - 0.5;
  double y = (double)0.5;
  double cx = n_cx - 0.5;
  double cy = n_cy - 0.5;

  while (x >= y) {
    set_pixel(surface, (int)(cx + x), (int)(cy + y), pixel);
    set_pixel(surface, (int)(cx + y), (int)(cy + x), pixel);

    if (x != 0) {
      set_pixel(surface, (int)(cx - x), (int)(cy + y), pixel);
      set_pixel(surface, (int)(cx + y), (int)(cy - x), pixel);
    }

    if (y != 0) {
      set_pixel(surface, (int)(cx + x), (int)(cy - y), pixel);
      set_pixel(surface, (int)(cx - y), (int)(cy + x), pixel);
    }

    if (x != 0 && y != 0) {
      set_pixel(surface, (int)(cx - x), (int)(cy - y), pixel);
      set_pixel(surface, (int)(cx - y), (int)(cy - x), pixel);
    }

    error += y;
    ++y;
    error += y;

    if (error >= 0) {
      --x;
      error -= x;
      error -= x;
    }
  }
}

void fill_circle(SDL_Surface *surface, int cx, int cy, int radius,
                 Uint32 pixel) {
  // Note that there is more to altering the bitrate of this
  // method than just changing this value.  See how pixels are
  // altered at the following web page for tips:
  //   http://www.libsdl.org/intro.en/usingvideo.html
  static const int BPP = 4;

  double r = (double)radius;

  for (double dy = 1; dy <= r; dy += 1.0) {
    // This loop is unrolled a bit, only iterating through half of the
    // height of the circle.  The result is used to draw a scan line and
    // its mirror image below it.

    // The following formula has been simplified from our original.  We
    // are using half of the width of the circle because we are provided
    // with a center and we need left/right coordinates.

    double dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
    int x = cx - dx;

    // Grab a pointer to the left-most pixel for each half of the circle
    Uint8 *target_pixel_a = (Uint8 *)surface->pixels +
                            ((int)(cy + r - dy)) * surface->pitch + x * BPP;
    Uint8 *target_pixel_b = (Uint8 *)surface->pixels +
                            ((int)(cy - r + dy)) * surface->pitch + x * BPP;

    for (; x <= cx + dx; x++) {
      *(Uint32 *)target_pixel_a = pixel;
      *(Uint32 *)target_pixel_b = pixel;
      target_pixel_a += BPP;
      target_pixel_b += BPP;
    }
  }
}

void draw_line(SDL_Surface *surface, int x0, int y0, int x1, int y1,
               Uint32 pixel) {
  int dx = x1 - x0;
  int dy = y1 - y0;
  int incX = dx < 0 ? -1 : dx > 0 ? 1 : 0;
  int incY = dy < 0 ? -1 : dy > 0 ? 1 : 0;
  dx = abs(dx);
  dy = abs(dy);

  if (dy == 0) {
    // horizontal line
    for (int x = x0; x != x1 + incX; x += incX)
      set_pixel(surface, x, y0, pixel);
  } else if (dx == 0) {
    // vertical line
    for (int y = y0; y != y1 + incY; y += incY)
      set_pixel(surface, x0, y, pixel);
  } else if (dx >= dy) {
    // more horizontal than vertical
    int slope = 2 * dy;
    int error = -dx;
    int errorInc = -2 * dx;
    int y = y0;

    for (int x = x0; x != x1 + incX; x += incX) {
      set_pixel(surface, x, y, pixel);
      error += slope;

      if (error >= 0) {
        y += incY;
        error += errorInc;
      }
    }
  } else {
    // more vertical than horizontal
    int slope = 2 * dx;
    int error = -dy;
    int errorInc = -2 * dy;
    int x = x0;

    for (int y = y0; y != y1 + incY; y += incY) {
      set_pixel(surface, x, y, pixel);
      error += slope;

      if (error >= 0) {
        x += incX;
        error += errorInc;
      }
    }
  }
}

// Draws the background graphic. Returns if the operation
// is successful or not.
//
// ARGUMENTS:
bool drawBackground() {
  bool returnValue = false; /* Value to return */
  SDL_Rect destRect{.x = 0,
                    .y = 0,
                    .w = static_cast<Uint16>(lpBackground->w),
                    .h = static_cast<Uint16>(lpBackground->h)};
  if (SDL_BlitSurface(lpBackground, nullptr, lpScreen, &destRect) == 0) {
    returnValue = true;
    SDL_UpdateRect(lpScreen, 0, 0, 0, 0);
  }
  return returnValue;
}
}

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
bool drawSetup() {
  bool returnValue; /* Value to return */
  SDL_Rect in;      /* Used for copying the bases & pills icon in */
  SDL_Rect out;     /* Used for copying the bases & pills icon in */
  char fileName[FILENAME_MAX];

  BYTE* buff = new BYTE[80438];
  /* Get tmp file */
  snprintf(fileName, sizeof(fileName), "%s/lbXXXXXX", g_get_tmp_dir());
  int ret = lzwdecoding((char *)TILES_IMAGE, (char *)buff, 36499);
  if (ret != 80438) {
    free(buff);
    MessageBox("Can't load graphics file", DIALOG_BOX_TITLE);
    return FALSE;
  }

  returnValue = TRUE;
  lpScreen = SDL_SetVideoMode(SCREEN_SIZE_X, SCREEN_SIZE_Y, 0, 0);
  if (lpScreen == nullptr) {
    returnValue = FALSE;
    MessageBox("Can't build main surface", DIALOG_BOX_TITLE);
  }

  /* Create the back buffer surface */
  if (returnValue == TRUE) {
    SDL_Surface *lpTemp = SDL_CreateRGBSurface(
        SDL_HWSURFACE, MAIN_BACK_BUFFER_SIZE_X * TILE_SIZE_X,
        MAIN_BACK_BUFFER_SIZE_Y * TILE_SIZE_Y, 16, 0, 0, 0, 0);
    if (lpTemp == nullptr) {
      returnValue = FALSE;
      MessageBox("Can't build a back buffer", DIALOG_BOX_TITLE);
    } else {
      lpBackBuffer = SDL_DisplayFormat(lpTemp);
      SDL_FreeSurface(lpTemp);
      if (lpBackBuffer == nullptr) {
        returnValue = FALSE;
        MessageBox("Can't build a back buffer", DIALOG_BOX_TITLE);
      }
    }
  }

  /* Create the tile buffer and copy the bitmap into it */
  if (returnValue == TRUE) {
    /* Create the buffer */
    FILE *fp = fopen(fileName, "wb");
    fwrite(buff, 80438, 1, fp);
    fclose(fp);
    lpTiles = SDL_LoadBMP(fileName);
    unlink(fileName);
    if (lpTiles == nullptr) {
      returnValue = FALSE;
      MessageBox("Can't load graphics file", DIALOG_BOX_TITLE);
    } else {
      /* Colour key */
      ret = SDL_SetColorKey(lpTiles, SDL_SRCCOLORKEY,
                            SDL_MapRGB(lpTiles->format, 0, 0xFF, 0));
      if (ret == -1) {
        MessageBox("Couldn't map colour key", DIALOG_BOX_TITLE);
        returnValue = FALSE;
      } else {
        //      lpTiles = SDL_DisplayFormat(lpTemp);
        //	SDL_FreeSurface(lpTemp);
        if (lpTiles == nullptr) {
          returnValue = FALSE;
          MessageBox("Can't build a tile file", DIALOG_BOX_TITLE);
        }
      }
    }
  }

  out.w = TILE_SIZE_X;
  out.h = TILE_SIZE_Y;
  in.w = TILE_SIZE_X;
  in.h = TILE_SIZE_Y;

  // Load the background surface
  if (returnValue == TRUE) {
    SDL_RWops *rw = SDL_RWFromMem(background_png, background_png_len);
    lpBackground = IMG_LoadPNG_RW(rw);
    if (lpBackground == nullptr) {
      returnValue = FALSE;
      MessageBox("Can't load background image", DIALOG_BOX_TITLE);
    }
    SDL_FreeRW(rw);
  }

  /* Create the Base status window */
  if (returnValue == TRUE) {
    SDL_Surface *lpTemp = SDL_CreateRGBSurface(
        0, STATUS_BASES_WIDTH, STATUS_BASES_HEIGHT, 16, 0, 0, 0, 0);
    if (lpTemp == nullptr) {
      returnValue = FALSE;
      MessageBox("Can't build a status base buffer", DIALOG_BOX_TITLE);
    } else {
      /* Fill the surface black */
      lpBasesStatus = SDL_DisplayFormat(lpTemp);
      SDL_FreeSurface(lpTemp);
      if (lpBasesStatus == nullptr) {
        returnValue = FALSE;
        MessageBox("Can't build a status base buffer", DIALOG_BOX_TITLE);
      } else {
        SDL_Rect fill{.x = 0,
                      .y = 0,
                      .w = static_cast<Uint16>(lpBasesStatus->w),
                      .h = static_cast<Uint16>(lpBasesStatus->h)};
        SDL_FillRect(lpBasesStatus, &fill,
                     SDL_MapRGB(lpBasesStatus->format, 0, 0, 0));
        /* Copy in the icon */
        in.x = BASE_GOOD_X;
        in.y = BASE_GOOD_Y;
        out.x = STATUS_BASES_MIDDLE_ICON_X;
        out.y = STATUS_BASES_MIDDLE_ICON_Y;
        SDL_BlitSurface(lpTiles, &in, lpBasesStatus, &out);
      }
    }
  }
  /* Makes the pills status */
  if (returnValue == TRUE) {
    SDL_Surface *lpTemp = SDL_CreateRGBSurface(
        0, STATUS_PILLS_WIDTH, STATUS_PILLS_HEIGHT, 16, 0, 0, 0, 0);
    if (lpTemp == nullptr) {
      returnValue = FALSE;
      MessageBox("Can't build a status pills buffer", DIALOG_BOX_TITLE);
    } else {
      lpPillsStatus = SDL_DisplayFormat(lpTemp);
      SDL_FreeSurface(lpTemp);
      if (lpTemp == FALSE) {
        returnValue = FALSE;
        MessageBox("Can't build a status pills buffer", DIALOG_BOX_TITLE);
      } else {
        /* Fill the surface black */
        SDL_Rect fill{.x = 0,
                      .y = 0,
                      .w = static_cast<Uint16>(lpPillsStatus->w),
                      .h = static_cast<Uint16>(lpPillsStatus->h)};
        SDL_FillRect(lpPillsStatus, &fill,
                     SDL_MapRGB(lpPillsStatus->format, 0, 0, 0));
        /* Copy in the icon */
        in.x = PILL_GOOD15_X;
        in.y = PILL_GOOD15_Y;
        out.x = STATUS_PILLS_MIDDLE_ICON_X;
        out.y = STATUS_PILLS_MIDDLE_ICON_Y;
        SDL_BlitSurface(lpTiles, &in, lpPillsStatus, &out);
      }
    }
  }

  /* Makes the tanks status */
  if (returnValue == TRUE) {
    SDL_Surface *lpTemp = SDL_CreateRGBSurface(
        0, STATUS_TANKS_WIDTH, STATUS_TANKS_HEIGHT, 16, 0, 0, 0, 0);
    if (lpTemp == nullptr) {
      returnValue = FALSE;
      MessageBox("Can't build a status tanks buffer", DIALOG_BOX_TITLE);
    } else {
      lpTankStatus = SDL_DisplayFormat(lpTemp);
      SDL_FreeSurface(lpTemp);
      if (lpTankStatus == nullptr) {
        returnValue = FALSE;
        MessageBox("Can't build a status tanks buffer", DIALOG_BOX_TITLE);
      } else {
        /* Fill the surface black */
        SDL_Rect fill{.x = 0,
                      .y = 0,
                      .w = static_cast<Uint16>(lpTankStatus->w),
                      .h = static_cast<Uint16>(lpTankStatus->h)};
        SDL_FillRect(lpTankStatus, &fill,
                     SDL_MapRGB(lpTankStatus->format, 0, 0, 0));
        /* Copy in the icon */
        in.x = TANK_SELF_0_X;
        in.y = TANK_SELF_0_Y;
        out.x = STATUS_TANKS_MIDDLE_ICON_X;
        out.y = STATUS_TANKS_MIDDLE_ICON_Y;
        SDL_BlitSurface(lpTiles, &in, lpTankStatus, &out);
      }
    }
  }
  if (returnValue == TRUE) {
    if (TTF_Init() < 0) {
      MessageBox("Couldn't init TTF rasteriser", DIALOG_BOX_TITLE);
      returnValue = FALSE;
    } else {
      lpFont = TTF_OpenFont("cour.ttf", 12);
      if (lpFont == nullptr) {
        MessageBox(
            "Couldn't open font file.\n Please place a courier font\ncalled "
            "\"cour.ttf\" in your\nLinBolo directory.",
            DIALOG_BOX_TITLE);
        returnValue = FALSE;
      }
    }
  }

  g_dwFrameTime = SDL_GetTicks();
  g_dwFrameCount = 0;
  drawSetupArrays();

  delete[] buff;
  return returnValue;
}

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
void drawCleanup(void) {
  if (lpTiles != nullptr) {
    SDL_FreeSurface(lpTiles);
    lpTiles = nullptr;
  }
  if (lpBackBuffer != nullptr) {
    SDL_FreeSurface(lpBackBuffer);
    lpBackBuffer = nullptr;
  }
  if (lpBasesStatus != nullptr) {
    SDL_FreeSurface(lpBasesStatus);
    lpBasesStatus = nullptr;
  }
  if (lpPillsStatus != nullptr) {
    SDL_FreeSurface(lpPillsStatus);
    lpPillsStatus = nullptr;
  }
  if (lpTankStatus != nullptr) {
    SDL_FreeSurface(lpTankStatus);
    lpTankStatus = nullptr;
  }
  if (lpFont != nullptr) {
    TTF_CloseFont(lpFont);
    TTF_Quit();
  }
  if (lpBackground != nullptr) {
    SDL_FreeSurface(lpBackground);
    lpBackground = nullptr;
  }
  if (lpScreen != nullptr) {
    SDL_FreeSurface(lpScreen);
    lpScreen = nullptr;
  }
}

int lastManX = 0;
int lastManY = 0;

/*********************************************************
 *NAME:          drawSetManClear
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/98
 *LAST MODIFIED: 18/1/98
 *PURPOSE:
 *  Clears the lgm status display.
 *
 *ARGUMENTS:
 *********************************************************/
void drawSetManClear() {
  int left, top, width, height;

  //  jm removed today gdk_threads_enter();
  left = MAN_STATUS_X;
  top = MAN_STATUS_Y;
  width = MAN_STATUS_WIDTH + 5;
  height = MAN_STATUS_HEIGHT + 5;
  gdk_draw_rectangle(drawingarea1->window, drawingarea1->style->black_gc, TRUE,
                     left, top, width, height);
  lastManX = 0;
  lastManY = 0;
  //  gdk_threads_leave();
}

/*********************************************************
 *NAME:          drawSetManStatus
 *AUTHOR:        John Morrison
 *CREATION DATE: 18/1/98
 *LAST MODIFIED: 18/1/98
 *PURPOSE:
 *  Draws the man arrow status. If isDead is set to true
 *  then the angle is ignored.
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *  isDead - Is the man dead
 *  angle  - The angle the item is facing
 *********************************************************/
void drawSetManStatus(bool isDead, TURNTYPE angle, bool needLocking) {
  // TURNTYPE oldAngle; /* Copy of the angle parameter */
  double dbAngle; /* Angle in radians */
  double dbTemp;
  int addX;        /* X And and Y co-ordinates */
  int addY;
  int left, top;

  // Clear the area
  SDL_Rect fill;
  fill.x = MAN_STATUS_X;
  fill.y = MAN_STATUS_Y;
  fill.w = MAN_STATUS_WIDTH + 5;
  fill.h = MAN_STATUS_HEIGHT + 5;
  SDL_FillRect(lpScreen, &fill, SDL_MapRGB(lpScreen->format, 0, 0, 0));

  // oldAngle = angle;
  angle += BRADIANS_SOUTH;
  if (angle >= BRADIANS_MAX) {
    angle -= BRADIANS_MAX;
  }

  if (angle >= BRADIANS_NORTH && angle < BRADIANS_EAST) {
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;

    addX = MAN_STATUS_CENTER_X;
    addY = MAN_STATUS_CENTER_Y;
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX += (int)dbTemp;
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY -= (int)dbTemp;
  } else if (angle >= BRADIANS_EAST && angle < BRADIANS_SOUTH) {
    angle = BRADIANS_SOUTH - angle;
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;

    addX = MAN_STATUS_CENTER_X;
    addY = MAN_STATUS_CENTER_Y;
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX += (int)dbTemp;
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY += (int)dbTemp;
  } else if (angle >= BRADIANS_SOUTH && angle < BRADIANS_WEST) {
    angle = BRADIANS_WEST - angle;
    angle = BRADIANS_EAST - angle;
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;

    addX = MAN_STATUS_CENTER_X;
    addY = MAN_STATUS_CENTER_Y;
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX -= (int)dbTemp;
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY += (int)dbTemp;
  } else {
    angle = (float)BRADIANS_MAX - angle;
    /* Convert bradians to degrees */
    dbAngle = (DEGREES_MAX / BRADIANS_MAX) * angle;
    /* Convert degrees to radians */
    dbAngle = (dbAngle / DEGREES_MAX) * RADIANS_MAX;

    addX = MAN_STATUS_CENTER_X;
    addY = MAN_STATUS_CENTER_Y;
    dbTemp = MAN_STATUS_RADIUS * (sin(dbAngle));
    addX -= (int)dbTemp;
    dbTemp = MAN_STATUS_RADIUS * (cos(dbAngle));
    addY -= (int)dbTemp;
  }

  left = MAN_STATUS_X;
  top = MAN_STATUS_Y;

  // SDL does not give us native arc, circle or line methods, so we have
  // to compute our own.
  SDL_LockSurface(lpScreen);
  addY += top;
  addX += left;
  if (isDead == true) {
    /* Draw dead circle */
    fill_circle(lpScreen, left + MAN_STATUS_CENTER_X, top + MAN_STATUS_CENTER_Y,
                MAN_STATUS_WIDTH / 2,
                SDL_MapRGB(lpScreen->format, 0xFF, 0xFF, 0xFF));
    lastManX = 0;
  } else {
    draw_circle(lpScreen, left + MAN_STATUS_CENTER_X, top + MAN_STATUS_CENTER_Y,
                MAN_STATUS_WIDTH / 2, SDL_MapRGB(lpScreen->format, 0xFF, 0xFF, 0xFF));
    draw_line(lpScreen, MAN_STATUS_CENTER_X + left, top + MAN_STATUS_CENTER_Y,
              addX, addY, SDL_MapRGB(lpScreen->format, 0xFF, 0xFF, 0xFF));

    lastManX = addX;
    lastManY = addY;
  }
  SDL_UnlockSurface(lpScreen);
  SDL_UpdateRect(lpScreen, left, top, MAN_STATUS_WIDTH, MAN_STATUS_HEIGHT);
}

/*********************************************************
 *NAME:          drawShells
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/12/98
 *LAST MODIFIED: 26/12/98
 *PURPOSE:
 *  Draws shells and explosions on the backbuffer.
 *
 *ARGUMENTS:
 *  sBullets - The screen Bullets data structure
 *********************************************************/
void drawShells(const bolo::ScreenBulletList &sBullets) {
  SDL_Rect output; /* Output Rectangle */
  SDL_Rect dest;

  for (const auto &bullet : sBullets) {
    dest.x = (bullet.pos.x * TILE_SIZE_X) + bullet.px;
    dest.y = (bullet.pos.y * TILE_SIZE_Y) + bullet.py;
    switch (bullet.frame) {
      case SHELL_EXPLOSION8:
        output.x = EXPLOSION8_X;
        output.y = EXPLOSION8_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION7:
        output.x = EXPLOSION7_X;
        output.y = EXPLOSION7_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION6:
        output.x = EXPLOSION6_X;
        output.y = EXPLOSION6_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION5:
        output.x = EXPLOSION5_X;
        output.y = EXPLOSION5_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION4:
        output.x = EXPLOSION4_X;
        output.y = EXPLOSION4_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION3:
        output.x = EXPLOSION3_X;
        output.y = EXPLOSION3_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION2:
        output.x = EXPLOSION2_X;
        output.y = EXPLOSION2_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_EXPLOSION1:
        output.x = EXPLOSION1_X;
        output.y = EXPLOSION1_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
      case SHELL_DIR0:
        output.x = SHELL_0_X;
        output.y = SHELL_0_Y;
        output.w = SHELL_0_WIDTH;
        output.h = SHELL_0_HEIGHT;
        break;
      case SHELL_DIR1:
        output.x = SHELL_1_X;
        output.y = SHELL_1_Y;
        output.w = SHELL_1_WIDTH;
        output.h = SHELL_1_HEIGHT;
        break;
      case SHELL_DIR2:
        output.x = SHELL_2_X;
        output.y = SHELL_2_Y;
        output.w = SHELL_2_WIDTH;
        output.h = SHELL_2_HEIGHT;
        break;
      case SHELL_DIR3:
        output.x = SHELL_3_X;
        output.y = SHELL_3_Y;
        output.w = SHELL_3_WIDTH;
        output.h = SHELL_3_HEIGHT;
        break;
      case SHELL_DIR4:
        output.x = SHELL_4_X;
        output.y = SHELL_4_Y;
        output.w = SHELL_4_WIDTH;
        output.h = SHELL_4_HEIGHT;
        break;
      case SHELL_DIR5:
        output.x = SHELL_5_X;
        output.y = SHELL_5_Y;
        output.w = SHELL_5_WIDTH;
        output.h = SHELL_5_HEIGHT;
        break;
      case SHELL_DIR6:
        output.x = SHELL_6_X;
        output.y = SHELL_6_Y;
        output.w = SHELL_6_WIDTH;
        output.h = SHELL_6_HEIGHT;
        break;
      case SHELL_DIR7:
        output.x = SHELL_7_X;
        output.y = SHELL_7_Y;
        output.w = SHELL_7_WIDTH;
        output.h = SHELL_7_HEIGHT;
        break;
      case SHELL_DIR8:
        output.x = SHELL_8_X;
        output.y = SHELL_8_Y;
        output.w = SHELL_8_WIDTH;
        output.h = SHELL_8_HEIGHT;
        break;
      case SHELL_DIR9:
        output.x = SHELL_9_X;
        output.y = SHELL_9_Y;
        output.w = SHELL_9_WIDTH;
        output.h = SHELL_9_HEIGHT;
        break;
      case SHELL_DIR10:
        output.x = SHELL_10_X;
        output.y = SHELL_10_Y;
        output.w = SHELL_10_WIDTH;
        output.h = SHELL_10_HEIGHT;
        break;
      case SHELL_DIR11:
        output.x = SHELL_11_X;
        output.y = SHELL_11_Y;
        output.w = SHELL_11_WIDTH;
        output.h = SHELL_11_HEIGHT;
        break;
      case SHELL_DIR12:
        output.x = SHELL_12_X;
        output.y = SHELL_12_Y;
        output.w = SHELL_12_WIDTH;
        output.h = SHELL_12_HEIGHT;
        break;
      case SHELL_DIR13:
        output.x = SHELL_13_X;
        output.y = SHELL_13_Y;
        output.w = SHELL_13_WIDTH;
        output.h = SHELL_13_HEIGHT;
        break;
      case SHELL_DIR14:
        output.x = SHELL_14_X;
        output.y = SHELL_14_Y;
        output.w = SHELL_14_WIDTH;
        output.h = SHELL_14_HEIGHT;
        break;
      case SHELL_DIR15:
        output.x = SHELL_15_X;
        output.y = SHELL_15_Y;
        output.w = SHELL_15_WIDTH;
        output.h = SHELL_15_HEIGHT;
        break;
    }
    dest.w = output.w;
    dest.h = output.h;
    SDL_BlitSurface(lpTiles, &output, lpBackBuffer, &dest);
  }
}

/*********************************************************
* *NAME:          drawTankLabel
* *AUTHOR:        John Morrison
*
*CREATION DATE: 2/2/98
*LAST MODIFIED: 2/2/98
*PURPOSE:
Draws the tank label if required.
*
*ARGUMENTS:
*  str - The string identifer of the tank
*  mx  - Tank map X position
*  my  - Tank map Y position
*  px  - Tank pixel offset
*  py  - Tank pixel offset
*********************************************************/
void drawTankLabel(const char *str, int mx, int my, BYTE px, BYTE py) {
  int len;       /* Length of the string */
  SDL_Rect dest; /* Defines the text rectangle */
  SDL_Surface *lpTextSurface;

  len = strlen(str);

  if (len > 0) {
    /* Draw it on the back buffer */
    lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
    if (lpTextSurface) {
      dest.x = ((mx + 1) * TILE_SIZE_X) + (px + 1);
      dest.y = my * TILE_SIZE_Y + py;
      dest.w = lpTextSurface->w;
      dest.h = lpTextSurface->h;
      /* Fix displaying off the edge of the screen */
      /* FIXME: Obsolete if ((x + textRect.right) > (MAIN_BACK_BUFFER_SIZE_X *
      (zf * TILE_SIZE_X))) { textRect.right = zf * ((MAIN_BACK_BUFFER_SIZE_X *
      TILE_SIZE_X) - x);
      }
      if ((y + textRect.bottom) > (MAIN_BACK_BUFFER_SIZE_Y * (zf *
      TILE_SIZE_Y))) { textRect.bottom = zf * ((MAIN_BACK_BUFFER_SIZE_Y *
      TILE_SIZE_Y) - y);
      } */
      /* Make it transparent */
      SDL_SetColorKey(lpTextSurface, SDL_SRCCOLORKEY,
                      SDL_MapRGB(lpTextSurface->format, 0, 0, 0));
      /* Output it */
      SDL_BlitSurface(lpTextSurface, nullptr, lpBackBuffer, &dest);
      SDL_UpdateRects(lpBackBuffer, 1, &dest);
      SDL_FreeSurface(lpTextSurface);
    }
  }
}

/*********************************************************
 *NAME:          drawTanks
 *AUTHOR:        John Morrison
 *CREATION DATE: 6/1/99
 *LAST MODIFIED: 2/2/99
 *PURPOSE:
 *  Draws tanks on the backbuffer.
 *
 *ARGUMENTS:
 *  tks - The screen Tanks data structure
 *********************************************************/
void drawTanks(const bolo::ScreenTankList &tks) {
  SDL_Rect output; /* Source Rectangle */
  SDL_Rect dest;

  output.w = TILE_SIZE_X;
  output.h = TILE_SIZE_Y;
  dest.w = output.w;
  dest.h = output.h;
  for (const auto &tank : tks.tanks) {
    switch (tank.frame) {
      case TANK_SELF_0:
        output.x = TANK_SELF_0_X;
        output.y = TANK_SELF_0_Y;
        break;
      case TANK_SELF_1:
        output.x = TANK_SELF_1_X;
        output.y = TANK_SELF_1_Y;
        break;
      case TANK_SELF_2:
        output.x = TANK_SELF_2_X;
        output.y = TANK_SELF_2_Y;
        break;
      case TANK_SELF_3:
        output.x = TANK_SELF_3_X;
        output.y = TANK_SELF_3_Y;
        break;
      case TANK_SELF_4:
        output.x = TANK_SELF_4_X;
        output.y = TANK_SELF_4_Y;
        break;
      case TANK_SELF_5:
        output.x = TANK_SELF_5_X;
        output.y = TANK_SELF_5_Y;
        break;
      case TANK_SELF_6:
        output.x = TANK_SELF_6_X;
        output.y = TANK_SELF_6_Y;
        break;
      case TANK_SELF_7:
        output.x = TANK_SELF_7_X;
        output.y = TANK_SELF_7_Y;
        break;
      case TANK_SELF_8:
        output.x = TANK_SELF_8_X;
        output.y = TANK_SELF_8_Y;
        break;
      case TANK_SELF_9:
        output.x = TANK_SELF_9_X;
        output.y = TANK_SELF_9_Y;
        break;
      case TANK_SELF_10:
        output.x = TANK_SELF_10_X;
        output.y = TANK_SELF_10_Y;
        break;
      case TANK_SELF_11:
        output.x = TANK_SELF_11_X;
        output.y = TANK_SELF_11_Y;
        break;
      case TANK_SELF_12:
        output.x = TANK_SELF_12_X;
        output.y = TANK_SELF_12_Y;
        break;
      case TANK_SELF_13:
        output.x = TANK_SELF_13_X;
        output.y = TANK_SELF_13_Y;
        break;
      case TANK_SELF_14:
        output.x = TANK_SELF_14_X;
        output.y = TANK_SELF_14_Y;
        break;
      case TANK_SELF_15:
        output.x = TANK_SELF_15_X;
        output.y = TANK_SELF_15_Y;
        break;
      case TANK_SELFBOAT_0:
        output.x = TANK_SELFBOAT_0_X;
        output.y = TANK_SELFBOAT_0_Y;
        break;
      case TANK_SELFBOAT_1:
        output.x = TANK_SELFBOAT_1_X;
        output.y = TANK_SELFBOAT_1_Y;
        break;
      case TANK_SELFBOAT_2:
        output.x = TANK_SELFBOAT_2_X;
        output.y = TANK_SELFBOAT_2_Y;
        break;
      case TANK_SELFBOAT_3:
        output.x = TANK_SELFBOAT_3_X;
        output.y = TANK_SELFBOAT_3_Y;
        break;
      case TANK_SELFBOAT_4:
        output.x = TANK_SELFBOAT_4_X;
        output.y = TANK_SELFBOAT_4_Y;
        break;
      case TANK_SELFBOAT_5:
        output.x = TANK_SELFBOAT_5_X;
        output.y = TANK_SELFBOAT_5_Y;
        break;
      case TANK_SELFBOAT_6:
        output.x = TANK_SELFBOAT_6_X;
        output.y = TANK_SELFBOAT_6_Y;
        break;
      case TANK_SELFBOAT_7:
        output.x = TANK_SELFBOAT_7_X;
        output.y = TANK_SELFBOAT_7_Y;
        break;
      case TANK_SELFBOAT_8:
        output.x = TANK_SELFBOAT_8_X;
        output.y = TANK_SELFBOAT_8_Y;
        break;
      case TANK_SELFBOAT_9:
        output.x = TANK_SELFBOAT_9_X;
        output.y = TANK_SELFBOAT_9_Y;
        break;
      case TANK_SELFBOAT_10:
        output.x = TANK_SELFBOAT_10_X;
        output.y = TANK_SELFBOAT_10_Y;
        break;
      case TANK_SELFBOAT_11:
        output.x = TANK_SELFBOAT_11_X;
        output.y = TANK_SELFBOAT_11_Y;
        break;
      case TANK_SELFBOAT_12:
        output.x = TANK_SELFBOAT_12_X;
        output.y = TANK_SELFBOAT_12_Y;
        break;
      case TANK_SELFBOAT_13:
        output.x = TANK_SELFBOAT_13_X;
        output.y = TANK_SELFBOAT_13_Y;
        break;
      case TANK_SELFBOAT_14:
        output.x = TANK_SELFBOAT_14_X;
        output.y = TANK_SELFBOAT_14_Y;
        break;
      case TANK_SELFBOAT_15:
        output.x = TANK_SELFBOAT_15_X;
        output.y = TANK_SELFBOAT_15_Y;
        break;
      case TANK_GOOD_0:
        output.x = TANK_GOOD_0_X;
        output.y = TANK_GOOD_0_Y;
        break;
      case TANK_GOOD_1:
        output.x = TANK_GOOD_1_X;
        output.y = TANK_GOOD_1_Y;
        break;
      case TANK_GOOD_2:
        output.x = TANK_GOOD_2_X;
        output.y = TANK_GOOD_2_Y;
        break;
      case TANK_GOOD_3:
        output.x = TANK_GOOD_3_X;
        output.y = TANK_GOOD_3_Y;
        break;
      case TANK_GOOD_4:
        output.x = TANK_GOOD_4_X;
        output.y = TANK_GOOD_4_Y;
        break;
      case TANK_GOOD_5:
        output.x = TANK_GOOD_5_X;
        output.y = TANK_GOOD_5_Y;
        break;
      case TANK_GOOD_6:
        output.x = TANK_GOOD_6_X;
        output.y = TANK_GOOD_6_Y;
        break;
      case TANK_GOOD_7:
        output.x = TANK_GOOD_7_X;
        output.y = TANK_GOOD_7_Y;
        break;
      case TANK_GOOD_8:
        output.x = TANK_GOOD_8_X;
        output.y = TANK_GOOD_8_Y;
        break;
      case TANK_GOOD_9:
        output.x = TANK_GOOD_9_X;
        output.y = TANK_GOOD_9_Y;
        break;
      case TANK_GOOD_10:
        output.x = TANK_GOOD_10_X;
        output.y = TANK_GOOD_10_Y;
        break;
      case TANK_GOOD_11:
        output.x = TANK_GOOD_11_X;
        output.y = TANK_GOOD_11_Y;
        break;
      case TANK_GOOD_12:
        output.x = TANK_GOOD_12_X;
        output.y = TANK_GOOD_12_Y;
        break;
      case TANK_GOOD_13:
        output.x = TANK_GOOD_13_X;
        output.y = TANK_GOOD_13_Y;
        break;
      case TANK_GOOD_14:
        output.x = TANK_GOOD_14_X;
        output.y = TANK_GOOD_14_Y;
        break;
      case TANK_GOOD_15:
        output.x = TANK_GOOD_15_X;
        output.y = TANK_GOOD_15_Y;
        break;
      case TANK_GOODBOAT_0:
        output.x = TANK_GOODBOAT_0_X;
        output.y = TANK_GOODBOAT_0_Y;
        break;
      case TANK_GOODBOAT_1:
        output.x = TANK_GOODBOAT_1_X;
        output.y = TANK_GOODBOAT_1_Y;
        break;
      case TANK_GOODBOAT_2:
        output.x = TANK_GOODBOAT_2_X;
        output.y = TANK_GOODBOAT_2_Y;
        break;
      case TANK_GOODBOAT_3:
        output.x = TANK_GOODBOAT_3_X;
        output.y = TANK_GOODBOAT_3_Y;
        break;
      case TANK_GOODBOAT_4:
        output.x = TANK_GOODBOAT_4_X;
        output.y = TANK_GOODBOAT_4_Y;
        break;
      case TANK_GOODBOAT_5:
        output.x = TANK_GOODBOAT_5_X;
        output.y = TANK_GOODBOAT_5_Y;
        break;
      case TANK_GOODBOAT_6:
        output.x = TANK_GOODBOAT_6_X;
        output.y = TANK_GOODBOAT_6_Y;
        break;
      case TANK_GOODBOAT_7:
        output.x = TANK_GOODBOAT_7_X;
        output.y = TANK_GOODBOAT_7_Y;
        break;
      case TANK_GOODBOAT_8:
        output.x = TANK_GOODBOAT_8_X;
        output.y = TANK_GOODBOAT_8_Y;
        break;
      case TANK_GOODBOAT_9:
        output.x = TANK_GOODBOAT_9_X;
        output.y = TANK_GOODBOAT_9_Y;
        break;
      case TANK_GOODBOAT_10:
        output.x = TANK_GOODBOAT_10_X;
        output.y = TANK_GOODBOAT_10_Y;
        break;
      case TANK_GOODBOAT_11:
        output.x = TANK_GOODBOAT_11_X;
        output.y = TANK_GOODBOAT_11_Y;
        break;
      case TANK_GOODBOAT_12:
        output.x = TANK_GOODBOAT_12_X;
        output.y = TANK_GOODBOAT_12_Y;
        break;
      case TANK_GOODBOAT_13:
        output.x = TANK_GOODBOAT_13_X;
        output.y = TANK_GOODBOAT_13_Y;
        break;
      case TANK_GOODBOAT_14:
        output.x = TANK_GOODBOAT_14_X;
        output.y = TANK_GOODBOAT_14_Y;
        break;
      case TANK_GOODBOAT_15:
        output.x = TANK_GOODBOAT_15_X;
        output.y = TANK_GOODBOAT_15_Y;
        break;
      case TANK_EVIL_0:
        output.x = TANK_EVIL_0_X;
        output.y = TANK_EVIL_0_Y;
        break;
      case TANK_EVIL_1:
        output.x = TANK_EVIL_1_X;
        output.y = TANK_EVIL_1_Y;
        break;
      case TANK_EVIL_2:
        output.x = TANK_EVIL_2_X;
        output.y = TANK_EVIL_2_Y;
        break;
      case TANK_EVIL_3:
        output.x = TANK_EVIL_3_X;
        output.y = TANK_EVIL_3_Y;
        break;
      case TANK_EVIL_4:
        output.x = TANK_EVIL_4_X;
        output.y = TANK_EVIL_4_Y;
        break;
      case TANK_EVIL_5:
        output.x = TANK_EVIL_5_X;
        output.y = TANK_EVIL_5_Y;
        break;
      case TANK_EVIL_6:
        output.x = TANK_EVIL_6_X;
        output.y = TANK_EVIL_6_Y;
        break;
      case TANK_EVIL_7:
        output.x = TANK_EVIL_7_X;
        output.y = TANK_EVIL_7_Y;
        break;
      case TANK_EVIL_8:
        output.x = TANK_EVIL_8_X;
        output.y = TANK_EVIL_8_Y;
        break;
      case TANK_EVIL_9:
        output.x = TANK_EVIL_9_X;
        output.y = TANK_EVIL_9_Y;
        break;
      case TANK_EVIL_10:
        output.x = TANK_EVIL_10_X;
        output.y = TANK_EVIL_10_Y;
        break;
      case TANK_EVIL_11:
        output.x = TANK_EVIL_11_X;
        output.y = TANK_EVIL_11_Y;
        break;
      case TANK_EVIL_12:
        output.x = TANK_EVIL_12_X;
        output.y = TANK_EVIL_12_Y;
        break;
      case TANK_EVIL_13:
        output.x = TANK_EVIL_13_X;
        output.y = TANK_EVIL_13_Y;
        break;
      case TANK_EVIL_14:
        output.x = TANK_EVIL_14_X;
        output.y = TANK_EVIL_14_Y;
        break;
      case TANK_EVIL_15:
        output.x = TANK_EVIL_15_X;
        output.y = TANK_EVIL_15_Y;
        break;
      case TANK_EVILBOAT_0:
        output.x = TANK_EVILBOAT_0_X;
        output.y = TANK_EVILBOAT_0_Y;
        break;
      case TANK_EVILBOAT_1:
        output.x = TANK_EVILBOAT_1_X;
        output.y = TANK_EVILBOAT_1_Y;
        break;
      case TANK_EVILBOAT_2:
        output.x = TANK_EVILBOAT_2_X;
        output.y = TANK_EVILBOAT_2_Y;
        break;
      case TANK_EVILBOAT_3:
        output.x = TANK_EVILBOAT_3_X;
        output.y = TANK_EVILBOAT_3_Y;
        break;
      case TANK_EVILBOAT_4:
        output.x = TANK_EVILBOAT_4_X;
        output.y = TANK_EVILBOAT_4_Y;
        break;
      case TANK_EVILBOAT_5:
        output.x = TANK_EVILBOAT_5_X;
        output.y = TANK_EVILBOAT_5_Y;
        break;
      case TANK_EVILBOAT_6:
        output.x = TANK_EVILBOAT_6_X;
        output.y = TANK_EVILBOAT_6_Y;
        break;
      case TANK_EVILBOAT_7:
        output.x = TANK_EVILBOAT_7_X;
        output.y = TANK_EVILBOAT_7_Y;
        break;
      case TANK_EVILBOAT_8:
        output.x = TANK_EVILBOAT_8_X;
        output.y = TANK_EVILBOAT_8_Y;
        break;
      case TANK_EVILBOAT_9:
        output.x = TANK_EVILBOAT_9_X;
        output.y = TANK_EVILBOAT_9_Y;
        break;
      case TANK_EVILBOAT_10:
        output.x = TANK_EVILBOAT_10_X;
        output.y = TANK_EVILBOAT_10_Y;
        break;
      case TANK_EVILBOAT_11:
        output.x = TANK_EVILBOAT_11_X;
        output.y = TANK_EVILBOAT_11_Y;
        break;
      case TANK_EVILBOAT_12:
        output.x = TANK_EVILBOAT_12_X;
        output.y = TANK_EVILBOAT_12_Y;
        break;
      case TANK_EVILBOAT_13:
        output.x = TANK_EVILBOAT_13_X;
        output.y = TANK_EVILBOAT_13_Y;
        break;
      case TANK_EVILBOAT_14:
        output.x = TANK_EVILBOAT_14_X;
        output.y = TANK_EVILBOAT_14_Y;
        break;
      case TANK_EVILBOAT_15:
        output.x = TANK_EVILBOAT_15_X;
        output.y = TANK_EVILBOAT_15_Y;
        break;

      default:
        /* TANK_TRANSPARENT */
        output.x = TANK_TRANSPARENT_X;
        output.y = TANK_TRANSPARENT_Y;
    }

    /* Output */
    output.x *= 1;
    output.y *= 1;
    dest.x = tank.pos.x * TILE_SIZE_X + tank.px;
    dest.y = tank.pos.y * TILE_SIZE_Y + tank.py;
    SDL_BlitSurface(lpTiles, &output, lpBackBuffer, &dest);
    /* Output the label */
    drawTankLabel(tank.playerName.c_str(), tank.pos.x, tank.pos.y, tank.px + 2,
                  tank.py + 2);
  }
}

/*********************************************************
 *NAME:          drawLgms
 *AUTHOR:        John Morrison
 *CREATION DATE: 17/1/99
 *LAST MODIFIED: 17/1/99
 *PURPOSE:
 *  Draws the builder
 *
 *ARGUMENTS:
 *  lgms - The screenLgm data structure
 *********************************************************/
void drawLGMs(const bolo::ScreenLgmList &lgms) {
  SDL_Rect output; /* Source Rectangle */

  for (const auto &lgm : lgms.lgms_) {
    switch (lgm.frame) {
      case LGM0:
        output.x = LGM0_X;
        output.y = LGM0_Y;
        output.w = LGM_WIDTH;
        output.h = LGM_HEIGHT;
        break;
      case LGM1:
        output.x = LGM1_X;
        output.y = LGM1_Y;
        output.w = LGM_WIDTH;
        output.h = LGM_HEIGHT;
        break;
      case LGM2:
        output.x = LGM2_X;
        output.y = LGM2_Y;
        output.w = LGM_WIDTH;
        output.h = LGM_HEIGHT;
        break;
      default:
        /* LGM3 - Helicopter */
        output.x = LGM_HELICOPTER_X;
        output.y = LGM_HELICOPTER_Y;
        output.w = TILE_SIZE_X;
        output.h = TILE_SIZE_Y;
        break;
    }
    SDL_Rect dest{.x = static_cast<Sint16>((lgm.pos.x * TILE_SIZE_X) + lgm.px),
                  .y = static_cast<Sint16>((lgm.pos.y * TILE_SIZE_Y) + lgm.py),
                  .w = output.w,
                  .h = output.h};
    SDL_BlitSurface(lpTiles, &output, lpBackBuffer, &dest);
  }
}

/*********************************************************
 *NAME:          drawPillInView
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/2/98
 *LAST MODIFIED: 29/4/00
 *PURPOSE:
 *  Draws the "Pillbox View" label
 *
 *ARGUMENTS:
 *
 *********************************************************/
void drawNetFailed() {
  SDL_Surface *lpTextSurface;

  lpTextSurface = TTF_RenderText_Shaded(lpFont, "Network Failed -  Resyncing",
                                        white, black);
  SDL_SetColorKey(lpTextSurface, SDL_SRCCOLORKEY,
                  SDL_MapRGB(lpTextSurface->format, 0, 0, 0));
  SDL_Rect dest{.x = 3 * TILE_SIZE_X,
                .y = 8 * TILE_SIZE_Y,
                .w = static_cast<Uint16>(lpTextSurface->w),
                .h = static_cast<Uint16>(lpTextSurface->h)};
  /* Output it */
  SDL_BlitSurface(lpTextSurface, nullptr, lpBackBuffer, &dest);
  SDL_FreeSurface(lpTextSurface);
}

/*********************************************************
 *NAME:          drawPillInView
 *AUTHOR:        John Morrison
 *CREATION DATE:  3/2/98
 *LAST MODIFIED: 29/4/00
 *PURPOSE:
 *  Draws the "Pillbox View" label
 *
 *ARGUMENTS:
 *
 *********************************************************/
void drawPillInView() {
  SDL_Surface *lpTextSurface;

  lpTextSurface = TTF_RenderText_Shaded(
      lpFont, langGetText(STR_DRAW_PILLBOXVIEW), white, black);
  SDL_SetColorKey(lpTextSurface, SDL_SRCCOLORKEY,
                  SDL_MapRGB(lpTextSurface->format, 0, 0, 0));
  SDL_Rect dest{.x = TILE_SIZE_X,
                .y = MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y,
                .w = static_cast<Uint16>(lpTextSurface->w),
                .h = static_cast<Uint16>(lpTextSurface->h)};
  /* Output it */
  SDL_BlitSurface(lpTextSurface, nullptr, lpBackBuffer, &dest);
  SDL_FreeSurface(lpTextSurface);
}

/*********************************************************
 *NAME:          drawStartDelay
 *AUTHOR:        John Morrison
 *CREATION DATE: 29/1/98
 *LAST MODIFIED: 29/4/00
 *PURPOSE:
 *  The start delay is still counting down. Draw it here.
 *
 *ARGUMENTS:
 *  rcWindow - Window rectangle
 *  srtDelay - The start delay
 *********************************************************/
void drawStartDelay(long srtDelay) {
  char str[FILENAME_MAX];    /* Output String */
  char strNum[FILENAME_MAX]; /* Holds the start delay as a string */
  SDL_Surface *lpTextSurface;

  SDL_FillRect(lpBackBuffer, nullptr,
               SDL_MapRGB(lpBackBuffer->format, 0, 0, 0));
  /* Prepare the string */
  srtDelay /= GAME_NUMGAMETICKS_SEC; /* Convert ticks back to seconds */
  sprintf(strNum, "%ld", srtDelay);
  strcpy(str, langGetText(STR_DRAW_GAMESTARTSIN));
  strcat(str, strNum);
  lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
  if (lpTextSurface) {
    SDL_Rect src{.x = TILE_SIZE_X + 5,
                 .y = TILE_SIZE_Y + 5,
                 .w = static_cast<Uint16>(lpTextSurface->w),
                 .h = static_cast<Uint16>(lpTextSurface->h)};
    SDL_BlitSurface(lpTextSurface, nullptr, lpBackBuffer, &src);
    SDL_UpdateRect(lpBackBuffer, 0, 0, 0, 0);
    SDL_Rect in{.x = TILE_SIZE_X,
                .y = TILE_SIZE_Y,
                .w = MAIN_SCREEN_SIZE_X * TILE_SIZE_X,
                .h = MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y};
    src.x = MAIN_OFFSET_X;
    src.y = MAIN_OFFSET_Y;
    src.w = in.w;
    src.h = in.h;
    SDL_BlitSurface(lpBackBuffer, &in, lpScreen, &src);
    SDL_UpdateRect(lpScreen, src.x, src.y, src.w, src.h);
    SDL_FreeSurface(lpTextSurface);
  }
}

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
 *  mineView - Pointer to the screen mines structure
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
                    BYTE cursorTop) {
  SDL_Rect output;     /* Output Rectangle */
  SDL_Rect textOutput; /* Text Output Rect */
  bool done;           /* Finished Looping */
  BYTE x;              /* X and Y co-ordinates */
  BYTE y;
  int outputX; /* X and Y co-ordinates in the tile image */
  int outputY;
  BYTE pos;        /* Current position */
  char str[255];   /* Frame Rate Counting Stuff */
  DWORD time;
  bool isPill;   /* Is the square a pill */
  bool isBase;   /* Is the square a base */
  BYTE labelNum; /* Returns the label number */
  SDL_Rect in;
  SDL_Surface *lpTextSurface;

  x = 0;
  y = 0;
  done = FALSE;
  in.w = TILE_SIZE_X;
  in.h = TILE_SIZE_Y;
  output.w = TILE_SIZE_X;
  output.h = TILE_SIZE_Y;
  str[0] = '\0';
  if (srtDelay > 0) {
    /* Waiting for game to start. Draw coutdown */
    drawStartDelay(srtDelay);
    return;
  }
  while (done == FALSE) {
    pos = tiles[x][y].terrain;
    isPill = FALSE;
    isBase = FALSE;
    outputX = drawPosX[pos];
    outputY = drawPosY[pos];
    if (pos == PILL_EVIL_15 || pos == PILL_EVIL_14 || pos == PILL_EVIL_13 ||
        pos == PILL_EVIL_12 || pos == PILL_EVIL_11 || pos == PILL_EVIL_10 ||
        pos == PILL_EVIL_9 || pos == PILL_EVIL_8 || pos == PILL_EVIL_7 ||
        pos == PILL_EVIL_6 || pos == PILL_EVIL_5 || pos == PILL_EVIL_4 ||
        pos == PILL_EVIL_3 || pos == PILL_EVIL_2 || pos == PILL_EVIL_1 ||
        pos == PILL_EVIL_0) {
      isPill = TRUE;
    }
    if (pos == PILL_GOOD_15 || pos == PILL_GOOD_14 || pos == PILL_GOOD_13 ||
        pos == PILL_GOOD_12 || pos == PILL_GOOD_11 || pos == PILL_GOOD_10 ||
        pos == PILL_GOOD_9 || pos == PILL_GOOD_8 || pos == PILL_GOOD_7 ||
        pos == PILL_GOOD_6 || pos == PILL_GOOD_5 || pos == PILL_GOOD_4 ||
        pos == PILL_GOOD_3 || pos == PILL_GOOD_2 || pos == PILL_GOOD_1 ||
        pos == PILL_GOOD_0) {
      isPill = TRUE;
    }
    if (pos == BASE_GOOD || pos == BASE_NEUTRAL || pos == BASE_EVIL) {
      isBase = TRUE;
    }

    /* Drawing */

    /* Draw the map block */
    in.x = outputX;
    in.y = outputY;
    output.x = x * TILE_SIZE_X;
    output.y = y * TILE_SIZE_Y;
    SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);

    /* Draw Mines */
    if (tiles[x][y].has_mine) {
      in.x = MINE_X;
      in.y = MINE_Y;
      SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
    }

    /* Draw the pillNumber or base Number if required */
    if (isPill == TRUE && showPillLabels == TRUE) {
      labelNum = screenPillNumPos(x, y);
      sprintf(str, "%d", (labelNum - 1));
      lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
      textOutput.x = (x * TILE_SIZE_X) + LABEL_OFFSET_X;
      textOutput.y = (y * TILE_SIZE_Y);
      textOutput.w = lpTextSurface->w;
      textOutput.h = lpTextSurface->h;
      SDL_BlitSurface(lpTextSurface, nullptr, lpBackBuffer, &textOutput);
      SDL_FreeSurface(lpTextSurface);
    }

    if (isBase == TRUE && showBaseLabels == TRUE) {
      labelNum = screenBaseNumPos(x, y);
      sprintf(str, "%d", (labelNum - 1));
      lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
      textOutput.x = (x * TILE_SIZE_X) + LABEL_OFFSET_X;
      textOutput.y = (y * TILE_SIZE_Y);
      textOutput.w = lpTextSurface->w;
      textOutput.h = lpTextSurface->h;
      SDL_BlitSurface(lpTextSurface, nullptr, lpBackBuffer, &textOutput);
      SDL_FreeSurface(lpTextSurface);
    }

    /* Increment the variable */
    x++;
    if (x == MAIN_BACK_BUFFER_SIZE_X) {
      y++;
      x = 0;
      if (y == MAIN_BACK_BUFFER_SIZE_Y) {
        done = TRUE;
      }
    }
  }

  /* Draw Explosions if Required */
  drawShells(sBullets);

  /* Draw the tank */
  drawTanks(tks);
  drawLGMs(lgms);

  /* Draw Gunsight */
  if (gunsight.has_value()) {
    in.x = GUNSIGHT_X;
    in.w = TILE_SIZE_X;
    in.y = GUNSIGHT_Y;
    in.h = TILE_SIZE_Y;
    output.x = (gunsight->pos.x) * TILE_SIZE_X + (gunsight->pixelX);
    output.y = (gunsight->pos.y) * TILE_SIZE_Y + (gunsight->pixelY);
    output.w = TILE_SIZE_X;
    output.h = TILE_SIZE_Y;
    SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
  }

  /* Draw the Cursor Square if required */
  if (useCursor == TRUE) {
    in.x = MOUSE_SQUARE_X;
    in.w = TILE_SIZE_X;
    in.y = MOUSE_SQUARE_Y;
    in.h = TILE_SIZE_Y;
    output.x = cursorLeft * TILE_SIZE_X;
    output.y = cursorTop * TILE_SIZE_Y;
    output.w = TILE_SIZE_X;
    output.h = TILE_SIZE_X;
    SDL_BlitSurface(lpTiles, &in, lpBackBuffer, &output);
  }

  /* Copy the back buffer to the window */
  SDL_UpdateRect(lpBackBuffer, 0, 0, 0, 0);
  in.x = TILE_SIZE_X + edgeX;
  in.y = TILE_SIZE_Y + edgeY;
  in.w = MAIN_SCREEN_SIZE_X * TILE_SIZE_X;
  in.h = MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
  output.x = MAIN_OFFSET_X;
  output.y = MAIN_OFFSET_Y;
  output.w = in.w;
  output.h = in.h;

  if (isPillView == TRUE) {
    /* we are in pillbox view - Write text here */
    drawPillInView();
  }

  if (netGetStatus() == netFailed) {
    drawNetFailed();
  }

  SDL_BlitSurface(lpBackBuffer, &in, lpScreen, &output);
  SDL_UpdateRect(lpScreen, output.x, output.y, output.w, output.h);

  /* Frame rate counting stuff */
  g_dwFrameCount++;
  time = SDL_GetTicks() - g_dwFrameTime;
  if (time > 1000) {
    g_dwFrameTotal = g_dwFrameCount;
    sprintf(str, "%ld", g_dwFrameTotal);
    g_dwFrameTime = SDL_GetTicks();
    g_dwFrameCount = 0;
  }
}

/*********************************************************
 *NAME:          drawSetBasesStatusClear
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/1/98
 *LAST MODIFIED: 23/1/98
 *PURPOSE:
 *  Clears the bases status display.
 *
 *ARGUMENTS:
 *********************************************************/
void drawSetBasesStatusClear(void) {
  SDL_Rect src;    /* Used for copying the bases & pills icon in */
  SDL_Rect dest;   /* Used for copying the bases & pills icon in */

  src.x = BASE_GOOD_X;
  src.y = BASE_GOOD_Y;
  src.w = TILE_SIZE_X;
  src.h = TILE_SIZE_Y;
  dest.x = STATUS_BASES_MIDDLE_ICON_X;
  dest.y = STATUS_BASES_MIDDLE_ICON_Y;
  dest.w = TILE_SIZE_X;
  dest.h = TILE_SIZE_Y;
  SDL_FillRect(lpBasesStatus, nullptr,
               SDL_MapRGB(lpBasesStatus->format, 0, 0, 0));
  SDL_BlitSurface(lpTiles, &src, lpBasesStatus, &dest);
  SDL_UpdateRect(lpBasesStatus, 0, 0, 0, 0);
}

/*********************************************************
 * *NAME:          drawSetPillsStatusClear
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/1/98
 *LAST MODIFIED: 23/1/98
 *PURPOSE:
 *  Clears the pills status display.
 *
 *ARGUMENTS:
 *********************************************************/
void drawSetPillsStatusClear(void) {
  SDL_Rect src;    /* Used for copying the bases & pills icon in */
  SDL_Rect dest;   /* Used for copying the bases & pills icon in */

  src.x = PILL_GOOD15_X;
  src.y = PILL_GOOD15_Y;
  src.w = TILE_SIZE_X;
  src.h = TILE_SIZE_Y;
  dest.y = STATUS_PILLS_MIDDLE_ICON_Y;
  dest.x = STATUS_PILLS_MIDDLE_ICON_X;
  dest.w = TILE_SIZE_X;
  dest.h = TILE_SIZE_Y;
  SDL_FillRect(lpPillsStatus, nullptr,
               SDL_MapRGB(lpPillsStatus->format, 0, 0, 0));
  SDL_BlitSurface(lpTiles, &src, lpPillsStatus, &dest);
  SDL_UpdateRect(lpPillsStatus, 0, 0, 0, 0);
}

/*********************************************************
 *NAME:          drawSetTanksStatusClear
 *AUTHOR:        John Morrison
 *CREATION DATE: 14/2/98
 *LAST MODIFIED: 14/2/98
 *PURPOSE:
 *  Clears the tanks status display.
 *
 *ARGUMENTS:
 *********************************************************/
void drawSetTanksStatusClear(void) {
  SDL_Rect src;    /* Used for copying the bases & pills icon in */
  SDL_Rect dest;   /* Used for copying the bases & pills icon in */

  src.x = TANK_SELF_0_X;
  src.y = TANK_SELF_0_Y;
  src.w = TILE_SIZE_X;
  src.h = TILE_SIZE_Y;
  dest.y = STATUS_TANKS_MIDDLE_ICON_Y;
  dest.x = STATUS_TANKS_MIDDLE_ICON_X;
  dest.w = TILE_SIZE_X;
  dest.h = TILE_SIZE_Y;
  SDL_FillRect(lpTankStatus, nullptr,
               SDL_MapRGB(lpTankStatus->format, 0, 0, 0));
  SDL_BlitSurface(lpTiles, &src, lpTankStatus, &dest);
  SDL_UpdateRect(lpTankStatus, 0, 0, 0, 0);
}

/*********************************************************
 *NAME:          drawCopyBasesStatus
 *AUTHOR:        John Morrison
 *CREATION DATE: 23/1/98
 *LAST MODIFIED: 23/1/98
 *PURPOSE:
 *  Copys the Bases status on to the primary buffer
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *********************************************************/
void drawCopyBasesStatus() {
  SDL_Rect dest; /* Destination location */

  dest.x = STATUS_BASES_LEFT;
  dest.y = STATUS_BASES_TOP;
  dest.w = STATUS_BASES_WIDTH;
  dest.h = STATUS_BASES_HEIGHT;
  SDL_BlitSurface(lpBasesStatus, nullptr, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
}

/*********************************************************
 * *NAME:          drawCopyPillsStatus
 * *AUTHOR:        John Morrison
 * *CREATION DATE: 23/1/98
 * *LAST MODIFIED: 23/1/98
 * *PURPOSE:
 * *  Copys the pills status on to the primary buffer
 * *
 * *ARGUMENTS:
 * *  xValue  - The left position of the window
 * *  yValue  - The top position of the window
 * *********************************************************/
void drawCopyPillsStatus() {
  SDL_Rect dest; /* Destination location */

  dest.x = STATUS_PILLS_LEFT;
  dest.y = STATUS_PILLS_TOP;
  dest.w = STATUS_PILLS_WIDTH;
  dest.h = STATUS_PILLS_HEIGHT;
  SDL_BlitSurface(lpPillsStatus, nullptr, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
}

/*********************************************************
 *NAME:          drawCopyTanksStatus
 *AUTHOR:        John Morrison
 *CREATION DATE: 14/2/98
 *LAST MODIFIED: 14/2/98
 *PURPOSE:
 *  Copys the tanks status on to the primary buffer
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *********************************************************/
void drawCopyTanksStatus() {
  SDL_Rect dest; /* Destination location */

  dest.x = STATUS_TANKS_LEFT;
  dest.y = STATUS_TANKS_TOP;
  dest.w = STATUS_TANKS_WIDTH;
  dest.h = STATUS_TANKS_HEIGHT;
  SDL_BlitSurface(lpTankStatus, nullptr, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
}

/*********************************************************
 *NAME:          drawStatusBase
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 23/1/99
 *PURPOSE:
 *  Draws the base status for a particular base
 *
 *ARGUMENTS:
 *  baseNum - The base number to draw (1-16)
 *  ba      - The allience of the base
 *  labels  - Should the label be shown
 *********************************************************/
void drawStatusBase(BYTE baseNum, baseAlliance ba, bool labels) {
  SDL_Rect src;  /* The src square on the tile file to retrieve */
  SDL_Rect dest; /* The dest square to draw it */
  char str[3];   /* String to output if labels are on */

  str[0] = '\0';

  src.w = STATUS_ITEM_SIZE_X;
  src.h = STATUS_ITEM_SIZE_Y;

  /* Set the co-ords of the tile file to get */
  switch (ba) {
    case baseDead:
      src.x = STATUS_ITEM_DEAD_X;
      src.y = STATUS_ITEM_DEAD_Y;
      break;
    case baseNeutral:
      src.x = STATUS_BASE_NEUTRAL_X;
      src.y = STATUS_BASE_NEUTRAL_Y;
      break;
    case baseOwnGood:
      src.x = STATUS_BASE_GOOD_X;
      src.y = STATUS_BASE_GOOD_Y;
      break;
    case baseAllieGood:
      src.x = STATUS_BASE_ALLIEGOOD_X;
      src.y = STATUS_BASE_ALLIEGOOD_Y;
      break;
    default:
      /* Base Evil */
      src.x = STATUS_BASE_EVIL_X;
      src.y = STATUS_BASE_EVIL_Y;
      break;
  }
  /* Modify the offset to allow for the indents */
  switch (baseNum) {
    case BASE_1:
      dest.x = STATUS_BASE_1_X;
      dest.y = STATUS_BASE_1_Y;
      break;
    case BASE_2:
      dest.x = STATUS_BASE_2_X;
      dest.y = STATUS_BASE_2_Y;
      break;
    case BASE_3:
      dest.x = STATUS_BASE_3_X;
      dest.y = STATUS_BASE_3_Y;
      break;
    case BASE_4:
      dest.x = STATUS_BASE_4_X;
      dest.y = STATUS_BASE_4_Y;
      break;
    case BASE_5:
      dest.x = STATUS_BASE_5_X;
      dest.y = STATUS_BASE_5_Y;
      break;
    case BASE_6:
      dest.x = STATUS_BASE_6_X;
      dest.y = STATUS_BASE_6_Y;
      break;
    case BASE_7:
      dest.x = STATUS_BASE_7_X;
      dest.y = STATUS_BASE_7_Y;
      break;
    case BASE_8:
      dest.x = STATUS_BASE_8_X;
      dest.y = STATUS_BASE_8_Y;
      break;
    case BASE_9:
      dest.x = STATUS_BASE_9_X;
      dest.y = STATUS_BASE_9_Y;
      break;
    case BASE_10:
      dest.x = STATUS_BASE_10_X;
      dest.y = STATUS_BASE_10_Y;
      break;
    case BASE_11:
      dest.x = STATUS_BASE_11_X;
      dest.y = STATUS_BASE_11_Y;
      break;
    case BASE_12:
      dest.x = STATUS_BASE_12_X;
      dest.y = STATUS_BASE_12_Y;
      break;
    case BASE_13:
      dest.x = STATUS_BASE_13_X;
      dest.y = STATUS_BASE_13_Y;
      break;
    case BASE_14:
      dest.x = STATUS_BASE_14_X;
      dest.y = STATUS_BASE_14_Y;
      break;
    case BASE_15:
      dest.x = STATUS_BASE_15_X;
      dest.y = STATUS_BASE_15_Y;
      break;
    case BASE_16:
      dest.x = STATUS_BASE_16_X;
      dest.y = STATUS_BASE_16_Y;
  }

  dest.w = STATUS_ITEM_SIZE_X;
  dest.h = STATUS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpBasesStatus, &dest);
  if (labels == TRUE) {
    /* Must draw the label */
    sprintf(str, "%d", (baseNum - 1));
    /* FIXME    if
       (SUCCEEDED(lpDDSBasesStatus->lpVtbl->GetDC(lpDDSBasesStatus,&hDC))) {
          fontSelectTiny(hDC);
          SetBkColor(hDC, RGB(0,0,0));
          SetTextColor(hDC, RGB(255,255,255));
          DrawText(hDC, str, strlen(str), &dest, (DT_TOP | DT_NOCLIP));
          lpDDSBasesStatus->lpVtbl->ReleaseDC(lpDDSBasesStatus, &hDC);
        } */
  }
  SDL_UpdateRects(lpBasesStatus, 1, &dest);
  // gdk_threads_leave();
}

/*********************************************************
 *NAME:          drawStatusPillbox
 *AUTHOR:        John Morrison
 *CREATION DATE: 21/12/98
 *LAST MODIFIED: 23/1/99
 *PURPOSE:
 *  Draws the pillbox status for a particular pillbox
 *
 *ARGUMENTS:
 *  pillNum - The tank number to draw (1-16)
 *  pb      - The allience of the pillbox
 *  labels  - Should labels be drawn?
 *********************************************************/
void drawStatusPillbox(BYTE pillNum, pillAlliance pb, bool labels) {
  SDL_Rect src;  /* The src square on the tile file to retrieve */
  SDL_Rect dest; /* The dest square to draw it */
  char str[3];   /* String to output if labels are on */

  str[0] = '\0';
  src.w = STATUS_ITEM_SIZE_X;
  src.h = STATUS_ITEM_SIZE_Y;

  /* Set the co-ords of the tile file to get */
  switch (pb) {
    case pillDead:
      src.x = STATUS_ITEM_DEAD_X;
      src.y = STATUS_ITEM_DEAD_Y;
      break;
    case pillNeutral:
      src.x = STATUS_PILLBOX_NEUTRAL_X;
      src.y = STATUS_PILLBOX_NEUTRAL_Y;
      break;
    case pillGood:
      src.x = STATUS_PILLBOX_GOOD_X;
      src.y = STATUS_PILLBOX_GOOD_Y;
      break;
    case pillAllie:
      src.x = STATUS_PILLBOX_ALLIEGOOD_X;
      src.y = STATUS_PILLBOX_ALLIEGOOD_Y;
      break;
    case pillTankGood:
      src.x = STATUS_PILLBOX_TANKGOOD_X;
      src.y = STATUS_PILLBOX_TANKGOOD_Y;
      break;
    case pillTankAllie:
      src.x = 272;  // STATUS_PILLBOX_TANKALLIE_X;
      src.y = 144;  // STATUS_PILLBOX_TANKALLIE_Y +5 ;
      break;
    case pillTankEvil:
      src.x = STATUS_PILLBOX_TANKEVIL_X;
      src.y = STATUS_PILLBOX_TANKEVIL_Y;
      break;
    default:
      /* PILLBOX Evil */
      src.x = STATUS_PILLBOX_EVIL_X;
      src.y = STATUS_PILLBOX_EVIL_Y;
      break;
  }
  /* Modify the offset to allow for the indents */
  switch (pillNum) {
    case PILLBOX_1:
      dest.x = STATUS_PILLBOX_1_X;
      dest.y = STATUS_PILLBOX_1_Y;
      break;
    case PILLBOX_2:
      dest.x = STATUS_PILLBOX_2_X;
      dest.y = STATUS_PILLBOX_2_Y;
      break;
    case PILLBOX_3:
      dest.x = STATUS_PILLBOX_3_X;
      dest.y = STATUS_PILLBOX_3_Y;
      break;
    case PILLBOX_4:
      dest.x = STATUS_PILLBOX_4_X;
      dest.y = STATUS_PILLBOX_4_Y;
      break;
    case PILLBOX_5:
      dest.x = STATUS_PILLBOX_5_X;
      dest.y = STATUS_PILLBOX_5_Y;
      break;
    case PILLBOX_6:
      dest.x = STATUS_PILLBOX_6_X;
      dest.y = STATUS_PILLBOX_6_Y;
      break;
    case PILLBOX_7:
      dest.x = STATUS_PILLBOX_7_X;
      dest.y = STATUS_PILLBOX_7_Y;
      break;
    case PILLBOX_8:
      dest.x = STATUS_PILLBOX_8_X;
      dest.y = STATUS_PILLBOX_8_Y;
      break;
    case PILLBOX_9:
      dest.x = STATUS_PILLBOX_9_X;
      dest.y = STATUS_PILLBOX_9_Y;
      break;
    case PILLBOX_10:
      dest.x = STATUS_PILLBOX_10_X;
      dest.y = STATUS_PILLBOX_10_Y;
      break;
    case PILLBOX_11:
      dest.x = STATUS_PILLBOX_11_X;
      dest.y = STATUS_PILLBOX_11_Y;
      break;
    case PILLBOX_12:
      dest.x = STATUS_PILLBOX_12_X;
      dest.y = STATUS_PILLBOX_12_Y;
      break;
    case PILLBOX_13:
      dest.x = STATUS_PILLBOX_13_X;
      dest.y = STATUS_PILLBOX_13_Y;
      break;
    case PILLBOX_14:
      dest.x = STATUS_PILLBOX_14_X;
      dest.y = STATUS_PILLBOX_14_Y;
      break;
    case PILLBOX_15:
      dest.x = STATUS_PILLBOX_15_X;
      dest.y = STATUS_PILLBOX_15_Y;
      break;
    case PILLBOX_16:
      dest.x = STATUS_PILLBOX_16_X;
      dest.y = STATUS_PILLBOX_16_Y;
  }

  dest.w = STATUS_ITEM_SIZE_X;
  dest.h = STATUS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpPillsStatus, &dest);
  if (labels == TRUE) {
    /* Must draw the label */
    sprintf(str, "%d", (pillNum - 1));
    /* FIXME:    if
       (SUCCEEDED(lpDDSPillsStatus->lpVtbl->GetDC(lpDDSPillsStatus,&hDC))) {
          fontSelectTiny(hDC);
          SetBkColor(hDC, RGB(0,0,0));
          SetTextColor(hDC, RGB(255,255,255));
          DrawText(hDC, str, strlen(str), &dest, (DT_TOP | DT_NOCLIP));
          lpDDSPillsStatus->lpVtbl->ReleaseDC(lpDDSPillsStatus,&hDC);
        } */
  }
  SDL_UpdateRects(lpPillsStatus, 1, &dest);
}

/*********************************************************
 *NAME:          drawStatusTank
 *AUTHOR:        John Morrison
 *CREATION DATE: 14/2/99
 *LAST MODIFIED: 14/2/99
 *PURPOSE:
 *  Draws the tank status for a particular tank
 *
 *ARGUMENTS:
 *  tankNum - The tank number to draw (1-16)
 *  ta      - The allience of the pillbox
 *********************************************************/
void drawStatusTank(BYTE tankNum, bolo::tankAlliance ta) {
  SDL_Rect src;  /* The src square on the tile file to retrieve */
  SDL_Rect dest; /* The dest square to draw it */

  src.w = STATUS_ITEM_SIZE_X;
  src.h = STATUS_ITEM_SIZE_Y;

  /* Set the co-ords of the tile file to get */
  switch (ta) {
    case bolo::tankAlliance::tankNone:
      src.x = STATUS_TANK_NONE_X;
      src.y = STATUS_TANK_NONE_Y;
      break;
    case bolo::tankAlliance::tankSelf:
      src.x = STATUS_TANK_SELF_X;
      src.y = STATUS_TANK_SELF_Y;
      break;
    case bolo::tankAlliance::tankAllie:
      src.x = STATUS_TANK_GOOD_X;
      src.y = STATUS_TANK_GOOD_Y;
      break;
    case bolo::tankAlliance::tankEvil:
      src.x = STATUS_TANK_EVIL_X;
      src.y = STATUS_TANK_EVIL_Y;
      break;
  }
  /* Modify the offset to allow for the indents */
  switch (tankNum) {
    case TANK_1:
      dest.x = STATUS_TANKS_1_X;
      dest.y = STATUS_TANKS_1_Y;
      break;
    case TANK_2:
      dest.x = STATUS_TANKS_2_X;
      dest.y = STATUS_TANKS_2_Y;
      break;
    case TANK_3:
      dest.x = STATUS_TANKS_3_X;
      dest.y = STATUS_TANKS_3_Y;
      break;
    case TANK_4:
      dest.x = STATUS_TANKS_4_X;
      dest.y = STATUS_TANKS_4_Y;
      break;
    case TANK_5:
      dest.x = STATUS_TANKS_5_X;
      dest.y = STATUS_TANKS_5_Y;
      break;
    case TANK_6:
      dest.x = STATUS_TANKS_6_X;
      dest.y = STATUS_TANKS_6_Y;
      break;
    case TANK_7:
      dest.x = STATUS_TANKS_7_X;
      dest.y = STATUS_TANKS_7_Y;
      break;
    case TANK_8:
      dest.x = STATUS_TANKS_8_X;
      dest.y = STATUS_TANKS_8_Y;
      break;
    case TANK_9:
      dest.x = STATUS_TANKS_9_X;
      dest.y = STATUS_TANKS_9_Y;
      break;
    case TANK_10:
      dest.x = STATUS_TANKS_10_X;
      dest.y = STATUS_TANKS_10_Y;
      break;
    case TANK_11:
      dest.x = STATUS_TANKS_11_X;
      dest.y = STATUS_TANKS_11_Y;
      break;
    case TANK_12:
      dest.x = STATUS_TANKS_12_X;
      dest.y = STATUS_TANKS_12_Y;
      break;
    case TANK_13:
      dest.x = STATUS_TANKS_13_X;
      dest.y = STATUS_TANKS_13_Y;
      break;
    case TANK_14:
      dest.x = STATUS_TANKS_14_X;
      dest.y = STATUS_TANKS_14_Y;
      break;
    case TANK_15:
      dest.x = STATUS_TANKS_15_X;
      dest.y = STATUS_TANKS_15_Y;
      break;
    case TANK_16:
      dest.x = STATUS_TANKS_16_X;
      dest.y = STATUS_TANKS_16_Y;
  }

  dest.w = STATUS_ITEM_SIZE_X;
  dest.h = STATUS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpTankStatus, &dest);
  SDL_UpdateRects(lpTankStatus, 1, &dest);
}

/*********************************************************
 *NAME:          drawStatusTankBars
 *AUTHOR:        John Morrison
 *CREATION DATE: 22/12/98
 *LAST MODIFIED: 22/12/98
 *PURPOSE:
 *  Draws the tanks armour, shells etc bars.
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *  shells  - Number of shells
 *  mines   - Number of mines
 *  armour  - Amount of armour
 *  trees   - Amount of trees
 *********************************************************/
void drawStatusTankBars(int xValue, int yValue, BYTE shells, BYTE mines,
                        BYTE armour, BYTE trees) {
  SDL_Rect dest; /* The dest square to draw it */
  SDL_Rect fill;
  Uint32 color; /* Fill green colour */

  dest.w = STATUS_TANK_BARS_WIDTH;
  color = SDL_MapRGB(lpScreen->format, 0, 0xFF, 0);

  /* Make the area black first */
  fill.y = yValue + STATUS_TANK_BARS_TOP + STATUS_TANK_BARS_HEIGHT -
           (BAR_TANK_MULTIPLY * 40);
  fill.h = yValue + STATUS_TANK_BARS_TOP + STATUS_TANK_BARS_HEIGHT - fill.y;
  fill.x = xValue + STATUS_TANK_SHELLS;
  fill.w = xValue + STATUS_TANK_TREES + STATUS_TANK_BARS_WIDTH - fill.x;
  SDL_FillRect(lpScreen, &fill, SDL_MapRGB(lpScreen->format, 0, 0, 0));

  /* Shells */
  dest.y = yValue + STATUS_TANK_BARS_TOP + STATUS_TANK_BARS_HEIGHT -
           (BAR_TANK_MULTIPLY * shells);
  dest.x = xValue + STATUS_TANK_SHELLS;
  dest.h = BAR_TANK_MULTIPLY * shells;
  SDL_FillRect(lpScreen, &dest, color);

  /* Mines */
  dest.y = yValue + STATUS_TANK_BARS_TOP + STATUS_TANK_BARS_HEIGHT -
           (BAR_TANK_MULTIPLY * mines);
  dest.x = xValue + STATUS_TANK_MINES;
  dest.h = BAR_TANK_MULTIPLY * mines;
  SDL_FillRect(lpScreen, &dest, color);

  /* Armour */
  dest.y = yValue + STATUS_TANK_BARS_TOP + STATUS_TANK_BARS_HEIGHT -
           (BAR_TANK_MULTIPLY * armour);
  dest.x = xValue + STATUS_TANK_ARMOUR;
  dest.h = BAR_TANK_MULTIPLY * armour;
  SDL_FillRect(lpScreen, &dest, color);

  /* Trees */
  dest.y = yValue + STATUS_TANK_BARS_TOP + STATUS_TANK_BARS_HEIGHT -
           (BAR_TANK_MULTIPLY * trees);
  dest.x = xValue + STATUS_TANK_TREES;
  dest.h = BAR_TANK_MULTIPLY * trees;
  SDL_FillRect(lpScreen, &dest, color);

  SDL_UpdateRects(lpScreen, 1, &fill);
}
/*********************************************************
 *NAME:          drawStatusBaseBars
 *AUTHOR:        John Morrison
 *CREATION DATE: 11/1/98
 *LAST MODIFIED: 11/1/98
 *PURPOSE:
 *  Draws the base armour, shells etc bars.
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *  shells  - Number of shells
 *  mines   - Number of mines
 *  armour  - Amount of armour
 *  redraw  - If set to true use the redraw last amounts
 *********************************************************/
void drawStatusBaseBars(int xValue, int yValue, BYTE shells, BYTE mines,
                        BYTE armour, bool redraw) {
  SDL_Rect dest; /* The dest square to draw it */
  SDL_Rect fill;
  static BYTE lastShells =
      0; /* Last amount of stuff to save on rendering and flicker */
  static BYTE lastMines = 0;
  static BYTE lastArmour = 0;
  Uint32 color; /* Fill green colour */

  if (lastShells != shells || lastMines != mines || lastArmour != armour ||
      redraw == TRUE) {
    if (redraw == FALSE) {
      lastShells = shells;
      lastMines = mines;
      lastArmour = armour;
    } else {
      shells = lastShells;
      mines = lastMines;
      armour = lastArmour;
    }
    /* Make the area black first */
    fill.y = yValue + STATUS_BASE_SHELLS;
    fill.x = xValue + STATUS_BASE_BARS_LEFT;
    fill.h = yValue + STATUS_BASE_MINES + STATUS_BASE_BARS_HEIGHT - fill.y;
    fill.w = STATUS_BASE_BARS_MAX_WIDTH;
    SDL_FillRect(lpScreen, &fill, SDL_MapRGB(lpScreen->format, 0, 0, 0));
    if (shells != 0 || mines != 0 || armour != 0) {
      color = SDL_MapRGB(lpScreen->format, 0, 0xFF, 0);
      dest.x = xValue + STATUS_BASE_BARS_LEFT;
      dest.h = STATUS_BASE_BARS_HEIGHT;
      /* Shells */
      dest.y = yValue + STATUS_BASE_SHELLS;
      dest.w = shells * BAR_BASE_MULTIPLY;
      SDL_FillRect(lpScreen, &dest, color);
      /* Mines */
      dest.y = yValue + STATUS_BASE_MINES;
      dest.w = mines * BAR_BASE_MULTIPLY;
      SDL_FillRect(lpScreen, &dest, color);
      /* Armour */
      dest.y = yValue + STATUS_BASE_ARMOUR;
      dest.w = armour * BAR_BASE_MULTIPLY;
      SDL_FillRect(lpScreen, &dest, color);
    }
    SDL_UpdateRects(lpScreen, 1, &fill);
  }
}

/*********************************************************
 *NAME:          drawSelectIndentsOn
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/12/98
 *LAST MODIFIED: 20/12/98
 *PURPOSE:
 *  Draws the indents around the five building selection
 *  graphics on the left based on the buildSelect value.
 *  Draws the red dot as well.
 *
 *ARGUMENTS:
 *  value   - The currently selected build icon
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *********************************************************/
void drawSelectIndentsOn(buildSelect value, int xValue, int yValue) {
  SDL_Rect src;    /* The src square on the tile file to retrieve */
  SDL_Rect dest;   /* The dest square to draw it */

  /* Set the co-ords of the tile file to get */
  src.x = INDENT_ON_X;
  src.y = INDENT_ON_Y;
  src.w = BS_ITEM_SIZE_X;
  src.h = BS_ITEM_SIZE_Y;

  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
    case BsTrees:
      dest.x += BS_TREE_OFFSET_X;
      dest.y += BS_TREE_OFFSET_Y;
      break;
    case BsRoad:
      dest.x += BS_ROAD_OFFSET_X;
      dest.y += BS_ROAD_OFFSET_Y;
      break;
    case BsBuilding:
      dest.x += BS_BUILDING_OFFSET_X;
      dest.y += BS_BUILDING_OFFSET_Y;
      break;
    case BsPillbox:
      dest.x += BS_PILLBOX_OFFSET_X;
      dest.y += BS_PILLBOX_OFFSET_Y;
      break;
    default:
      /* BsMine:*/
      dest.x += BS_MINE_OFFSET_X;
      dest.y += BS_MINE_OFFSET_Y;
      break;
  }
  dest.w = BS_ITEM_SIZE_X;
  dest.h = BS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);

  /* Set the co-ords of the tile file to get */
  src.x = INDENT_DOT_ON_X;
  src.y = INDENT_DOT_ON_Y;
  src.w = BS_DOT_ITEM_SIZE_X;
  src.h = BS_DOT_ITEM_SIZE_Y;

  /* Draw the dot */
  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
    case BsTrees:
      dest.x += BS_DOT_TREE_OFFSET_X;
      dest.y += BS_DOT_TREE_OFFSET_Y;
      break;
    case BsRoad:
      dest.x += BS_DOT_ROAD_OFFSET_X;
      dest.y += BS_DOT_ROAD_OFFSET_Y;
      break;
    case BsBuilding:
      dest.x += BS_DOT_BUILDING_OFFSET_X;
      dest.y += BS_DOT_BUILDING_OFFSET_Y;
      break;
    case BsPillbox:
      dest.x += BS_DOT_PILLBOX_OFFSET_X;
      dest.y += BS_DOT_PILLBOX_OFFSET_Y;
      break;
    default:
      /* BsMine:*/
      dest.x += BS_DOT_MINE_OFFSET_X;
      dest.y += BS_DOT_MINE_OFFSET_Y;
      break;
  }

  dest.w = BS_DOT_ITEM_SIZE_X;
  dest.h = BS_DOT_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
}

/*********************************************************
 *NAME:          drawSelectIndentsOff
 *AUTHOR:        John Morrison
 *CREATION DATE: 20/12/98
 *LAST MODIFIED: 20/12/98
 *PURPOSE:
 *  Draws the indents around the five building selection
 *  graphics off the left based on the buildSelect value.
 *  Draws the red dot as well.
 *
 *ARGUMENTS:
 *  value   - The currently selected build icon
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *********************************************************/
void drawSelectIndentsOff(buildSelect value, int xValue, int yValue) {
  SDL_Rect src;    /* The src square on the tile file to retrieve */
  SDL_Rect dest;   /* The dest square to draw it */

  /* Set the co-ords of the tile file to get */
  src.x = INDENT_OFF_X;
  src.y = INDENT_OFF_Y;
  src.w = BS_ITEM_SIZE_X;
  src.h = BS_ITEM_SIZE_Y;

  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
    case BsTrees:
      dest.x += BS_TREE_OFFSET_X;
      dest.y += BS_TREE_OFFSET_Y;
      break;
    case BsRoad:
      dest.x += BS_ROAD_OFFSET_X;
      dest.y += BS_ROAD_OFFSET_Y;
      break;
    case BsBuilding:
      dest.x += BS_BUILDING_OFFSET_X;
      dest.y += BS_BUILDING_OFFSET_Y;
      break;
    case BsPillbox:
      dest.x += BS_PILLBOX_OFFSET_X;
      dest.y += BS_PILLBOX_OFFSET_Y;
      break;
    default:
      /* BsMine:*/
      dest.x += BS_MINE_OFFSET_X;
      dest.y += BS_MINE_OFFSET_Y;
      break;
  }
  dest.w = BS_ITEM_SIZE_X;
  dest.h = BS_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);

  /* Set the co-ords of the tile file to get */
  src.x = INDENT_DOT_OFF_X;
  src.y = INDENT_DOT_OFF_Y;
  src.w = BS_DOT_ITEM_SIZE_X;
  src.h = BS_DOT_ITEM_SIZE_Y;

  /* Draw the dot */
  /* Modify the offset to allow for the indents */
  dest.x = xValue;
  dest.y = yValue;
  switch (value) {
    case BsTrees:
      dest.x += BS_DOT_TREE_OFFSET_X;
      dest.y += BS_DOT_TREE_OFFSET_Y;
      break;
    case BsRoad:
      dest.x += BS_DOT_ROAD_OFFSET_X;
      dest.y += BS_DOT_ROAD_OFFSET_Y;
      break;
    case BsBuilding:
      dest.x += BS_DOT_BUILDING_OFFSET_X;
      dest.y += BS_DOT_BUILDING_OFFSET_Y;
      break;
    case BsPillbox:
      dest.x += BS_DOT_PILLBOX_OFFSET_X;
      dest.y += BS_DOT_PILLBOX_OFFSET_Y;
      break;
    default:
      /* BsMine:*/
      dest.x += BS_DOT_MINE_OFFSET_X;
      dest.y += BS_DOT_MINE_OFFSET_Y;
      break;
  }

  dest.w = BS_DOT_ITEM_SIZE_X;
  dest.h = BS_DOT_ITEM_SIZE_Y;

  /* Perform the drawing */
  SDL_BlitSurface(lpTiles, &src, lpScreen, &dest);
  SDL_UpdateRects(lpScreen, 1, &dest);
}

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
                   bool showPillsStatus, bool showBasesStatus) {
  BYTE total;  /* Total number of elements */
  BYTE count;  /* Looping Variable */
  BYTE shells; /* Tank amounts */
  BYTE mines;
  BYTE armour;
  BYTE trees;
  char top[MESSAGE_STRING_SIZE]; /* Message lines */
  char bottom[MESSAGE_STRING_SIZE];
  int kills; /* Number of kills and deaths the tank has */
  int deaths;
  /* LGM Status */
  bool lgmIsOut;
  bool lgmIsDead;
  TURNTYPE lgmAngle;

  lpScreen = SDL_SetVideoMode(width, height, 0, 0);
  drawBackground();
  drawSelectIndentsOn(value, 0, 0);
  drawSetBasesStatusClear();
  clientMutexWaitFor();
  total = screenNumBases();
  for (count = 1; count <= total; count++) {
    BYTE ba = screenBaseAlliance(count);
    drawStatusBase(count, (baseAlliance)ba, showBasesStatus);
  }
  clientMutexRelease();
  drawCopyBasesStatus();
  /* Draw Pillbox Status */
  clientMutexWaitFor();
  drawSetPillsStatusClear();
  total = screenNumPills();
  for (count = 1; count <= total; count++) {
    BYTE ba = screenPillAlliance(count);
    drawStatusPillbox(count, (pillAlliance)ba, showPillsStatus);
  }
  clientMutexRelease();
  drawCopyPillsStatus();

  /* Draw Tanks Status */
  drawSetTanksStatusClear();
  clientMutexWaitFor();
  total = screenGetNumPlayers();
  for (count = 1; count <= MAX_TANKS; count++) {
    bolo::tankAlliance ba = screenTankAlliance(count);
    drawStatusTank(count, ba);
  }
  clientMutexRelease();
  drawCopyTanksStatus();

  /* Draw tank Bars */
  clientMutexWaitFor();
  screenGetTankStats(&shells, &mines, &armour, &trees);
  drawStatusTankBars(0, 0, shells, mines, armour, trees);
  screenGetMessages(top, bottom);
  drawMessages(0, 0, top, bottom);
  screenGetKillsDeaths(&kills, &deaths);
  drawKillsDeaths(0, 0, kills, deaths);
  drawStatusBaseBars(0, 0, 0, 0, 0, TRUE);
  screenGetLgmStatus(&lgmIsOut, &lgmIsDead, &lgmAngle);
  if (lgmIsOut == TRUE) {
    drawSetManStatus(lgmIsDead, lgmAngle, FALSE);
  }
  clientMutexRelease();
}

/*********************************************************
 *NAME:          drawMessages
 *AUTHOR:        John Morrison
 *CREATION DATE: 3/1/99
 *LAST MODIFIED: 3/1/99
 *PURPOSE:
 *  Draws the message window
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *  top    - The top string to draw
 *  bottom - The bottom string to draw
 *********************************************************/
void drawMessages(int xValue, int yValue, const char *top, const char *bottom) {
  SDL_Surface *lpTextSurface;
  SDL_Rect dest; /* The dest square to draw it */

  lpTextSurface = TTF_RenderText_Shaded(lpFont, top, white, black);
  if (lpTextSurface) {
    dest.x = xValue + MESSAGE_LEFT;
    dest.y = yValue + MESSAGE_TOP;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, nullptr, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
  }
  lpTextSurface = TTF_RenderText_Shaded(lpFont, bottom, white, black);
  if (lpTextSurface) {
    dest.x = xValue + MESSAGE_LEFT;
    dest.y = yValue + MESSAGE_TOP + MESSAGE_TEXT_HEIGHT;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, nullptr, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
  }
}

/*********************************************************
 *NAME:          drawDownloadScreen
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/3/98
 *LAST MODIFIED: 13/12/98
 *PURPOSE:
 *  Draws the download line on the screen
 *
 *ARGUMENTS:
 *  rcWindow  - Window Co-ordinates
 *  justBlack - TRUE if we want a black screen
 *********************************************************/
void drawDownloadScreen(bool justBlack) {
  SDL_Rect output; /* Output RECT */
  SDL_Rect in;

  /* Fill the area black */
  SDL_FillRect(lpBackBuffer, nullptr,
               SDL_MapRGB(lpBackBuffer->format, 0, 0, 0));
  /* Fill the downloaded area white */
  if (justBlack == FALSE) {
    output.x = 0;
    output.y = 0;
    output.h = netGetDownloadPos();
    output.w = (MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y) + TILE_SIZE_Y;
    SDL_FillRect(lpBackBuffer, &output,
                 SDL_MapRGB(lpBackBuffer->format, 255, 255, 255));
  }

  /* Copy the back buffer to the window */
  SDL_UpdateRect(lpBackBuffer, 0, 0, 0, 0);
  in.x = TILE_SIZE_X + 2;
  in.y = TILE_SIZE_Y + 1;
  in.w = MAIN_SCREEN_SIZE_X * TILE_SIZE_X;
  in.h = MAIN_SCREEN_SIZE_Y * TILE_SIZE_Y;
  output.x = MAIN_OFFSET_X;
  output.y = MAIN_OFFSET_Y;
  output.w = in.w;
  output.h = in.h;

  SDL_BlitSurface(lpBackBuffer, &in, lpScreen, &output);
  SDL_UpdateRect(lpScreen, output.x, output.y, output.w, output.h);
}

/*********************************************************
 *NAME:          drawKillsDeaths
 *AUTHOR:        John Morrison
 *CREATION DATE:  8/1/99
 *LAST MODIFIED:  8/1/99
 *PURPOSE:
 *  Draws the tanks kills/deaths
 *
 *ARGUMENTS:
 *  xValue  - The left position of the window
 *  yValue  - The top position of the window
 *  kills  - The number of kills the tank has.
 *  deaths - The number of times the tank has died
 *********************************************************/
void drawKillsDeaths(int xValue, int yValue, int kills, int deaths) {
  SDL_Surface *lpTextSurface;
  SDL_Rect dest; /* The dest square to draw it */
  char str[16];  /* Holds the charectors to print */

  sprintf(str, "%d", kills);

  lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
  if (lpTextSurface) {
    dest.x = xValue + STATUS_KILLS_LEFT;
    dest.y = yValue + STATUS_KILLS_TOP + 1;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, nullptr, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
  }
  sprintf(str, "%d", deaths);
  lpTextSurface = TTF_RenderText_Shaded(lpFont, str, white, black);
  if (lpTextSurface) {
    dest.x = xValue + STATUS_DEATHS_LEFT;
    dest.y = yValue + STATUS_DEATHS_TOP + 1;
    dest.w = lpTextSurface->w;
    dest.h = lpTextSurface->h;
    SDL_BlitSurface(lpTextSurface, nullptr, lpScreen, &dest);
    SDL_UpdateRects(lpScreen, 1, &dest);
    SDL_FreeSurface(lpTextSurface);
  }
}

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
void drawSetupArrays() {
  drawPosX[DEEP_SEA_SOLID] = DEEP_SEA_SOLID_X;
  drawPosY[DEEP_SEA_SOLID] = DEEP_SEA_SOLID_Y;
  drawPosX[DEEP_SEA_CORN1] = DEEP_SEA_CORN1_X;
  drawPosY[DEEP_SEA_CORN1] = DEEP_SEA_CORN1_Y;
  drawPosX[DEEP_SEA_CORN2] = DEEP_SEA_CORN2_X;
  drawPosY[DEEP_SEA_CORN2] = DEEP_SEA_CORN2_Y;
  drawPosX[DEEP_SEA_CORN3] = DEEP_SEA_CORN3_X;
  drawPosY[DEEP_SEA_CORN3] = DEEP_SEA_CORN3_Y;
  drawPosX[DEEP_SEA_CORN4] = DEEP_SEA_CORN4_X;
  drawPosY[DEEP_SEA_CORN4] = DEEP_SEA_CORN4_Y;
  drawPosX[DEEP_SEA_SIDE1] = DEEP_SEA_SIDE1_X;
  drawPosY[DEEP_SEA_SIDE1] = DEEP_SEA_SIDE1_Y;
  drawPosX[DEEP_SEA_SIDE2] = DEEP_SEA_SIDE2_X;
  drawPosY[DEEP_SEA_SIDE2] = DEEP_SEA_SIDE2_Y;
  drawPosX[DEEP_SEA_SIDE3] = DEEP_SEA_SIDE3_X;
  drawPosY[DEEP_SEA_SIDE3] = DEEP_SEA_SIDE3_Y;
  drawPosX[DEEP_SEA_SIDE4] = DEEP_SEA_SIDE4_X;
  drawPosY[DEEP_SEA_SIDE4] = DEEP_SEA_SIDE4_Y;

  drawPosX[BUILD_SINGLE] = BUILD_SINGLE_X;
  drawPosY[BUILD_SINGLE] = BUILD_SINGLE_Y;
  drawPosX[BUILD_SOLID] = BUILD_SOLID_X;
  drawPosY[BUILD_SOLID] = BUILD_SOLID_Y;
  drawPosX[BUILD_CORNER1] = BUILD_CORNER1_X;
  drawPosY[BUILD_CORNER1] = BUILD_CORNER1_Y;
  drawPosX[BUILD_CORNER2] = BUILD_CORNER2_X;
  drawPosY[BUILD_CORNER2] = BUILD_CORNER2_Y;
  drawPosX[BUILD_CORNER3] = BUILD_CORNER3_X;
  drawPosY[BUILD_CORNER3] = BUILD_CORNER3_Y;
  drawPosX[BUILD_CORNER4] = BUILD_CORNER4_X;
  drawPosY[BUILD_CORNER4] = BUILD_CORNER4_Y;
  drawPosX[BUILD_L1] = BUILD_L1_X;
  drawPosY[BUILD_L1] = BUILD_L1_Y;
  drawPosX[BUILD_L2] = BUILD_L2_X;
  drawPosY[BUILD_L2] = BUILD_L2_Y;
  drawPosX[BUILD_L3] = BUILD_L3_X;
  drawPosY[BUILD_L3] = BUILD_L3_Y;
  drawPosX[BUILD_L4] = BUILD_L4_X;
  drawPosY[BUILD_L4] = BUILD_L4_Y;
  drawPosX[BUILD_T1] = BUILD_T1_X;
  drawPosY[BUILD_T1] = BUILD_T1_Y;
  drawPosX[BUILD_T2] = BUILD_T2_X;
  drawPosY[BUILD_T2] = BUILD_T2_Y;
  drawPosX[BUILD_T3] = BUILD_T3_X;
  drawPosY[BUILD_T3] = BUILD_T3_Y;
  drawPosX[BUILD_T4] = BUILD_T4_X;
  drawPosY[BUILD_T4] = BUILD_T4_Y;
  drawPosX[BUILD_HORZ] = BUILD_HORZ_X;
  drawPosY[BUILD_HORZ] = BUILD_HORZ_Y;
  drawPosX[BUILD_VERT] = BUILD_VERT_X;
  drawPosY[BUILD_VERT] = BUILD_VERT_Y;
  drawPosX[BUILD_VERTEND1] = BUILD_VERTEND1_X;
  drawPosY[BUILD_VERTEND1] = BUILD_VERTEND1_Y;
  drawPosX[BUILD_VERTEND2] = BUILD_VERTEND2_X;
  drawPosY[BUILD_VERTEND2] = BUILD_VERTEND2_Y;
  drawPosX[BUILD_HORZEND1] = BUILD_HORZEND1_X;
  drawPosY[BUILD_HORZEND1] = BUILD_HORZEND1_Y;
  drawPosX[BUILD_HORZEND2] = BUILD_HORZEND2_X;
  drawPosY[BUILD_HORZEND2] = BUILD_HORZEND2_Y;
  drawPosX[BUILD_CROSS] = BUILD_CROSS_X;
  drawPosY[BUILD_CROSS] = BUILD_CROSS_Y;
  drawPosX[BUILD_SIDE1] = BUILD_SIDE1_X;
  drawPosY[BUILD_SIDE1] = BUILD_SIDE1_Y;
  drawPosX[BUILD_SIDE2] = BUILD_SIDE2_X;
  drawPosY[BUILD_SIDE2] = BUILD_SIDE2_Y;
  drawPosX[BUILD_SIDE3] = BUILD_SIDE3_X;
  drawPosY[BUILD_SIDE3] = BUILD_SIDE3_Y;
  drawPosX[BUILD_SIDE4] = BUILD_SIDE4_X;
  drawPosY[BUILD_SIDE4] = BUILD_SIDE4_Y;
  drawPosX[BUILD_SIDECORN1] = BUILD_SIDECORN1_X;
  drawPosY[BUILD_SIDECORN1] = BUILD_SIDECORN1_Y;
  drawPosX[BUILD_SIDECORN2] = BUILD_SIDECORN2_X;
  drawPosY[BUILD_SIDECORN2] = BUILD_SIDECORN2_Y;
  drawPosX[BUILD_SIDECORN3] = BUILD_SIDECORN3_X;
  drawPosY[BUILD_SIDECORN3] = BUILD_SIDECORN3_Y;
  drawPosX[BUILD_SIDECORN4] = BUILD_SIDECORN4_X;
  drawPosY[BUILD_SIDECORN4] = BUILD_SIDECORN4_Y;
  drawPosX[BUILD_SIDECORN5] = BUILD_SIDECORN5_X;
  drawPosY[BUILD_SIDECORN5] = BUILD_SIDECORN5_Y;
  drawPosX[BUILD_SIDECORN6] = BUILD_SIDECORN6_X;
  drawPosY[BUILD_SIDECORN6] = BUILD_SIDECORN6_Y;
  drawPosX[BUILD_SIDECORN7] = BUILD_SIDECORN7_X;
  drawPosY[BUILD_SIDECORN7] = BUILD_SIDECORN7_Y;
  drawPosX[BUILD_SIDECORN8] = BUILD_SIDECORN8_X;
  drawPosY[BUILD_SIDECORN8] = BUILD_SIDECORN8_Y;
  drawPosX[BUILD_SIDECORN9] = BUILD_SIDECORN9_X;
  drawPosY[BUILD_SIDECORN9] = BUILD_SIDECORN9_Y;
  drawPosX[BUILD_SIDECORN10] = BUILD_SIDECORN10_X;
  drawPosY[BUILD_SIDECORN10] = BUILD_SIDECORN10_Y;
  drawPosX[BUILD_SIDECORN11] = BUILD_SIDECORN11_X;
  drawPosY[BUILD_SIDECORN11] = BUILD_SIDECORN11_Y;
  drawPosX[BUILD_SIDECORN12] = BUILD_SIDECORN12_X;
  drawPosY[BUILD_SIDECORN12] = BUILD_SIDECORN12_Y;
  drawPosX[BUILD_SIDECORN13] = BUILD_SIDECORN13_X;
  drawPosY[BUILD_SIDECORN13] = BUILD_SIDECORN13_Y;
  drawPosX[BUILD_SIDECORN14] = BUILD_SIDECORN14_X;
  drawPosY[BUILD_SIDECORN14] = BUILD_SIDECORN14_Y;
  drawPosX[BUILD_SIDECORN15] = BUILD_SIDECORN15_X;
  drawPosY[BUILD_SIDECORN15] = BUILD_SIDECORN15_Y;
  drawPosX[BUILD_SIDECORN16] = BUILD_SIDECORN16_X;
  drawPosY[BUILD_SIDECORN16] = BUILD_SIDECORN16_Y;
  drawPosX[BUILD_TWIST1] = BUILD_TWIST1_X;
  drawPosY[BUILD_TWIST1] = BUILD_TWIST1_Y;
  drawPosX[BUILD_TWIST2] = BUILD_TWIST2_X;
  drawPosY[BUILD_TWIST2] = BUILD_TWIST2_Y;
  drawPosX[BUILD_MOST1] = BUILD_MOST1_X;
  drawPosY[BUILD_MOST1] = BUILD_MOST1_Y;
  drawPosX[BUILD_MOST2] = BUILD_MOST2_X;
  drawPosY[BUILD_MOST2] = BUILD_MOST2_Y;
  drawPosX[BUILD_MOST3] = BUILD_MOST3_X;
  drawPosY[BUILD_MOST3] = BUILD_MOST3_Y;
  drawPosX[BUILD_MOST4] = BUILD_MOST4_X;
  drawPosY[BUILD_MOST4] = BUILD_MOST4_Y;

  drawPosX[RIVER_END1] = RIVER_END1_X;
  drawPosY[RIVER_END1] = RIVER_END1_Y;
  drawPosX[RIVER_END2] = RIVER_END2_X;
  drawPosY[RIVER_END2] = RIVER_END2_Y;
  drawPosX[RIVER_END3] = RIVER_END3_X;
  drawPosY[RIVER_END3] = RIVER_END3_Y;
  drawPosX[RIVER_END4] = RIVER_END4_X;
  drawPosY[RIVER_END4] = RIVER_END4_Y;
  drawPosX[RIVER_SOLID] = RIVER_SOLID_X;
  drawPosY[RIVER_SOLID] = RIVER_SOLID_Y;
  drawPosX[RIVER_SURROUND] = RIVER_SURROUND_X;
  drawPosY[RIVER_SURROUND] = RIVER_SURROUND_Y;
  drawPosX[RIVER_SIDE1] = RIVER_SIDE1_X;
  drawPosY[RIVER_SIDE1] = RIVER_SIDE1_Y;
  drawPosX[RIVER_SIDE2] = RIVER_SIDE2_X;
  drawPosY[RIVER_SIDE2] = RIVER_SIDE2_Y;
  drawPosX[RIVER_ONESIDE1] = RIVER_ONESIDE1_X;
  drawPosY[RIVER_ONESIDE1] = RIVER_ONESIDE1_Y;
  drawPosX[RIVER_ONESIDE2] = RIVER_ONESIDE2_X;
  drawPosY[RIVER_ONESIDE2] = RIVER_ONESIDE2_Y;
  drawPosX[RIVER_ONESIDE3] = RIVER_ONESIDE3_X;
  drawPosY[RIVER_ONESIDE3] = RIVER_ONESIDE3_Y;
  drawPosX[RIVER_ONESIDE4] = RIVER_ONESIDE4_X;
  drawPosY[RIVER_ONESIDE4] = RIVER_ONESIDE4_Y;
  drawPosX[RIVER_CORN1] = RIVER_CORN1_X;
  drawPosY[RIVER_CORN1] = RIVER_CORN1_Y;
  drawPosX[RIVER_CORN2] = RIVER_CORN2_X;
  drawPosY[RIVER_CORN2] = RIVER_CORN2_Y;
  drawPosX[RIVER_CORN3] = RIVER_CORN3_X;
  drawPosY[RIVER_CORN3] = RIVER_CORN3_Y;
  drawPosX[RIVER_CORN4] = RIVER_CORN4_X;
  drawPosY[RIVER_CORN4] = RIVER_CORN4_Y;

  drawPosX[SWAMP] = SWAMP_X;
  drawPosY[SWAMP] = SWAMP_Y;
  drawPosX[CRATER] = CRATER_X;
  drawPosY[CRATER] = CRATER_Y;

  drawPosX[ROAD_CORNER1] = ROAD_CORNER1_X;
  drawPosY[ROAD_CORNER1] = ROAD_CORNER1_Y;
  drawPosX[ROAD_CORNER2] = ROAD_CORNER2_X;
  drawPosY[ROAD_CORNER2] = ROAD_CORNER2_Y;
  drawPosX[ROAD_CORNER3] = ROAD_CORNER3_X;
  drawPosY[ROAD_CORNER3] = ROAD_CORNER3_Y;
  drawPosX[ROAD_CORNER4] = ROAD_CORNER4_X;
  drawPosY[ROAD_CORNER4] = ROAD_CORNER4_Y;
  drawPosX[ROAD_CORNER5] = ROAD_CORNER5_X;
  drawPosY[ROAD_CORNER5] = ROAD_CORNER5_Y;
  drawPosX[ROAD_CORNER6] = ROAD_CORNER6_X;
  drawPosY[ROAD_CORNER6] = ROAD_CORNER6_Y;
  drawPosX[ROAD_CORNER7] = ROAD_CORNER7_X;
  drawPosY[ROAD_CORNER7] = ROAD_CORNER7_Y;
  drawPosX[ROAD_CORNER8] = ROAD_CORNER8_X;
  drawPosY[ROAD_CORNER8] = ROAD_CORNER8_Y;

  drawPosX[ROAD_SIDE1] = ROAD_SIDE1_X;
  drawPosY[ROAD_SIDE1] = ROAD_SIDE1_Y;
  drawPosX[ROAD_SIDE2] = ROAD_SIDE2_X;
  drawPosY[ROAD_SIDE2] = ROAD_SIDE2_Y;
  drawPosX[ROAD_SIDE3] = ROAD_SIDE3_X;
  drawPosY[ROAD_SIDE3] = ROAD_SIDE3_Y;
  drawPosX[ROAD_SIDE4] = ROAD_SIDE4_X;
  drawPosY[ROAD_SIDE4] = ROAD_SIDE4_Y;
  drawPosX[ROAD_SOLID] = ROAD_SOLID_X;
  drawPosY[ROAD_SOLID] = ROAD_SOLID_Y;
  drawPosX[ROAD_CROSS] = ROAD_CROSS_X;
  drawPosY[ROAD_CROSS] = ROAD_CROSS_Y;
  drawPosX[ROAD_T1] = ROAD_T1_X;
  drawPosY[ROAD_T1] = ROAD_T1_Y;
  drawPosX[ROAD_T2] = ROAD_T2_X;
  drawPosY[ROAD_T2] = ROAD_T2_Y;
  drawPosX[ROAD_T3] = ROAD_T3_X;
  drawPosY[ROAD_T3] = ROAD_T3_Y;
  drawPosX[ROAD_T4] = ROAD_T4_X;
  drawPosY[ROAD_T4] = ROAD_T4_Y;
  drawPosX[ROAD_HORZ] = ROAD_HORZ_X;
  drawPosY[ROAD_HORZ] = ROAD_HORZ_Y;
  drawPosX[ROAD_VERT] = ROAD_VERT_X;
  drawPosY[ROAD_VERT] = ROAD_VERT_Y;
  drawPosX[ROAD_WATER1] = ROAD_WATER1_X;
  drawPosY[ROAD_WATER1] = ROAD_WATER1_Y;
  drawPosX[ROAD_WATER2] = ROAD_WATER2_X;
  drawPosY[ROAD_WATER2] = ROAD_WATER2_Y;
  drawPosX[ROAD_WATER3] = ROAD_WATER3_X;
  drawPosY[ROAD_WATER3] = ROAD_WATER3_Y;
  drawPosX[ROAD_WATER4] = ROAD_WATER4_X;
  drawPosY[ROAD_WATER4] = ROAD_WATER4_Y;
  drawPosX[ROAD_WATER5] = ROAD_WATER5_X;
  drawPosY[ROAD_WATER5] = ROAD_WATER5_Y;
  drawPosX[ROAD_WATER6] = ROAD_WATER6_X;
  drawPosY[ROAD_WATER6] = ROAD_WATER6_Y;
  drawPosX[ROAD_WATER7] = ROAD_WATER7_X;
  drawPosY[ROAD_WATER7] = ROAD_WATER7_Y;
  drawPosX[ROAD_WATER8] = ROAD_WATER8_X;
  drawPosY[ROAD_WATER8] = ROAD_WATER8_Y;
  drawPosX[ROAD_WATER9] = ROAD_WATER9_X;
  drawPosY[ROAD_WATER9] = ROAD_WATER9_Y;
  drawPosX[ROAD_WATER10] = ROAD_WATER10_X;
  drawPosY[ROAD_WATER10] = ROAD_WATER10_Y;
  drawPosX[ROAD_WATER11] = ROAD_WATER11_X;
  drawPosY[ROAD_WATER11] = ROAD_WATER11_Y;

  drawPosX[PILL_EVIL_15] = PILL_EVIL15_X;
  drawPosY[PILL_EVIL_15] = PILL_EVIL15_Y;
  drawPosX[PILL_EVIL_14] = PILL_EVIL14_X;
  drawPosY[PILL_EVIL_14] = PILL_EVIL14_Y;
  drawPosX[PILL_EVIL_13] = PILL_EVIL13_X;
  drawPosY[PILL_EVIL_13] = PILL_EVIL13_Y;
  drawPosX[PILL_EVIL_12] = PILL_EVIL12_X;
  drawPosY[PILL_EVIL_12] = PILL_EVIL12_Y;
  drawPosX[PILL_EVIL_11] = PILL_EVIL11_X;
  drawPosY[PILL_EVIL_11] = PILL_EVIL11_Y;
  drawPosX[PILL_EVIL_10] = PILL_EVIL10_X;
  drawPosY[PILL_EVIL_10] = PILL_EVIL10_Y;
  drawPosX[PILL_EVIL_9] = PILL_EVIL9_X;
  drawPosY[PILL_EVIL_9] = PILL_EVIL9_Y;
  drawPosX[PILL_EVIL_8] = PILL_EVIL8_X;
  drawPosY[PILL_EVIL_8] = PILL_EVIL8_Y;
  drawPosX[PILL_EVIL_7] = PILL_EVIL7_X;
  drawPosY[PILL_EVIL_7] = PILL_EVIL7_Y;
  drawPosX[PILL_EVIL_6] = PILL_EVIL6_X;
  drawPosY[PILL_EVIL_6] = PILL_EVIL6_Y;
  drawPosX[PILL_EVIL_5] = PILL_EVIL5_X;
  drawPosY[PILL_EVIL_5] = PILL_EVIL5_Y;
  drawPosX[PILL_EVIL_4] = PILL_EVIL4_X;
  drawPosY[PILL_EVIL_4] = PILL_EVIL4_Y;
  drawPosX[PILL_EVIL_3] = PILL_EVIL3_X;
  drawPosY[PILL_EVIL_3] = PILL_EVIL3_Y;
  drawPosX[PILL_EVIL_2] = PILL_EVIL2_X;
  drawPosY[PILL_EVIL_2] = PILL_EVIL2_Y;
  drawPosX[PILL_EVIL_1] = PILL_EVIL1_X;
  drawPosY[PILL_EVIL_1] = PILL_EVIL1_Y;
  drawPosX[PILL_EVIL_0] = PILL_EVIL0_X;
  drawPosY[PILL_EVIL_0] = PILL_EVIL0_Y;

  drawPosX[PILL_GOOD_15] = PILL_GOOD15_X;
  drawPosY[PILL_GOOD_15] = PILL_GOOD15_Y;
  drawPosX[PILL_GOOD_14] = PILL_GOOD14_X;
  drawPosY[PILL_GOOD_14] = PILL_GOOD14_Y;
  drawPosX[PILL_GOOD_13] = PILL_GOOD13_X;
  drawPosY[PILL_GOOD_13] = PILL_GOOD13_Y;
  drawPosX[PILL_GOOD_12] = PILL_GOOD12_X;
  drawPosY[PILL_GOOD_12] = PILL_GOOD12_Y;
  drawPosX[PILL_GOOD_11] = PILL_GOOD11_X;
  drawPosY[PILL_GOOD_11] = PILL_GOOD11_Y;
  drawPosX[PILL_GOOD_10] = PILL_GOOD10_X;
  drawPosY[PILL_GOOD_10] = PILL_GOOD10_Y;
  drawPosX[PILL_GOOD_9] = PILL_GOOD9_X;
  drawPosY[PILL_GOOD_9] = PILL_GOOD9_Y;
  drawPosX[PILL_GOOD_8] = PILL_GOOD8_X;
  drawPosY[PILL_GOOD_8] = PILL_GOOD8_Y;
  drawPosX[PILL_GOOD_7] = PILL_GOOD7_X;
  drawPosY[PILL_GOOD_7] = PILL_GOOD7_Y;
  drawPosX[PILL_GOOD_6] = PILL_GOOD6_X;
  drawPosY[PILL_GOOD_6] = PILL_GOOD6_Y;
  drawPosX[PILL_GOOD_5] = PILL_GOOD5_X;
  drawPosY[PILL_GOOD_5] = PILL_GOOD5_Y;
  drawPosX[PILL_GOOD_4] = PILL_GOOD4_X;
  drawPosY[PILL_GOOD_4] = PILL_GOOD4_Y;
  drawPosX[PILL_GOOD_3] = PILL_GOOD3_X;
  drawPosY[PILL_GOOD_3] = PILL_GOOD3_Y;
  drawPosX[PILL_GOOD_2] = PILL_GOOD2_X;
  drawPosY[PILL_GOOD_2] = PILL_GOOD2_Y;
  drawPosX[PILL_GOOD_1] = PILL_GOOD1_X;
  drawPosY[PILL_GOOD_1] = PILL_GOOD1_Y;
  drawPosX[PILL_GOOD_0] = PILL_GOOD0_X;
  drawPosY[PILL_GOOD_0] = PILL_GOOD0_Y;

  drawPosX[BASE_GOOD] = BASE_GOOD_X;
  drawPosY[BASE_GOOD] = BASE_GOOD_Y;
  drawPosX[BASE_NEUTRAL] = BASE_NEUTRAL_X;
  drawPosY[BASE_NEUTRAL] = BASE_NEUTRAL_Y;
  drawPosX[BASE_EVIL] = BASE_EVIL_X;
  drawPosY[BASE_EVIL] = BASE_EVIL_Y;

  drawPosX[FOREST] = FOREST_X;
  drawPosY[FOREST] = FOREST_Y;
  drawPosX[FOREST_SINGLE] = FOREST_SINGLE_X;
  drawPosY[FOREST_SINGLE] = FOREST_SINGLE_Y;
  drawPosX[FOREST_BR] = FOREST_BR_X;
  drawPosY[FOREST_BR] = FOREST_BR_Y;
  drawPosX[FOREST_BL] = FOREST_BL_X;
  drawPosY[FOREST_BL] = FOREST_BL_Y;
  drawPosX[FOREST_AR] = FOREST_AR_X;
  drawPosY[FOREST_AR] = FOREST_AR_Y;
  drawPosX[FOREST_AL] = FOREST_AL_X;
  drawPosY[FOREST_AL] = FOREST_AL_Y;
  drawPosX[FOREST_ABOVE] = FOREST_ABOVE_X;
  drawPosY[FOREST_ABOVE] = FOREST_ABOVE_Y;
  drawPosX[FOREST_BELOW] = FOREST_BELOW_X;
  drawPosY[FOREST_BELOW] = FOREST_BELOW_Y;
  drawPosX[FOREST_LEFT] = FOREST_LEFT_X;
  drawPosY[FOREST_LEFT] = FOREST_LEFT_Y;
  drawPosX[FOREST_RIGHT] = FOREST_RIGHT_X;
  drawPosY[FOREST_RIGHT] = FOREST_RIGHT_Y;

  drawPosX[RUBBLE] = RUBBLE_X;
  drawPosY[RUBBLE] = RUBBLE_Y;
  drawPosX[GRASS] = GRASS_X;
  drawPosY[GRASS] = GRASS_Y;
  drawPosX[HALFBUILDING] = SHOT_BUILDING_X;
  drawPosY[HALFBUILDING] = SHOT_BUILDING_Y;

  drawPosX[BOAT_0] = BOAT0_X;
  drawPosY[BOAT_0] = BOAT0_Y;
  drawPosX[BOAT_1] = BOAT1_X;
  drawPosY[BOAT_1] = BOAT1_Y;
  drawPosX[BOAT_2] = BOAT2_X;
  drawPosY[BOAT_2] = BOAT2_Y;
  drawPosX[BOAT_3] = BOAT3_X;
  drawPosY[BOAT_3] = BOAT3_Y;
  drawPosX[BOAT_4] = BOAT4_X;
  drawPosY[BOAT_4] = BOAT4_Y;
  drawPosX[BOAT_5] = BOAT5_X;
  drawPosY[BOAT_5] = BOAT5_Y;
  drawPosX[BOAT_6] = BOAT6_X;
  drawPosY[BOAT_6] = BOAT6_Y;
  drawPosX[BOAT_7] = BOAT7_X;
  drawPosY[BOAT_7] = BOAT7_Y;

  /* Draw Tank frames */

  /* Do I want to do this?
  drawTankPosX[TANK_SELF_0] =  TANK_SELF_0_X;
  drawTankPosY[TANK_SELF_0] =  TANK_SELF_0_Y;
  drawTankPosX[TANK_SELF_1] =  TANK_SELF_1_X;
  drawTankPosY[TANK_SELF_1] =  TANK_SELF_1_Y;
  drawTankPosX[TANK_SELF_2] =  TANK_SELF_2_X;
  drawTankPosY[TANK_SELF_2] =  TANK_SELF_2_Y;
  drawTankPosX[TANK_SELF_3] =  TANK_SELF_3_X;
  drawTankPosY[TANK_SELF_3] =  TANK_SELF_3_Y;
  drawTankPosX[TANK_SELF_4] =  TANK_SELF_4_X;
  drawTankPosY[TANK_SELF_4] =  TANK_SELF_4_Y;
  drawTankPosX[TANK_SELF_5] =  TANK_SELF_5_X;
  drawTankPosY[TANK_SELF_5] =  TANK_SELF_5_Y;
  drawTankPosX[TANK_SELF_6] =  TANK_SELF_6_X;
  drawTankPosY[TANK_SELF_6] =  TANK_SELF_6_Y;
  drawTankPosX[TANK_SELF_7] =  TANK_SELF_7_X;
  drawTankPosY[TANK_SELF_7] =  TANK_SELF_7_Y;
  drawTankPosX[TANK_SELF_8] =  TANK_SELF_8_X;
  drawTankPosY[TANK_SELF_8] =  TANK_SELF_8_Y;
  drawTankPosX[TANK_SELF_9] =  TANK_SELF_9_X;
  drawTankPosY[TANK_SELF_9] =  TANK_SELF_9_Y;
  drawTankPosX[TANK_SELF_10] =  TANK_SELF_10_X;
  drawTankPosY[TANK_SELF_10] =  TANK_SELF_10_Y;
  drawTankPosX[TANK_SELF_11] =  TANK_SELF_11_X;
  drawTankPosY[TANK_SELF_11] =  TANK_SELF_11_Y;
  drawTankPosX[TANK_SELF_12] =  TANK_SELF_12_X;
  drawTankPosY[TANK_SELF_12] =  TANK_SELF_12_Y;
  drawTankPosX[TANK_SELF_13] =  TANK_SELF_13_X;
  drawTankPosY[TANK_SELF_13] =  TANK_SELF_13_Y;
  drawTankPosX[TANK_SELF_14] =  TANK_SELF_14_X;
  drawTankPosY[TANK_SELF_14] =  TANK_SELF_14_Y;
  drawTankPosX[TANK_SELF_15] =  TANK_SELF_15_X;
  drawTankPosY[TANK_SELF_15] =  TANK_SELF_15_Y;

  drawTankPosX[TANK_SELFBOAT_0] =  TANK_SELFBOAT_0_X;
  drawTankPosY[TANK_SELFBOAT_0] =  TANK_SELFBOAT_0_Y;
  drawTankPosX[TANK_SELFBOAT_1] =  TANK_SELFBOAT_1_X;
  drawTankPosY[TANK_SELFBOAT_1] =  TANK_SELFBOAT_1_Y;
  drawTankPosX[TANK_SELFBOAT_2] =  TANK_SELFBOAT_2_X;
  drawTankPosY[TANK_SELFBOAT_2] =  TANK_SELFBOAT_2_Y;
  drawTankPosX[TANK_SELFBOAT_3] =  TANK_SELFBOAT_3_X;
  drawTankPosY[TANK_SELFBOAT_3] =  TANK_SELFBOAT_3_Y;
  drawTankPosX[TANK_SELFBOAT_4] =  TANK_SELFBOAT_4_X;
  drawTankPosY[TANK_SELFBOAT_4] =  TANK_SELFBOAT_4_Y;
  drawTankPosX[TANK_SELFBOAT_5] =  TANK_SELFBOAT_5_X;
  drawTankPosY[TANK_SELFBOAT_5] =  TANK_SELFBOAT_5_Y;
  drawTankPosX[TANK_SELFBOAT_6] =  TANK_SELFBOAT_6_X;
  drawTankPosY[TANK_SELFBOAT_6] =  TANK_SELFBOAT_6_Y;
  drawTankPosX[TANK_SELFBOAT_7] =  TANK_SELFBOAT_7_X;
  drawTankPosY[TANK_SELFBOAT_7] =  TANK_SELFBOAT_7_Y;
  drawTankPosX[TANK_SELFBOAT_8] =  TANK_SELFBOAT_8_X;
  drawTankPosY[TANK_SELFBOAT_8] =  TANK_SELFBOAT_8_Y;
  drawTankPosX[TANK_SELFBOAT_9] =  TANK_SELFBOAT_9_X;
  drawTankPosY[TANK_SELFBOAT_9] =  TANK_SELFBOAT_9_Y;
  drawTankPosX[TANK_SELFBOAT_10] =  TANK_SELFBOAT_10_X;
  drawTankPosY[TANK_SELFBOAT_10] =  TANK_SELFBOAT_10_Y;
  drawTankPosX[TANK_SELFBOAT_11] =  TANK_SELFBOAT_11_X;
  drawTankPosY[TANK_SELFBOAT_11] =  TANK_SELFBOAT_11_Y;
  drawTankPosX[TANK_SELFBOAT_12] =  TANK_SELFBOAT_12_X;
  drawTankPosY[TANK_SELFBOAT_12] =  TANK_SELFBOAT_12_Y;
  drawTankPosX[TANK_SELFBOAT_13] =  TANK_SELFBOAT_13_X;
  drawTankPosY[TANK_SELFBOAT_13] =  TANK_SELFBOAT_13_Y;
  drawTankPosX[TANK_SELFBOAT_14] =  TANK_SELFBOAT_14_X;
  drawTankPosY[TANK_SELFBOAT_14] =  TANK_SELFBOAT_14_Y;
  drawTankPosX[TANK_SELFBOAT_15] =  TANK_SELFBOAT_15_X;
  drawTankPosY[TANK_SELFBOAT_15] =  TANK_SELFBOAT_15_Y; */
}
