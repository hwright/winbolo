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
*Name:          dnsLookups
*Filename:      dnsLookups.c
*Author:        John Morrison
*Creation Date: 10/04/01
*Last Modified: 10/04/01
*Purpose:
*  WinBolo Server Thread manager
*********************************************************/

#ifdef _WIN32
	#include <windows.h>
	#include <winbase.h>
#else
	#include "SDL.h"
	#include "SDL_thread.h"
	typedef SDL_mutex *HANDLE;
#endif
#include <unistd.h>
#include <string.h>
#include "../../bolo/global.h"
#include "../netclient.h"
#include "../dnsLookups.h"



static HANDLE hDnsMutexHandle = NULL;
static SDL_Thread *hDnsThread;
static dnsList dnsProcessing;
static dnsList dnsWaiting;
static bool dnsShouldRun;
static bool dnsFinished;

/*********************************************************
*NAME:          eCreate
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  Creates the DNS Lookups thread. Returns success
*
*ARGUMENTS:
*
*********************************************************/
bool dnsLookupsCreate(void) {
  bool returnValue;        /* Value to return */

  returnValue = true;
  dnsProcessing = NULL;
  dnsWaiting = NULL;
  dnsShouldRun = true;
  dnsFinished = false;
  
  hDnsMutexHandle = SDL_CreateMutex();
  if (hDnsMutexHandle == NULL) {
    returnValue = false;
  }

  /* create thread and run */
  if (returnValue == true) {
    hDnsThread = SDL_CreateThread(dnsLookupsRun, NULL);
    if (hDnsThread  == NULL) {
      SDL_DestroyMutex(hDnsMutexHandle);
      hDnsMutexHandle = NULL;
      returnValue = false;
    }

  }
  return returnValue;
}

/*********************************************************
*NAME:          dnsLookupsDestroy
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  Destroys the DNS lookup Thread.
*
*ARGUMENTS:
*
*********************************************************/
void dnsLookupsDestroy(void) {
  dnsList del; /* Use to delete our queues */

  if (hDnsMutexHandle != NULL) { /* FIXME: Will be non null if we started it OK. Is there a better way? (threadid?) */
    /* Wait for current to finish */
    dnsShouldRun = false;
    while (dnsFinished == false) {
      /* Wait a bit for the last call to finish */
      sleep(DNS_SHUTDOWN_SLEEP_TIME_LINUX);
    }
    SDL_WaitThread(hDnsThread, NULL);

    /* Free our list queues */
    SDL_mutexP(hDnsMutexHandle);
    while (NonEmpty(dnsProcessing)) {
      del = dnsProcessing;
      dnsProcessing = dnsProcessing->next;
      delete del;
    }
    while (NonEmpty(dnsWaiting)) {
      del = dnsWaiting;
      dnsWaiting = dnsWaiting->next;
      delete del;
    } 
    SDL_mutexV(hDnsMutexHandle);

    /* Remove the locking mutex */
    SDL_DestroyMutex(hDnsMutexHandle);
    hDnsMutexHandle = NULL;
  }
}


/*********************************************************
*NAME:          dnsLookupsAddRequest
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  Adds a request to the DNS lookup queue
*
*ARGUMENTS:
*  ip   - The IP address or DNS name to lookup
*  func - The function to call back when the results have
*         have be determined
*********************************************************/
void dnsLookupsAddRequest(char *ip, void (*func)(char*, char*)) {
  dnsList add; /* Used to add to the queue */

  if (dnsShouldRun == true) {
    add = new dnsListObj;
    strcpy(add->ip, ip);
    add->func = func;
    SDL_mutexP(hDnsMutexHandle);
    add->next = dnsWaiting;
    dnsWaiting = add;
    SDL_mutexV(hDnsMutexHandle);
  }
}


/*********************************************************
*NAME:          dnsLookupsRun
*AUTHOR:        John Morrison
*CREATION DATE: 10/04/01
*LAST MODIFIED: 10/04/01
*PURPOSE:
*  The DNS Lookups thread run method
*
*ARGUMENTS:
*
*********************************************************/
int dnsLookupsRun(void*) {
  char dest[512]; /* Destination address space */
  dnsList q;      /* Used to iterate through the list */

  while (dnsShouldRun == true) {
    SDL_mutexP(hDnsMutexHandle);
    dnsProcessing = dnsWaiting;
    dnsWaiting = NULL;
    SDL_mutexV(hDnsMutexHandle);
    
    while (NonEmpty(dnsProcessing) && dnsShouldRun == true) {
      q = dnsProcessing;
      netClientGetAddress(q->ip, dest);
/*      func = q->func;
      func(q->ip, dest); */
      /* FIXME: Hardcoded because I don't know how to call back in linux */
      netProcessedDnsLookup(q->ip, dest);
      dnsProcessing = q->next;
      delete q;
    }
    sleep(DNS_THREAD_SLEEP_TIME_LINUX);
  }
  dnsFinished = true;
  return 0;
}

