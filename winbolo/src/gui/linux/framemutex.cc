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
 *Name:          frameMutex
 *Filename:      frameMutex.c
 *Author:        John Morrison
 *Creation Date: 27/5/00
 *Last Modified: 27/5/00
 *Purpose:
 *  WinBolo Server Thread manager
 *********************************************************/

#include "framemutex.h"

#include <string.h>
#include <time.h>

#include <mutex>

#include "../../bolo/global.h"

static std::recursive_mutex hFrameMutexHandle;

/*********************************************************
 *NAME:          frameMutexWaitFor
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/5/00
 *LAST MODIFIED: 27/5/00
 *PURPOSE:
 *  Destroys the frame Mutex.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void frameMutexWaitFor(void) { hFrameMutexHandle.lock(); }

/*********************************************************
 *NAME:          frameMutexRelease
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/5/00
 *LAST MODIFIED: 27/5/00
 *PURPOSE:
 *  Frees the lock on the frame Mutex so other waiting
 *  subsystems can aquire it.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void frameMutexRelease(void) { hFrameMutexHandle.unlock(); }
