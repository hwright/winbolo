/*
 * Copyright (c) 1998-2008 John Morrison.
 * Copyright (c) 2024-     Hyrum Wright.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// Implements the Win32 ini file functions required. Also
// includes functions to preference file name and path.

#ifndef __PREFERENCES_H
#define __PREFERENCES_H

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

namespace bolo {

class Preferences {
 public:
  // Create a set of preferences, using the contents of `filename` for the
  // initial values.  If `filename` does not exist, the initial set will be
  // empty (but `filename` can be created with `write` later.
  Preferences(std::string_view filename);

  // Free resources and write out the preferences to `filename`.
  ~Preferences();

  // Get the value for `section` and `key`.  Returns std::nullopt if neither
  // exist.
  std::optional<std::string> get(std::string_view section,
                                 std::string_view key);

  // Set the value for `section` and `key`, overwriting any existing value.
  void set(std::string_view section, std::string_view key,
           std::string_view value);

  // Output the current preferences to `filename`.
  void write(std::string_view filename);

  static constexpr std::string_view DEFAULT_FILE = "linbolo.ini";

 private:
  // A map of section titles to a map of key/value pairs.
  std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
      prefs_;

  // The filename we will write to.
  std::string filename_;
};

#if 0
// Get the given preference value.
//
// ARGUMENTS:
//  section  - The section name excluding square [ ]
//  item     - The item name to read
//  def      - The default item to replace with if missing
//  filename - Filename and path to read file from
std::string GetPrivateProfileString(std::string_view section,
                                    std::string_view item, std::string_view def,
                                    std::string_view filename);

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
void WritePrivateProfileString(std::string_view section, std::string_view item,
                               std::string_view value,
                               std::string_view filename);
#endif

// Return the preference path and file name. Under linux
// this is $HOME/.linbolo/linbolo.ini
//
// ARGUMENTS:
//  value - Pointer to hold path returned
std::string GetPreferenceFile();

}  // namespace bolo

#endif /* __PREFERENCES_H */
