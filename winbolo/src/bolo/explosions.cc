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

#include "explosions.h"

#include "frontend.h"
#include "global.h"
#include "rubble.h"
#include "screen.h"
#include "screentypes.h"

namespace bolo {

namespace {

const int EXPLODE_DEATH = 1;

// Explosions should only be updated every 3 ticks
const int UPDATE_TIME = 3;

}  // namespace

void ExplosionTracker::addItem(MapPoint pos, uint8_t px, uint8_t py,
                               uint8_t startPos) {
  explosions_.push_back({.pos = pos, .px = px, .py = py, .length = startPos});
}

void ExplosionTracker::Update() {
  static uint8_t update = 0; /* The update time */

  update++;
  if (update != UPDATE_TIME) {
    return;
  } else {
    update = 0;
  }

  std::vector<int> to_remove;

  for (int i = 0; i < explosions_.size(); ++i) {
    auto &exp = explosions_[i];
    if (exp.length > EXPLODE_DEATH) {
      exp.length--;
    } else {
      // Remove from data structure
      to_remove.push_back(i);
    }
  }

  // Reverse iterate, because forward iterating would change the indicies
  // of future members
  for (auto it = to_remove.rbegin(); it != to_remove.rend(); ++it) {
    explosions_.erase(explosions_.begin() + *it);
  }
}

void ExplosionTracker::calcScreenBullets(bolo::ScreenBulletList *sBullets,
                                         uint8_t leftPos, uint8_t rightPos,
                                         uint8_t topPos, uint8_t bottomPos) {
  for (auto &exp : explosions_) {
    if (exp.pos.x >= leftPos && exp.pos.x < rightPos && exp.pos.y >= topPos &&
        exp.pos.y < bottomPos) {
      sBullets->push_back(bolo::ScreenBullet{
          .pos = {.x = static_cast<uint8_t>(exp.pos.x - leftPos),
                  .y = static_cast<uint8_t>(exp.pos.y - topPos)},
          .px = exp.px,
          .py = exp.py,
          .frame = exp.length});
    }
  }
}

}  // namespace bolo
