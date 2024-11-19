/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright.
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
#include <unistd.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <string>

#include "../../bolo/global.h"

namespace bolo {

namespace {

// Make the `path` directory with a `brains` subdirectory.
void MakeDir(std::string_view path) {
  std::filesystem::create_directory(path);
  std::filesystem::create_directory(std::format("{}/brains", path));
}

}  // namespace

std::string GetPrivateProfileString(std::string_view section,
                                    std::string_view item, std::string_view def,
                                    std::string_view filename) {
  std::ifstream input((std::string(filename)));
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

void WritePrivateProfileString(std::string_view section, std::string_view item,
                               std::string_view value,
                               std::string_view filename) {
  char line[512];
  int len;
  FILE *fp;
  bool done = false;
  bool inSec = false;

  std::string buff;
  std::string sec = std::format("[{}]", section);
  int secLen = sec.length();

  fp = fopen(std::string(filename).c_str(), "r");
  if (fp) {
    fgets(line, 512, fp);
    while (!feof(fp)) {
      if (!done) {
        if (strncmp(sec.c_str(), line, secLen) == 0 && !inSec) {
          // Found section
          inSec = true;
          buff.append(line);
          sec = std::format("{}=", item);
          len = sec.length();
        } else if (inSec && line[0] == '[') {
          // Leaving section - Add here
          buff.append(std::format("{}={}\n", item, value));
          buff.append(line);
        } else if (inSec && strncmp(sec.c_str(), line, len) == 0) {
          // Found the line to replace
          buff.append(std::format("{}={}\n", item, value));
          done = true;
        } else {
          // Just add the line
          buff.append(line);
        }
      } else {
        buff.append(line);
      }
      fgets(line, 512, fp);
    }
    fclose(fp);
  } else {
    buff = std::format("{}\n{}={}\n", sec, item, value);
    done = true;
  }

  if (!done) {
    // We didn't find it in the file and we are still in the right section
    // Append it here
    if (!inSec) {
      buff.append(std::format("{}\n", sec));
    }
    buff.append(std::format("{}={}\n", item, value));
  }

  // Write the entire file back out
  std::ofstream output((std::string(filename)));
  output << buff;
  output.close();
}

std::string GetPreferenceFile() {
  struct passwd *pwd;

  pwd = getpwuid(getuid());
  std::string filename = std::format("{}/.linbolo/", pwd->pw_dir);
  MakeDir(filename.c_str());
  return std::format("{}{}", filename, PREFERENCE_FILE);
}

}  // namespace bolo
