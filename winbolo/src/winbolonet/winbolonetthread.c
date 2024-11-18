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
*Name:          winbolonetThread
*Filename:      winbolonetThread.c
*Author:        John Morrison
*Creation Date: 16/02/03
*Last Modified: 16/02/03
*Purpose:
*  WinBolo.net Thread manager - Used to stop updates
*  causing game problems
*********************************************************/

#ifdef _WIN32
//  #include <windows.h>
//  #include <string.h>
#else
	#include "SDL.h"
	#include "SDL_thread.h"
	typedef SDL_mutex *HANDLE;
  	typedef int DWORD; /* Not used by required by windows code */
#endif
#include <unistd.h>
#include <string.h>
#include "../bolo/global.h"
#include "http.h"
#include "winbolonetthread.h"

HANDLE hWbnMutexHandle = NULL;
#ifdef _WIN32
  HANDLE hWbnThread;
  DWORD wbnThreadID;
#else
  SDL_Thread *hWbnThread;
#endif

wbnList wbnProcessing;
wbnList wbnWaiting;
bool wbnShouldRun;
bool wbnFinished;

/*********************************************************
*NAME:          winbolonetThreadCreate 
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  Creates the winbolonet update thread. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool winbolonetThreadCreate(void) {
  bool returnValue;        /* Value to return */

  returnValue = true;
  wbnProcessing = NULL;
  wbnWaiting = NULL;
  wbnShouldRun = true;
  wbnFinished = false;
  
#ifdef _WIN32
  char name[FILENAME_MAX]; /* Used in Mutex creation */
  sprintf(name, "%s%d", "WBNUPDATE", GetTickCount());
  hWbnMutexHandle = CreateMutex(NULL, false, (LPCTSTR ) name);
#else
  hWbnMutexHandle = SDL_CreateMutex();
#endif
  if (hWbnMutexHandle == NULL) {
    returnValue = false;
  }

  /* create thread and run */
#ifdef _WIN32
  if (returnValue == true) {
    hWbnThread = CreateThread((LPSECURITY_ATTRIBUTES) NULL, 0, (LPTHREAD_START_ROUTINE) winbolonetThreadRun, 0, 0, &wbnThreadID);
    if (hWbnThread == NULL) {
      CloseHandle(hWbnMutexHandle);
      hWbnMutexHandle = NULL;
      returnValue = false;
    }
  }
#else
  if (returnValue == true) {
    hWbnThread = SDL_CreateThread(winbolonetThreadRun, NULL);
    if (hWbnThread  == NULL) {
      SDL_DestroyMutex(hWbnMutexHandle);
      hWbnMutexHandle = NULL;
      returnValue = false;
    }
  }
#endif

  return returnValue;
}

/*********************************************************
*NAME:          winbolonetThreadDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  Destroys the WBN update thread.
*
*ARGUMENTS:
*
*********************************************************/
void winbolonetThreadDestroy(void) {
  wbnList del;   /* Use to delete our queues */

  if (hWbnMutexHandle != NULL) { /* FIXME: Will be non null if we started it OK. Is there a better way? (threadid?) */
    /* Wait for all events to be sent... */
    while (wbnProcessing != NULL || wbnWaiting != NULL) {
#ifdef _WIN32
      Sleep(WBN_SHUTDOWN_SLEEP_TIME);
#else
      sleep(WBN_SHUTDOWN_SLEEP_TIME_LINUX);
#endif
    }

    /* Wait for current to finish */
    wbnShouldRun = false;
    while (wbnFinished == false) {
      /* Wait a bit for the last call to finish */
#ifdef _WIN32
      Sleep(WBN_SHUTDOWN_SLEEP_TIME);
#else
      sleep(WBN_SHUTDOWN_SLEEP_TIME_LINUX);
#endif
    }

    /* End Thread */
#ifdef _WIN32
    DWORD val = 0; /* Thread Exit Value for WIN32 */
    WaitForSingleObject(hWbnThread, WBN_WAIT_THREAD_EXIT);
    GetExitCodeThread(hWbnThread, &val);
    if (val == STILL_ACTIVE) {
      TerminateThread(hWbnThread, 0);
    }
    
    CloseHandle(hWbnThread);
#else
    SDL_WaitThread(hWbnThread, NULL);
#endif

    /* Free our list queues */
#ifdef _WIN32
    WaitForSingleObject(hWbnMutexHandle, INFINITE);
#else
    SDL_mutexP(hWbnMutexHandle);
#endif

    while (NonEmpty(wbnProcessing)) {
      del = wbnProcessing;
      wbnProcessing = wbnProcessing->next;
      free(del);
    }
    while (NonEmpty(wbnWaiting)) {
      del = wbnWaiting;
      wbnWaiting = wbnWaiting->next;
      free(del);
    } 

#ifdef _WIN32
    ReleaseMutex(hWbnMutexHandle);
    CloseHandle(hWbnMutexHandle);
#else
    SDL_mutexV(hWbnMutexHandle);
    SDL_DestroyMutex(hWbnMutexHandle);
#endif
    
    hWbnMutexHandle = NULL;
  }
}


