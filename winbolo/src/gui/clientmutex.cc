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
 *Name:          clientMutex
 *Filename:      clientMutex.c
 *Author:        John Morrison
 *Creation Date: 27/5/00
 *Last Modified: 27/5/00
 *Purpose:
 *  WinBolo Server Thread manager
 *********************************************************/

#include "clientmutex.h"

#include <mutex>

static std::recursive_mutex hClientMutexHandle;

/*********************************************************
 *NAME:          clientMutexWaitFor
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/5/00
 *LAST MODIFIED: 27/5/00
 *PURPOSE:
 *  Destroys the client Mutex.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void clientMutexWaitFor(void) { hClientMutexHandle.lock(); }

/*********************************************************
 *NAME:          clientMutexRelease
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/5/00
 *LAST MODIFIED: 27/5/00
 *PURPOSE:
 *  Frees the lock on the client Mutex so other waiting
 *  subsystems can aquire it.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void clientMutexRelease(void) { hClientMutexHandle.unlock(); }
