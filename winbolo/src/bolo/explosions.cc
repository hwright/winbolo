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

// Explosions should only be updated every 3 ticks
const int UPDATE_TIME = 3;

}  // namespace

void ExplosionTracker::addItem(MapPoint pos, uint8_t px, uint8_t py) {
  explosions_[START].push_back({.pos = pos, .px = px, .py = py});
}

void ExplosionTracker::Update() {
  static uint8_t update = 0; /* The update time */

  update++;
  if (update != UPDATE_TIME) {
    return;
  } else {
    update = 0;
  }

  // For all of the items in the `DEATH` state, remove them by bumping each
  // state forward.
  explosions_[1].clear();

  // Now move each list up the array.  This puts the empty array at the back.
  for (int i = 1; i < explosions_.size() - 1; ++i) {
    std::swap(explosions_[i], explosions_[i + 1]);
  }
}

void ExplosionTracker::calcScreenBullets(bolo::ScreenBulletList *sBullets,
                                         uint8_t leftPos, uint8_t rightPos,
                                         uint8_t topPos, uint8_t bottomPos) {
  for (int length = 1; length < explosions_.size(); ++length) {
    for (auto &exp : explosions_[length]) {
      if (exp.pos.x >= leftPos && exp.pos.x < rightPos && exp.pos.y >= topPos &&
          exp.pos.y < bottomPos) {
        sBullets->push_back(bolo::ScreenBullet{
            .pos = {.x = static_cast<uint8_t>(exp.pos.x - leftPos),
                    .y = static_cast<uint8_t>(exp.pos.y - topPos)},
            .px = exp.px,
            .py = exp.py,
            .frame = static_cast<uint8_t>(length)});
      }
    }
  }
}

}  // namespace bolo
