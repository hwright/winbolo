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

#include "preferences.h"

#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../bolo/global.h"

/*********************************************************
 *NAME:          GetPrivateProfileString
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 26/11/99
 *PURPOSE:
 * Implements the Win32 GetPrivateProfileString function
 *
 *ARGUMENTS:
 * section  - The section name excluding square [ ]
 * item     - The item name to read
 * def      - The default item to replace with if missing
 * output   - The output variable to store in
 * filename - Filename and path to read file from
 *********************************************************/
void GetPrivateProfileString(const char *section, const char *item,
                             const char *def, char *output, int outlen,
                             const char *filename) {
  FILE *fp;
  bool found = false;
  char line[512];
  char sec[512];
  int len;

  found = false;
  fp = fopen(filename, "r");
  if (fp) {
    sprintf(sec, "[%s]\n", section);
    while (!found && !feof(fp)) {
      fgets(line, 512, fp);
      if (strcmp(line, sec) == 0) {
        sprintf(sec, "%s=", item);
        len = strlen(sec);
        while (!found && !feof(fp)) {
          fgets(line, 512, fp);
          if (strncmp(sec, line, len) == 0) {
            found = true;
            strcpy(output, (line + len));
          } else if (line[0] == '[') {
            strncpy(output, def, outlen);
            found = true;
          }
        }
      }
    }

    fclose(fp);
  }

  if (!found) {
    strncpy(output, def, outlen);
  } else {
    /* Stip newline if required */
    if (output[strlen(output) - 1] == '\n') {
      output[strlen(output) - 1] = '\0';
    }
  }
}

/*********************************************************
 *NAME:          WritePrivateProfileString
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 26/11/99
 *PURPOSE:
 * Implements the Win32 WritePrivateProfileString function
 *
 *ARGUMENTS:
 * section - The section name excluding square [ ]
 * item    - The item name to write
 * value   - The value to set the item name too
 * filename - Filename and path to write too
 *********************************************************/
void WritePrivateProfileString(const char *section, const char *item,
                               const char *value, const char *filename) {
  char *buff;
  char line[512];
  char sec[512];
  int len;
  FILE *fp;
  bool done = false;
  bool inSec = false;
  int secLen;

  buff = new char[16 * 1024];
  buff[0] = '\0';
  fp = fopen(filename, "r");
  sprintf(sec, "[%s]", section);
  secLen = strlen(sec);

  if (fp) {
    fgets(line, 512, fp);
    while (!feof(fp)) {
      if (!done) {
        if (strncmp(sec, line, secLen) == 0 && !inSec) {
          /* Found section */
          inSec = true;
          strcat(buff, line);
          sprintf(sec, "%s=", item);
          len = strlen(sec);
        } else if (inSec && line[0] == '[') {
          /* Leaving section - Add here */
          strcat(buff, item);
          strcat(buff, "=");
          strcat(buff, value);
          strcat(buff, "\n");
          strcat(buff, line);
        } else if (inSec && strncmp(sec, line, len) == 0) {
          /* Found the line to replace */
          strcat(buff, item);
          strcat(buff, "=");
          strcat(buff, value);
          strcat(buff, "\n");
          done = true;
        } else {
          /* Just add the line */
          strcat(buff, line);
        }
      } else {
        strcat(buff, line);
      }
      fgets(line, 512, fp);
    }
    fclose(fp);
  } else {
    sprintf(buff, "%s\n%s=%s\n", sec, item, value);
    done = true;
  }
  if (!done) {
    /* We didn't find it in the file and we are still in the right section
     * Append it here
     */
    if (!inSec) {
      strcat(buff, sec);
      strcat(buff, "\n");
    }
    strcat(buff, item);
    strcat(buff, "=");
    strcat(buff, value);
    strcat(buff, "\n");
  }
  fp = fopen(filename, "w");
  fputs(buff, fp);
  fclose(fp);
  delete[] buff;
}

void preferencesMakeDir(char *path) {
  char mkDirPath[FILENAME_MAX];
  mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  strcpy(mkDirPath, path);
  strcat(mkDirPath, "/brains");
  mkdir(mkDirPath, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
}

/*********************************************************
 *NAME:          preferencesGetPreferenceFile
 *AUTHOR:        John Morrison
 *CREATION DATE: 26/11/99
 *LAST MODIFIED: 26/11/99
 *PURPOSE:
 * Returns the preference path and file name. Under linux
 * this is $HOME/.linbolo/linbolo.ini
 *
 *ARGUMENTS:
 * value - Pointer to hold path returned
 *********************************************************/
void preferencesGetPreferenceFile(char *value) {
  struct passwd *pwd;

  pwd = getpwuid(getuid());
  strcpy(value, pwd->pw_dir);
  strcat(value, "/.linbolo/");
  preferencesMakeDir(value);
  strcat(value, PREFERENCE_FILE);
}
