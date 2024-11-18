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

#include <filesystem>
#include <format>
#include <fstream>
#include <string>

#include "../../bolo/global.h"

namespace bolo {

namespace {

void makeDir(std::string_view path) {
  std::filesystem::create_directory(path);
  std::filesystem::create_directory(std::format("{}/brains", path));
}

}  // namespace
std::string GetPrivateProfileString(std::string_view section,
                                    std::string_view item, std::string_view def,
                                    std::string_view filename) {
  std::ifstream input((std::string(filename).c_str()));
  if (input.is_open()) {
    std::string sec = std::format("[{}]", section);
    std::string line;
    while (!input.eof()) {
      std::getline(input, line, '\n');
      if (line == sec) {
        sec = std::format("{}=", item);
        int len = sec.length();
        while (!input.eof()) {
          std::getline(input, line, '\n');
          if (sec.substr(0, len) == line.substr(0, len)) {
            return std::string(line, len);
          } else if (line[0] == '[') {
            return std::string(def);
          }
        }
      }
    }

    input.close();
  }

  return std::string(def);
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
std::string preferencesGetPreferenceFile() {
  struct passwd *pwd;

  pwd = getpwuid(getuid());
  std::string filename = std::format("{}/.linbolo/", pwd->pw_dir);
  makeDir(filename.c_str());
  return std::format("{}{}", filename, PREFERENCE_FILE);
}

}  // namespace bolo
