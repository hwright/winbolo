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

// Linux Frontend implementation

#ifndef _LINUX_FRONTEND_H
#define _LINUX_FRONTEND_H

#include "../../bolo/frontend.h"

namespace bolo {

class LinuxFrontend : public Frontend {
 public:
  void updateTankStatusBars(uint8_t shells, uint8_t mines, uint8_t armour,
                            uint8_t trees) override;
  void updateBaseStatusBars(uint8_t shells, uint8_t mines,
                            uint8_t armour) override;
  void playSound(sndEffects value) override;
};

}  // namespace bolo

#endif
