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

int coeff[32] = {
	0,   1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
	16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16
};

// some of the rendering code in gfx.h (such as mode 0 line 1298)
// renders outside the given buffer (past 239) which corrupts other memory,
// so rather than find all places in that code that need to be fixed,
// just give it enough extra scratch space to use
#define LINE_BUFFER_OVERFLOW_LEEWAY (512-240)

u32  line0[240+LINE_BUFFER_OVERFLOW_LEEWAY];
u32  line1[240+LINE_BUFFER_OVERFLOW_LEEWAY];
u32  line2[240+LINE_BUFFER_OVERFLOW_LEEWAY];
u32  line3[240+LINE_BUFFER_OVERFLOW_LEEWAY];
u32  lineOBJ[240+LINE_BUFFER_OVERFLOW_LEEWAY];
u32  lineOBJWin[240+LINE_BUFFER_OVERFLOW_LEEWAY];
u32  lineMix[240+LINE_BUFFER_OVERFLOW_LEEWAY];
bool gfxInWin0[240+LINE_BUFFER_OVERFLOW_LEEWAY];
bool gfxInWin1[240+LINE_BUFFER_OVERFLOW_LEEWAY];

int gfxBG2Changed = 0;
int gfxBG3Changed = 0;

int gfxBG2X       = 0;
int gfxBG2Y       = 0;
int gfxBG2LastX   = 0;
int gfxBG2LastY   = 0;
int gfxBG3X       = 0;
int gfxBG3Y       = 0;
int gfxBG3LastX   = 0;
int gfxBG3LastY   = 0;
int gfxLastVCOUNT = 0;
