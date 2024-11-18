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

/* This file is heavily based upon sample code provided by Microsoft as part of
 * the DirectX SDK */

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
//
// DSLoadSoundBufferFile Loads an IDirectSoundBuffer from a Win32 resource in
//                      the current application.
//
// Params:
//  pDS         -- Pointer to an IDirectSound that will be used to create
//                 the buffer.
//
//  lpName      -- Name of WAV resource to load the data from.  Can be a
//                 resource id specified using the MAKEINTRESOURCE macro.
//
// Returns an IDirectSoundBuffer containing the wave data or NULL on error.
//
// example:
//  in the application's resource script (.RC file)
//      Turtle WAV turtle.wav
//
//  some code in the application:
//      IDirectSoundBuffer *pDSB = DSLoadSoundBuffer(pDS, "Turtle");
//
//      if (pDSB)
//      {
//          IDirectSoundBuffer_Play(pDSB, 0, 0, DSBPLAY_TOEND);
//          /* ... */
//
///////////////////////////////////////////////////////////////////////////////
IDirectSoundBuffer *DSLoadSoundBufferFile(IDirectSound *pDS, LPCTSTR lpName);

//
// Function added by John Morrison back in early 99 some time.
//

///////////////////////////////////////////////////////////////////////////////
//
// DSLoadSoundBufferResource
//                      Loads an IDirectSoundBuffer from a Win32 resource in
//                      the current application.
//
// Params:
//  pDS         -- Pointer to an IDirectSound that will be used to create
//                 the buffer.
//
//  lpName      -- Name of WAV resource to load the data from.  Can be a
//                 resource id specified using the MAKEINTRESOURCE macro.
//
// Returns an IDirectSoundBuffer containing the wave data or NULL on error.
//
// example:
//  in the application's resource script (.RC file)
//      Turtle WAV turtle.wav
//
//  some code in the application:
//      IDirectSoundBuffer *pDSB = DSLoadSoundBuffer(pDS, "Turtle");
//
//      if (pDSB)
//      {
//          IDirectSoundBuffer_Play(pDSB, 0, 0, DSBPLAY_TOEND);
//          /* ... */
//
///////////////////////////////////////////////////////////////////////////////
IDirectSoundBuffer *DSLoadSoundBufferResource(IDirectSound *pDS,
                                              HMODULE hModule, LPCTSTR lpName);

///////////////////////////////////////////////////////////////////////////////
//
// DSReloadSoundBuffer  Reloads an IDirectSoundBuffer from a Win32 resource in
//                      the current application. normally used to handle
//                      a DSERR_BUFFERLOST error.
// Params:
//  pDSB        -- Pointer to an IDirectSoundBuffer to be reloaded.
//
//  lpName      -- Name of WAV resource to load the data from.  Can be a
//                 resource id specified using the MAKEINTRESOURCE macro.
//
// Returns a BOOL indicating whether the buffer was successfully reloaded.
//
// example:
//  in the application's resource script (.RC file)
//      Turtle WAV turtle.wav
//
//  some code in the application:
//  TryAgain:
//      HRESULT hres = IDirectSoundBuffer_Play(pDSB, 0, 0, DSBPLAY_TOEND);
//
//      if (FAILED(hres))
//      {
//          if ((hres == DSERR_BUFFERLOST) &&
//              DSReloadSoundBuffer(pDSB, "Turtle"))
//          {
//              goto TryAgain;
//          }
//          /* deal with other errors... */
//      }
//
///////////////////////////////////////////////////////////////////////////////
BOOL DSReloadSoundBuffer(IDirectSoundBuffer *pDSB, HMODULE hModule,
                         LPCTSTR lpName);

//
// Same thing just a file name instead
//
BOOL DSReLoadSoundBufferFile(IDirectSoundBuffer *pDSB, LPCTSTR lpFileName);

///////////////////////////////////////////////////////////////////////////////
//
// DSGetWaveResource    Finds a WAV resource in a Win32 module.
//
// Params:
//  hModule     -- Win32 module handle of module containing WAV resource.
//                 Pass NULL to indicate current application.
//
//  lpName      -- Name of WAV resource to load the data from.  Can be a
//                 resource id specified using the MAKEINTRESOURCE macro.
//
//  ppWaveHeader-- Optional pointer to WAVEFORMATEX * to receive a pointer to
//                 the WAVEFORMATEX header in the specified WAV resource.
//                 Pass NULL if not required.
//
//  ppbWaveData -- Optional pointer to BYTE * to receive a pointer to the
//                 waveform data in the specified WAV resource.  Pass NULL if
//                 not required.
//
//  pdwWaveSize -- Optional pointer to DWORD to receive the size of the
//                 waveform data in the specified WAV resource.  Pass NULL if
//                 not required.
//
// Returns a BOOL indicating whether a valid WAV resource was found.
//
///////////////////////////////////////////////////////////////////////////////
BOOL DSGetWaveResource(HMODULE hModule, LPCTSTR lpName,
                       WAVEFORMATEX **ppWaveHeader, BYTE **ppbWaveData,
                       DWORD *pdwWaveSize);

///////////////////////////////////////////////////////////////////////////////
//
// helper routines
//
///////////////////////////////////////////////////////////////////////////////

BOOL DSFillSoundBuffer(IDirectSoundBuffer *pDSB, BYTE *pbWaveData,
                       DWORD dwWaveSize);
BOOL DSParseWaveResource(void *pvRes, WAVEFORMATEX **ppWaveHeader,
                         BYTE **ppbWaveData, DWORD *pdwWaveSize);

#ifdef __cplusplus
}
#endif
