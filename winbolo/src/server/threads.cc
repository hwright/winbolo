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
 *Name:          Threads
 *Filename:      threads.c
 *Author:        John Morrison
 *Creation Date: 12/08/99
 *Last Modified: 21/09/03
 *Purpose:
 *  WinBolo Server Thread manager
 *********************************************************/

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "SDL.h"
typedef SDL_mutex *HANDLE;
#endif
#include "../bolo/global.h"
#include "servercore.h"
#include "servertransport.h"
#include "threads.h"

static HANDLE hMutexHandle = nullptr;
static bool threadServerContext = false;
static bool threadStarted = false;

/*********************************************************
 *NAME:          threadsCreate
 *AUTHOR:        John Morrison
 *CREATION DATE: 12/08/99
 *LAST MODIFIED: 21/09/03
 *PURPOSE:
 *  Creates the thread manager for the server and sets up
 *  TCP connection request and UDP listen thread. Returns
 *  success
 *
 *ARGUMENTS:
 *  context - true if to start in server context
 *********************************************************/
bool threadsCreate(bool context) {
  bool returnValue = true; /* Value to return */

  if (threadStarted) {
    return true;
  }
  threadServerContext = context;
  screenServerConsoleMessage("Thread Manager Startup");
#ifdef _WIN32
  hMutexHandle = CreateMutex(NULL, false, "WinBoloDS");
#else
  hMutexHandle = SDL_CreateMutex();
#endif
  if (hMutexHandle == nullptr) {
    returnValue = false;
    fprintf(stderr, "Error Creating Mutex\n");
  }

  threadStarted = returnValue;
  return returnValue;
}

/*********************************************************
 *NAME:          threadsDestroy
 *AUTHOR:        John Morrison
 *CREATION DATE: 12/8/99
 *LAST MODIFIED: 24/8/99
 *PURPOSE:
 *  Terminate and shuts down the threads.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void threadsDestroy(void) {
  threadStarted = false;
  screenServerConsoleMessage("Thread Manager Shutdown");
  /* TCP Listener */
#ifdef _WIN32
  CloseHandle(hMutexHandle);
#else
  SDL_DestroyMutex(hMutexHandle);
#endif
  hMutexHandle = nullptr;
}

/*********************************************************
 *NAME:          threadsWaitFor
 *AUTHOR:        John Morrison
 *CREATION DATE: 12/8/99
 *LAST MODIFIED: 24/8/99
 *PURPOSE:
 *  Waits till the mutex is acquired.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void threadsWaitForMutex(void) {
#ifdef _WIN32
  WaitForSingleObject(hMutexHandle, INFINITE);
#else
  SDL_mutexP(hMutexHandle);
#endif
}

/*********************************************************
 *NAME:          threadsRelease
 *AUTHOR:        John Morrison
 *CREATION DATE: 12/8/99
 *LAST MODIFIED: 24/8/99
 *PURPOSE:
 *  Releases the mutex when we are done with it.
 *
 *ARGUMENTS:
 *
 *********************************************************/
void threadsReleaseMutex(void) {
#ifdef _WIN32
  ReleaseMutex(hMutexHandle);
#else
  SDL_mutexV(hMutexHandle);
#endif
}

/*********************************************************
 *NAME:          threadsGetContext
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/07/03
 *LAST MODIFIED: 27/07/03
 *PURPOSE:
 *  Returns the current thread context. true if we are in
 *  a server context, false if we are in a client
 *
 *ARGUMENTS:
 *
 *********************************************************/
bool threadsGetContext() { return threadServerContext; }

/*********************************************************
 *NAME:          threadsSetContext
 *AUTHOR:        John Morrison
 *CREATION DATE: 27/07/03
 *LAST MODIFIED: 27/07/03
 *PURPOSE:
 *  Sets the current thread context. true if we are in
 *  a server context, false if we are in a client
 *
 *ARGUMENTS:
 *  serverContext - Context to set to
 *********************************************************/
void threadsSetContext(bool serverContext) {
  threadServerContext = serverContext;
}
