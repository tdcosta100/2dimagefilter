// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2004 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#include "../Port.h"
#include "GB.h"

u8 *gbMemoryMap[16];

int32 gbRomSizeMask = 0;
int32 gbRomSize     = 0;
int32 gbRamSizeMask = 0;
int32 gbRamSize     = 0;

u8 * gbMemory     = NULL;
u8 * gbVram       = NULL;
u8 * gbRom        = NULL;
u8 * gbRam        = NULL;
u8 * gbWram       = NULL;
u16 *gbLineBuffer = NULL;

u16   gbPalette[128];
u8    gbBgp[4] = { 0, 1, 2, 3};
u8    gbObp0[4] = { 0, 1, 2, 3};
u8    gbObp1[4] = { 0, 1, 2, 3};
int32 gbWindowLine = -1;

int32 gbCgbMode = 0;

u16   gbColorFilter[32768];
int32 gbColorOption      = 0;
int32 gbPaletteOption    = 0;
int32 gbEmulatorType     = 0;
int32 gbBorderOn         = 1;
int32 gbBorderAutomatic  = 0;
int32 gbBorderLineSkip   = 160;
int32 gbBorderRowSkip    = 0;
int32 gbBorderColumnSkip = 0;
int32 gbDmaTicks         = 0;

u8 (*gbSerialFunction)(u8) = NULL;