/*********************************************************
*NAME:          winbolonetThreadAddRequest
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  Adds a request to the WBN update queue
*
*ARGUMENTS:
*  data - Data to send 
*  len  - Length of the data 
*********************************************************/
void winbolonetThreadAddRequest(BYTE *data, int len) {
  wbnList add; /* Used to add to the queue */

  if (wbnShouldRun == true) {
    add = malloc(sizeof(*add));
    memcpy(add->data, data, len);
    add->len = len;
#ifdef _WIN32
    WaitForSingleObject(hWbnMutexHandle, INFINITE);
    add->next = wbnWaiting;
    wbnWaiting = add;    
    ReleaseMutex(hWbnMutexHandle);
#else
    SDL_mutexP(hWbnMutexHandle);
    add->next = wbnWaiting;
    wbnWaiting = add;
    SDL_mutexV(hWbnMutexHandle);
#endif
  }
}


/*********************************************************
*NAME:          winbolonetThreadRun
*AUTHOR:        John Morrison
*CREATION DATE: 16/02/03
*LAST MODIFIED: 16/02/03
*PURPOSE:
*  The update thread run method
*
*ARGUMENTS:
*
*********************************************************/
int winbolonetThreadRun() {
  char dest[512]; /* Destination address space */
  wbnList q;      /* Used to iterate through the list */
  wbnList prev;      /* Used to iterate through the list */

  while (wbnShouldRun == true) {

#ifdef _WIN32
    WaitForSingleObject(hWbnMutexHandle, INFINITE);
    wbnProcessing = wbnWaiting;
    wbnWaiting = NULL;   
    ReleaseMutex(hWbnMutexHandle);
#else
    SDL_mutexP(hWbnMutexHandle);
    wbnProcessing = wbnWaiting;
    wbnWaiting = NULL;
    SDL_mutexV(hWbnMutexHandle);
#endif    
    
    while (NonEmpty(wbnProcessing) && wbnShouldRun == true) {
      /* Get last entry */
      if (wbnProcessing->next == NULL) {
        /* Only one entry */
        httpSendMessage(wbnProcessing->data, wbnProcessing->len, (BYTE *) dest, 512);
        free(wbnProcessing);
	wbnProcessing = NULL; 
      } else {
        prev = wbnProcessing;
        q = wbnProcessing;
        while (q->next != NULL) {
          prev = q;
          q = q->next;
        }
        /* Got last entry */
        prev->next = NULL;
        httpSendMessage(q->data, q->len, (BYTE *) dest, 512);
        free(q);
      }
    }
#ifdef _WIN32
    Sleep(WBN_THREAD_SLEEP_TIME);
#else
    sleep(WBN_THREAD_SLEEP_TIME_LINUX);
#endif
  }
  wbnFinished = true;
  return 0;
}

