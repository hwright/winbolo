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

Preferences::Preferences(std::string_view filename) : filename_(filename) {
  std::ifstream input(filename_);
  std::unordered_map<std::string, std::string>* section_ptr = nullptr;

  if (input.is_open()) {
    while (!input.eof()) {
      std::string line;
      std::getline(input, line);
      if (line[0] == '[') {
        // Assume section heading
        std::string section = line.substr(1, line.size() - 2);
        section_ptr = &prefs_[section];
      } else if (section_ptr != nullptr && !line.empty()) {
        // Assume key/value pair
        int pos = line.find("=");
        (*section_ptr)[line.substr(0, pos)] = line.substr(pos + 1, line.size());
      }
    }
  }
}

Preferences::~Preferences() { write(filename_); }

std::optional<std::string> Preferences::get(std::string_view section,
                                            std::string_view key) {
  if (auto it = prefs_.find(std::string(section)); it != prefs_.end()) {
    auto& section = it->second;
    if (auto it2 = section.find(std::string(key)); it2 != section.end()) {
      return it2->second;
    }
  }

  return std::nullopt;
}

void Preferences::set(std::string_view section, std::string_view key,
                      std::string_view value) {
  prefs_[std::string(section)][std::string(key)] = value;
}

void Preferences::write(std::string_view filename) {
  std::ofstream output(std::string(filename), std::ios_base::trunc);
  if (output.is_open()) {
    for (auto& [heading, section] : prefs_) {
      output << std::format("[{}]\n", heading);
      for (auto& [key, value] : section) {
        output << std::format("{}={}\n", key, value);
      }
    }
    output.close();
  }
}

std::string GetPreferenceFile() {
  struct passwd* pwd;

  pwd = getpwuid(getuid());
  std::string filename = std::format("{}/.linbolo/", pwd->pw_dir);
  MakeDir(filename);
  return std::format("{}{}", filename, Preferences::DEFAULT_FILE);
}

}  // namespace bolo
