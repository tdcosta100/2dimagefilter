// -*- C++ -*-
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

#ifndef VBA_GBA_GFX_H
#define VBA_GBA_GFX_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Port.h"
#include "GBA.h"
#include "Globals.h"

//#define SPRITE_DEBUG

void gfxDrawTextScreen(u16, u16, u16, u32 *);
void gfxDrawRotScreen(u16,
                      u16, u16,
                      u16, u16,
                      u16, u16,
                      u16, u16,
                      int&, int&,
                      int,
                      u32 *);
void gfxDrawRotScreen16Bit(u16,
                           u16, u16,
                           u16, u16,
                           u16, u16,
                           u16, u16,
                           int&, int&,
                           int,
                           u32 *);
void gfxDrawRotScreen256(u16,
                         u16, u16,
                         u16, u16,
                         u16, u16,
                         u16, u16,
                         int&, int&,
                         int,
                         u32 *);
void gfxDrawRotScreen16Bit160(u16,
                              u16, u16,
                              u16, u16,
                              u16, u16,
                              u16, u16,
                              int&, int&,
                              int,
                              u32 *);
void gfxDrawSprites(u32 *);
void gfxIncreaseBrightness(u32 *line, int coeff);
void gfxDecreaseBrightness(u32 *line, int coeff);
void gfxAlphaBlend(u32 *ta, u32 *tb, int ca, int cb);

void mode0RenderLine();
void mode0RenderLineNoWindow();
void mode0RenderLineAll();

void mode1RenderLine();
void mode1RenderLineNoWindow();
void mode1RenderLineAll();

void mode2RenderLine();
void mode2RenderLineNoWindow();
void mode2RenderLineAll();

void mode3RenderLine();
void mode3RenderLineNoWindow();
void mode3RenderLineAll();

void mode4RenderLine();
void mode4RenderLineNoWindow();
void mode4RenderLineAll();

void mode5RenderLine();
void mode5RenderLineNoWindow();
void mode5RenderLineAll();

extern int  coeff[32];
extern u32  line0[240];
extern u32  line1[240];
extern u32  line2[240];
extern u32  line3[240];
extern u32  lineOBJ[240];
extern u32  lineOBJWin[240];
extern u32  lineMix[240];
extern bool gfxInWin0[240];
extern bool gfxInWin1[240];

extern int gfxBG2Changed;
extern int gfxBG3Changed;

extern int gfxBG2X;
extern int gfxBG2Y;
extern int gfxBG2LastX;
extern int gfxBG2LastY;
extern int gfxBG3X;
extern int gfxBG3Y;
extern int gfxBG3LastX;
extern int gfxBG3LastY;
extern int gfxLastVCOUNT;

inline void gfxClearArray(u32 *array)
{
	for (int i = 0; i < 240; i++)
	{
		*array++ = 0x80000000;
	}
}

inline void gfxDrawTextScreen(u16 control, u16 hofs, u16 vofs,
                              u32 *line)
{
	u16 *palette    = (u16 *)paletteRAM;
	u8 * charBase   = &vram[((control >> 2) & 0x03) * 0x4000];
	u16 *screenBase = (u16 *)&vram[((control >> 8) & 0x1f) * 0x800];
	u32  prio       = ((control & 3)<<25) + 0x1000000;
	int  sizeX      = 256;
	int  sizeY      = 256;
	switch ((control >> 14) & 3)
	{
	case 0:
		break;
	case 1:
		sizeX = 512;
		break;
	case 2:
		sizeY = 512;
		break;
	case 3:
		sizeX = 512;
		sizeY = 512;
		break;
	}

	int maskX = sizeX-1;
	int maskY = sizeY-1;

	bool mosaicOn = (control & 0x40) ? true : false;

	int xxx     = hofs & maskX;
	int yyy     = (vofs + VCOUNT) & maskY;
	int mosaicX = (MOSAIC & 0x000F)+1;
	int mosaicY = ((MOSAIC & 0x00F0)>>4)+1;

	if (mosaicOn)
	{
		if ((VCOUNT % mosaicY) != 0)
		{
			mosaicY = (VCOUNT / mosaicY) * mosaicY;
			yyy     = (vofs + mosaicY) & maskY;
		}
	}

	if (yyy > 255 && sizeY > 256)
	{
		yyy        &= 255;
		screenBase += 0x400;
		if (sizeX > 256)
			screenBase += 0x400;
	}

	int yshift = ((yyy>>3)<<5);
	if ((control) & 0x80)
	{
		u16 *screenSource = screenBase + 0x400 * (xxx>>8) + ((xxx & 255)>>3) + yshift;
		for (int x = 0; x < 240; x++)
		{
			u16 data = READ16LE(screenSource);

			int tile  = data & 0x3FF;
			int tileX = (xxx & 7);
			int tileY = yyy & 7;

			if (data & 0x0400)
				tileX = 7 - tileX;
			if (data & 0x0800)
				tileY = 7 - tileY;

			u8 color = charBase[tile * 64 + tileY * 8 + tileX];

			line[x] = color ? (READ16LE(&palette[color]) | prio) : 0x80000000;

			if (data & 0x0400)
			{
				if (tileX == 0)
					screenSource++;
			}
			else if (tileX == 7)
				screenSource++;
			xxx++;
			if (xxx == 256)
			{
				if (sizeX > 256)
					screenSource = screenBase + 0x400 + yshift;
				else
				{
					screenSource = screenBase + yshift;
					xxx = 0;
				}
			}
			else if (xxx >= sizeX)
			{
				xxx = 0;
				screenSource = screenBase + yshift;
			}
		}
	}
	else
	{
		u16 *screenSource = screenBase + 0x400*(xxx>>8)+((xxx&255)>>3) +
		                    yshift;
		for (int x = 0; x < 240; x++)
		{
			u16 data = READ16LE(screenSource);

			int tile  = data & 0x3FF;
			int tileX = (xxx & 7);
			int tileY = yyy & 7;

			if (data & 0x0400)
				tileX = 7 - tileX;
			if (data & 0x0800)
				tileY = 7 - tileY;

			u8 color = charBase[(tile<<5) + (tileY<<2) + (tileX>>1)];

			if (tileX & 1)
			{
				color = (color >> 4);
			}
			else
			{
				color &= 0x0F;
			}

			int pal = (READ16LE(screenSource)>>8) & 0xF0;
			line[x] = color ? (READ16LE(&palette[pal + color])|prio) : 0x80000000;

			if (data & 0x0400)
			{
				if (tileX == 0)
					screenSource++;
			}
			else if (tileX == 7)
				screenSource++;
			xxx++;
			if (xxx == 256)
			{
				if (sizeX > 256)
					screenSource = screenBase + 0x400 + yshift;
				else
				{
					screenSource = screenBase + yshift;
					xxx = 0;
				}
			}
			else if (xxx >= sizeX)
			{
				xxx = 0;
				screenSource = screenBase + yshift;
			}
		}
	}
	if (mosaicOn)
	{
		if (mosaicX > 1)
		{
			int m = 1;
			for (int i = 0; i < 239; i++)
			{
				line[i+1] = line[i];
				m++;
				if (m == mosaicX)
				{
					m = 1;
					i++;
				}
			}
		}
	}
}

inline void gfxDrawRotScreen(u16 control,
                             u16 x_l, u16 x_h,
                             u16 y_l, u16 y_h,
                             u16 pa,  u16 pb,
                             u16 pc,  u16 pd,
                             int& currentX, int& currentY,
                             int changed,
                             u32 *line)
{
	u16 *palette    = (u16 *)paletteRAM;
	u8 * charBase   = &vram[((control >> 2) & 0x03) * 0x4000];
	u8 * screenBase = (u8 *)&vram[((control >> 8) & 0x1f) * 0x800];
	int  prio       = ((control & 3) << 25) + 0x1000000;

	int sizeX = 128;
	int sizeY = 128;
	switch ((control >> 14) & 3)
	{
	case 0:
		break;
	case 1:
		sizeX = sizeY = 256;
		break;
	case 2:
		sizeX = sizeY = 512;
		break;
	case 3:
		sizeX = sizeY = 1024;
		break;
	}

	int dx = pa & 0x7FFF;
	if (pa & 0x8000)
		dx |= 0xFFFF8000;
	int dmx = pb & 0x7FFF;
	if (pb & 0x8000)
		dmx |= 0xFFFF8000;
	int dy = pc & 0x7FFF;
	if (pc & 0x8000)
		dy |= 0xFFFF8000;
	int dmy = pd & 0x7FFFF;
	if (pd & 0x8000)
		dmy |= 0xFFFF8000;

	if (VCOUNT == 0)
		changed = 3;

	if (changed & 1)
	{
		currentX = (x_l) | ((x_h & 0x07FF)<<16);
		if (x_h & 0x0800)
			currentX |= 0xF8000000;
	}
	else
	{
		currentX += dmx;
	}

	if (changed & 2)
	{
		currentY = (y_l) | ((y_h & 0x07FF)<<16);
		if (y_h & 0x0800)
			currentY |= 0xF8000000;
	}
	else
	{
		currentY += dmy;
	}

	int realX = currentX;
	int realY = currentY;

	if (control & 0x40)
	{
		int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
		int y       = (VCOUNT % mosaicY);
		realX -= y*dmx;
		realY -= y*dmy;
	}

	int xxx = (realX >> 8);
	int yyy = (realY >> 8);

	if (control & 0x2000)
	{
		xxx %= sizeX;
		yyy %= sizeY;
		if (xxx < 0)
			xxx += sizeX;
		if (yyy < 0)
			yyy += sizeY;
	}

	if (control & 0x80)
	{
		for (int x = 0; x < 240; x++)
		{
			if (xxx < 0 ||
			    yyy < 0 ||
			    xxx >= sizeX ||
			    yyy >= sizeY)
			{
				line[x] = 0x80000000;
			}
			else
			{
				int tile = screenBase[(xxx>>3) + (yyy>>3)*(sizeX>>3)];

				int tileX = (xxx & 7);
				int tileY = yyy & 7;

				u8 color = charBase[(tile<<6) + (tileY<<3) + tileX];

				line[x] = color ? (READ16LE(&palette[color])|prio) : 0x80000000;
			}
			realX += dx;
			realY += dy;

			xxx = (realX >> 8);
			yyy = (realY >> 8);

			if (control & 0x2000)
			{
				xxx %= sizeX;
				yyy %= sizeY;
				if (xxx < 0)
					xxx += sizeX;
				if (yyy < 0)
					yyy += sizeY;
			}
		}
	}
	else
	{
		for (int x = 0; x < 240; x++)
		{
			if (xxx < 0 ||
			    yyy < 0 ||
			    xxx >= sizeX ||
			    yyy >= sizeY)
			{
				line[x] = 0x80000000;
			}
			else
			{
				int tile = screenBase[(xxx>>3) + (yyy>>3)*(sizeX>>3)];

				int tileX = (xxx & 7);
				int tileY = yyy & 7;

				u8 color = charBase[(tile<<6) + (tileY<<3) + tileX];

				line[x] = color ? (READ16LE(&palette[color])|prio) : 0x80000000;
			}
			realX += dx;
			realY += dy;

			xxx = (realX >> 8);
			yyy = (realY >> 8);

			if (control & 0x2000)
			{
				xxx %= sizeX;
				yyy %= sizeY;
				if (xxx < 0)
					xxx += sizeX;
				if (yyy < 0)
					yyy += sizeY;
			}
		}
	}

	if (control & 0x40)
	{
		int mosaicX = (MOSAIC & 0xF) + 1;
		if (mosaicX > 1)
		{
			int m = 1;
			for (int i = 0; i < 239; i++)
			{
				line[i+1] = line[i];
				m++;
				if (m == mosaicX)
				{
					m = 1;
					i++;
				}
			}
		}
	}
}

inline void gfxDrawRotScreen16Bit(u16 control,
                                  u16 x_l, u16 x_h,
                                  u16 y_l, u16 y_h,
                                  u16 pa,  u16 pb,
                                  u16 pc,  u16 pd,
                                  int& currentX, int& currentY,
                                  int changed,
                                  u32 *line)
{
	u16 *screenBase = (u16 *)&vram[0];
	int  prio       = ((control & 3) << 25) + 0x1000000;
	int  sizeX      = 240;
	int  sizeY      = 160;

	int startX = (x_l) | ((x_h & 0x07FF)<<16);
	if (x_h & 0x0800)
		startX |= 0xF8000000;
	int startY = (y_l) | ((y_h & 0x07FF)<<16);
	if (y_h & 0x0800)
		startY |= 0xF8000000;

	int dx = pa & 0x7FFF;
	if (pa & 0x8000)
		dx |= 0xFFFF8000;
	int dmx = pb & 0x7FFF;
	if (pb & 0x8000)
		dmx |= 0xFFFF8000;
	int dy = pc & 0x7FFF;
	if (pc & 0x8000)
		dy |= 0xFFFF8000;
	int dmy = pd & 0x7FFFF;
	if (pd & 0x8000)
		dmy |= 0xFFFF8000;

	if (VCOUNT == 0)
		changed = 3;

	if (changed & 1)
	{
		currentX = (x_l) | ((x_h & 0x07FF)<<16);
		if (x_h & 0x0800)
			currentX |= 0xF8000000;
	}
	else
		currentX += dmx;

	if (changed & 2)
	{
		currentY = (y_l) | ((y_h & 0x07FF)<<16);
		if (y_h & 0x0800)
			currentY |= 0xF8000000;
	}
	else
	{
		currentY += dmy;
	}

	int realX = currentX;
	int realY = currentY;

	if (control & 0x40)
	{
		int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
		int y       = (VCOUNT % mosaicY);
		realX -= y*dmx;
		realY -= y*dmy;
	}

	int xxx = (realX >> 8);
	int yyy = (realY >> 8);

	for (int x = 0; x < 240; x++)
	{
		if (xxx < 0 ||
		    yyy < 0 ||
		    xxx >= sizeX ||
		    yyy >= sizeY)
		{
			line[x] = 0x80000000;
		}
		else
		{
			line[x] = (READ16LE(&screenBase[yyy * sizeX + xxx]) | prio);
		}
		realX += dx;
		realY += dy;

		xxx = (realX >> 8);
		yyy = (realY >> 8);
	}

	if (control & 0x40)
	{
		int mosaicX = (MOSAIC & 0xF) + 1;
		if (mosaicX > 1)
		{
			int m = 1;
			for (int i = 0; i < 239; i++)
			{
				line[i+1] = line[i];
				m++;
				if (m == mosaicX)
				{
					m = 1;
					i++;
				}
			}
		}
	}
}

inline void gfxDrawRotScreen256(u16 control,
                                u16 x_l, u16 x_h,
                                u16 y_l, u16 y_h,
                                u16 pa,  u16 pb,
                                u16 pc,  u16 pd,
                                int &currentX, int& currentY,
                                int changed,
                                u32 *line)
{
	u16 *palette    = (u16 *)paletteRAM;
	u8 * screenBase = (DISPCNT & 0x0010) ? &vram[0xA000] : &vram[0x0000];
	int  prio       = ((control & 3) << 25) + 0x1000000;
	int  sizeX      = 240;
	int  sizeY      = 160;

	int startX = (x_l) | ((x_h & 0x07FF)<<16);
	if (x_h & 0x0800)
		startX |= 0xF8000000;
	int startY = (y_l) | ((y_h & 0x07FF)<<16);
	if (y_h & 0x0800)
		startY |= 0xF8000000;

	int dx = pa & 0x7FFF;
	if (pa & 0x8000)
		dx |= 0xFFFF8000;
	int dmx = pb & 0x7FFF;
	if (pb & 0x8000)
		dmx |= 0xFFFF8000;
	int dy = pc & 0x7FFF;
	if (pc & 0x8000)
		dy |= 0xFFFF8000;
	int dmy = pd & 0x7FFFF;
	if (pd & 0x8000)
		dmy |= 0xFFFF8000;

	if (VCOUNT == 0)
		changed = 3;

	if (changed & 1)
	{
		currentX = (x_l) | ((x_h & 0x07FF)<<16);
		if (x_h & 0x0800)
			currentX |= 0xF8000000;
	}
	else
	{
		currentX += dmx;
	}

	if (changed & 2)
	{
		currentY = (y_l) | ((y_h & 0x07FF)<<16);
		if (y_h & 0x0800)
			currentY |= 0xF8000000;
	}
	else
	{
		currentY += dmy;
	}

	int realX = currentX;
	int realY = currentY;

	if (control & 0x40)
	{
		int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
		int y       = (VCOUNT / mosaicY) * mosaicY;
		realX = startX + y*dmx;
		realY = startY + y*dmy;
	}

	int xxx = (realX >> 8);
	int yyy = (realY >> 8);

	for (int x = 0; x < 240; x++)
	{
		if (xxx < 0 ||
		    yyy < 0 ||
		    xxx >= sizeX ||
		    yyy >= sizeY)
		{
			line[x] = 0x80000000;
		}
		else
		{
			u8 color = screenBase[yyy * 240 + xxx];

			line[x] = color ? (READ16LE(&palette[color])|prio) : 0x80000000;
		}
		realX += dx;
		realY += dy;

		xxx = (realX >> 8);
		yyy = (realY >> 8);
	}

	if (control & 0x40)
	{
		int mosaicX = (MOSAIC & 0xF) + 1;
		if (mosaicX > 1)
		{
			int m = 1;
			for (int i = 0; i < 239; i++)
			{
				line[i+1] = line[i];
				m++;
				if (m == mosaicX)
				{
					m = 1;
					i++;
				}
			}
		}
	}
}

inline void gfxDrawRotScreen16Bit160(u16 control,
                                     u16 x_l, u16 x_h,
                                     u16 y_l, u16 y_h,
                                     u16 pa,  u16 pb,
                                     u16 pc,  u16 pd,
                                     int& currentX, int& currentY,
                                     int changed,
                                     u32 *line)
{
	u16 *screenBase = (DISPCNT & 0x0010) ? (u16 *)&vram[0xa000] :
	                  (u16 *)&vram[0];
	int prio  = ((control & 3) << 25) + 0x1000000;
	int sizeX = 160;
	int sizeY = 128;

	int startX = (x_l) | ((x_h & 0x07FF)<<16);
	if (x_h & 0x0800)
		startX |= 0xF8000000;
	int startY = (y_l) | ((y_h & 0x07FF)<<16);
	if (y_h & 0x0800)
		startY |= 0xF8000000;

	int dx = pa & 0x7FFF;
	if (pa & 0x8000)
		dx |= 0xFFFF8000;
	int dmx = pb & 0x7FFF;
	if (pb & 0x8000)
		dmx |= 0xFFFF8000;
	int dy = pc & 0x7FFF;
	if (pc & 0x8000)
		dy |= 0xFFFF8000;
	int dmy = pd & 0x7FFFF;
	if (pd & 0x8000)
		dmy |= 0xFFFF8000;

	if (VCOUNT == 0)
		changed = 3;

	if (changed & 1)
	{
		currentX = (x_l) | ((x_h & 0x07FF)<<16);
		if (x_h & 0x0800)
			currentX |= 0xF8000000;
	}
	else
	{
		currentX += dmx;
	}

	if (changed & 2)
	{
		currentY = (y_l) | ((y_h & 0x07FF)<<16);
		if (y_h & 0x0800)
			currentY |= 0xF8000000;
	}
	else
	{
		currentY += dmy;
	}

	int realX = currentX;
	int realY = currentY;

	if (control & 0x40)
	{
		int mosaicY = ((MOSAIC & 0xF0)>>4) + 1;
		int y       = (VCOUNT / mosaicY) * mosaicY;
		realX = startX + y*dmx;
		realY = startY + y*dmy;
	}

	int xxx = (realX >> 8);
	int yyy = (realY >> 8);

	for (int x = 0; x < 240; x++)
	{
		if (xxx < 0 ||
		    yyy < 0 ||
		    xxx >= sizeX ||
		    yyy >= sizeY)
		{
			line[x] = 0x80000000;
		}
		else
		{
			line[x] = (READ16LE(&screenBase[yyy * sizeX + xxx]) | prio);
		}
		realX += dx;
		realY += dy;

		xxx = (realX >> 8);
		yyy = (realY >> 8);
	}

	if (control & 0x40)
	{
		int mosaicX = (MOSAIC & 0xF) + 1;
		if (mosaicX > 1)
		{
			int m = 1;
			for (int i = 0; i < 239; i++)
			{
				line[i+1] = line[i];
				m++;
				if (m == mosaicX)
				{
					m = 1;
					i++;
				}
			}
		}
	}
}

inline void gfxDrawSprites(u32 *lineOBJ)
{
	int m = 0;
	gfxClearArray(lineOBJ);
	if (layerEnable & 0x1000)
	{
		u16 *sprites       = (u16 *)oam;
		u16 *spritePalette = &((u16 *)paletteRAM)[256];
		int  mosaicY       = ((MOSAIC & 0xF000)>>12) + 1;
		int  mosaicX       = ((MOSAIC & 0xF00)>>8) + 1;
		for (int x = 0; x < 128; x++)
		{
			u16 a0 = READ16LE(sprites++);
			u16 a1 = READ16LE(sprites++);
			u16 a2 = READ16LE(sprites++);
			sprites++;

			// ignore OBJ-WIN
			if ((a0 & 0x0c00) == 0x0800)
				continue;

			int sizeY = 8;
			int sizeX = 8;

			switch (((a0 >>12) & 0x0c)|(a1>>14))
			{
			case 0:
				break;
			case 1:
				sizeX = sizeY = 16;
				break;
			case 2:
				sizeX = sizeY = 32;
				break;
			case 3:
				sizeX = sizeY = 64;
				break;
			case 4:
				sizeX = 16;
				break;
			case 5:
				sizeX = 32;
				break;
			case 6:
				sizeX = 32;
				sizeY = 16;
				break;
			case 7:
				sizeX = 64;
				sizeY = 32;
				break;
			case 8:
				sizeY = 16;
				break;
			case 9:
				sizeY = 32;
				break;
			case 10:
				sizeX = 16;
				sizeY = 32;
				break;
			case 11:
				sizeX = 32;
				sizeY = 64;
				break;
			default:
				continue;
			}

#ifdef SPRITE_DEBUG
			int maskX = sizeX-1;
			int maskY = sizeY-1;
#endif

			int sy = (a0 & 255);

			if (sy > 160)
				sy -= 256;

			if (a0 & 0x0100)
			{
				int fieldX = sizeX;
				int fieldY = sizeY;
				if (a0 & 0x0200)
				{
					fieldX <<= 1;
					fieldY <<= 1;
				}

				int t = VCOUNT - sy;
				if ((t >= 0) && (t < fieldY))
				{
					int sx = (a1 & 0x1FF);
					if ((sx < 240) || (((sx + fieldX) & 511) < 240))
					{
						// int t2 = t - (fieldY >> 1);
						int  rot = (a1 >> 9) & 0x1F;
						u16 *OAM = (u16 *)oam;
						int  dx  = READ16LE(&OAM[3 + (rot << 4)]);
						if (dx & 0x8000)
							dx |= 0xFFFF8000;
						int dmx = READ16LE(&OAM[7 + (rot << 4)]);
						if (dmx & 0x8000)
							dmx |= 0xFFFF8000;
						int dy = READ16LE(&OAM[11 + (rot << 4)]);
						if (dy & 0x8000)
							dy |= 0xFFFF8000;
						int dmy = READ16LE(&OAM[15 + (rot << 4)]);
						if (dmy & 0x8000)
							dmy |= 0xFFFF8000;

						if (a0 & 0x1000)
						{
							t -= (t % mosaicY);
						}

						int realX = ((sizeX) << 7) - (fieldX >> 1)*dx - (fieldY>>1)*dmx
						            + t * dmx;
						int realY = ((sizeY) << 7) - (fieldX >> 1)*dy - (fieldY>>1)*dmy
						            + t * dmy;

						u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);

						if (a0 & 0x2000)
						{
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;
							int inc = 32;
							if (DISPCNT & 0x40)
								inc = sizeX >> 2;
							else
								c &= 0x3FE;
							for (int x = 0; x < fieldX; x++)
							{
								int xxx = realX >> 8;
								int yyy = realY >> 8;

								if (xxx < 0 || xxx >= sizeX ||
								    yyy < 0 || yyy >= sizeY ||
								    sx >= 240)
									;
								else
								{
									u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
									                             + ((yyy & 7)<<3) + ((xxx >> 3)<<6) +
									                             (xxx & 7))&0x7FFF)];
									if ((color == 0) && (((prio >> 25)&3) <
									                     ((lineOBJ[sx]>>25)&3)))
									{
										lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
										if ((a0 & 0x1000) && m)
											lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
									}
									else if ((color) && (prio < (lineOBJ[sx]&0xFF000000)))
									{
										lineOBJ[sx] = READ16LE(&spritePalette[color]) | prio;
										if ((a0 & 0x1000) && m)
											lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
									}

									if (a0 & 0x1000)
									{
										m++;
										if (m == mosaicX)
											m = 0;
									}
#ifdef SPRITE_DEBUG
									if (t == 0 || t == maskY || x == 0 || x == maskX)
										lineOBJ[sx] = 0x001F;
#endif
								}
								sx     = (sx+1)&511;;
								realX += dx;
								realY += dy;
							}
						}
						else
						{
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;

							int inc = 32;
							if (DISPCNT & 0x40)
								inc = sizeX >> 3;
							int palette = (a2 >> 8) & 0xF0;
							for (int x = 0; x < fieldX; x++)
							{
								int xxx = realX >> 8;
								int yyy = realY >> 8;
								if (xxx < 0 || xxx >= sizeX ||
								    yyy < 0 || yyy >= sizeY ||
								    sx >= 240)
									;
								else
								{
									u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
									                             + ((yyy & 7)<<2) + ((xxx >> 3)<<5) +
									                             ((xxx & 7)>>1))&0x7FFF)];
									if (xxx & 1)
										color >>= 4;
									else
										color &= 0x0F;

									if ((color == 0) && (((prio >> 25)&3) <
									                     ((lineOBJ[sx]>>25)&3)))
									{
										lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
										if ((a0 & 0x1000) && m)
											lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
									}
									else if ((color) && (prio < (lineOBJ[sx]&0xFF000000)))
									{
										lineOBJ[sx] = READ16LE(&spritePalette[palette+color]) | prio;
										if ((a0 & 0x1000) && m)
											lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
									}
								}
								if ((a0 & 0x1000) && m)
								{
									m++;
									if (m == mosaicX)
										m = 0;
								}

#ifdef SPRITE_DEBUG
								if (t == 0 || t == maskY || x == 0 || x == maskX)
									lineOBJ[sx] = 0x001F;
#endif
								sx     = (sx+1)&511;;
								realX += dx;
								realY += dy;
							}
						}
					}
				}
			}
			else
			{
				int t = VCOUNT - sy;
				if ((t >= 0) && (t < sizeY))
				{
					int sx = (a1 & 0x1FF);
					if (((sx < 240) || (((sx+sizeX)&511) < 240)) && !(a0 & 0x0200))
					{
						if (a0 & 0x2000)
						{
							if (a1 & 0x2000)
								t = sizeY - t - 1;
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;

							int inc = 32;
							if (DISPCNT & 0x40)
							{
								inc = sizeX >> 2;
							}
							else
							{
								c &= 0x3FE;
							}
							int xxx = 0;
							if (a1 & 0x1000)
								xxx = sizeX-1;

							if (a0 & 0x1000)
							{
								t -= (t % mosaicY);
							}

							int address = 0x10000 + ((((c+ (t>>3) * inc) << 5)
							                          + ((t & 7) << 3) + ((xxx>>3)<<6) + (xxx & 7)) & 0x7FFF);

							if (a1 & 0x1000)
								xxx = 7;
							u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);

							for (int xx = 0; xx < sizeX; xx++)
							{
								if (sx < 240)
								{
									u8 color = vram[address];
									if ((color == 0) && (((prio >> 25)&3) <
									                     ((lineOBJ[sx]>>25)&3)))
									{
										lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
										if ((a0 & 0x1000) && m)
											lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
									}
									else if ((color) && (prio < (lineOBJ[sx]&0xFF000000)))
									{
										lineOBJ[sx] = READ16LE(&spritePalette[color]) | prio;
										if ((a0 & 0x1000) && m)
											lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
									}

									if (a0 & 0x1000)
									{
										m++;
										if (m == mosaicX)
											m = 0;
									}

#ifdef SPRITE_DEBUG
									if (t == 0 || t == maskY || xx == 0 || xx == maskX)
										lineOBJ[sx] = 0x001F;
#endif
								}

								sx = (sx+1) & 511;
								if (a1 & 0x1000)
								{
									xxx--;
									address--;
									if (xxx == -1)
									{
										address -= 56;
										xxx      = 7;
									}
									if (address < 0x10000)
										address += 0x8000;
								}
								else
								{
									xxx++;
									address++;
									if (xxx == 8)
									{
										address += 56;
										xxx      = 0;
									}
									if (address > 0x17fff)
										address -= 0x8000;
								}
							}
						}
						else
						{
							if (a1 & 0x2000)
								t = sizeY - t - 1;
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;

							int inc = 32;
							if (DISPCNT & 0x40)
							{
								inc = sizeX >> 3;
							}
							int xxx = 0;
							if (a1 & 0x1000)
								xxx = sizeX - 1;

							if (a0 & 0x1000)
							{
								t -= (t % mosaicY);
							}

							int address = 0x10000 + ((((c + (t>>3) * inc)<<5)
							                          + ((t & 7)<<2) + ((xxx>>3)<<5) + ((xxx & 7) >> 1))&0x7FFF);
							u32 prio    = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
							int palette = (a2 >> 8) & 0xF0;
							if (a1 & 0x1000)
							{
								xxx = 7;
								for (int xx = sizeX - 1; xx >= 0; xx--)
								{
									if (sx < 240)
									{
										u8 color = vram[address];
										if (xx & 1)
										{
											color = (color >> 4);
										}
										else
											color &= 0x0F;

										if ((color == 0) && (((prio >> 25)&3) <
										                     ((lineOBJ[sx]>>25)&3)))
										{
											lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
											if ((a0 & 0x1000) && m)
												lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
										}
										else if ((color) && (prio < (lineOBJ[sx]&0xFF000000)))
										{
											lineOBJ[sx] = READ16LE(&spritePalette[palette + color]) | prio;
											if ((a0 & 0x1000) && m)
												lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
										}
									}
									if (a0 & 0x1000)
									{
										m++;
										if (m == mosaicX)
											m = 0;
									}
#ifdef SPRITE_DEBUG
									if (t == 0 || t == maskY || xx == 0 || xx == maskX)
										lineOBJ[sx] = 0x001F;
#endif
									sx = (sx+1) & 511;
									xxx--;
									if (!(xx & 1))
										address--;
									if (xxx == -1)
									{
										xxx      = 7;
										address -= 28;
									}
									if (address < 0x10000)
										address += 0x8000;
								}
							}
							else
							{
								for (int xx = 0; xx < sizeX; xx++)
								{
									if (sx < 240)
									{
										u8 color = vram[address];
										if (xx & 1)
										{
											color = (color >> 4);
										}
										else
											color &= 0x0F;

										if ((color == 0) && (((prio >> 25)&3) <
										                     ((lineOBJ[sx]>>25)&3)))
										{
											lineOBJ[sx] = (lineOBJ[sx] & 0xF9FFFFFF) | prio;
											if ((a0 & 0x1000) && m)
												lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
										}
										else if ((color) && (prio < (lineOBJ[sx]&0xFF000000)))
										{
											lineOBJ[sx] = READ16LE(&spritePalette[palette + color]) | prio;
											if ((a0 & 0x1000) && m)
												lineOBJ[sx] = (lineOBJ[sx-1] & 0xF9FFFFFF) | prio;
										}
									}
									if (a0 & 0x1000)
									{
										m++;
										if (m == mosaicX)
											m = 0;
									}
#ifdef SPRITE_DEBUG
									if (t == 0 || t == maskY || xx == 0 || xx == maskX)
										lineOBJ[sx] = 0x001F;
#endif
									sx = (sx+1) & 511;
									xxx++;
									if (xx & 1)
										address++;
									if (xxx == 8)
									{
										address += 28;
										xxx      = 0;
									}
									if (address > 0x17fff)
										address -= 0x8000;
								}
							}
						}
					}
				}
			}
		}
	}
}

inline void gfxDrawOBJWin(u32 *lineOBJWin)
{
	gfxClearArray(lineOBJWin);
	if (layerEnable & 0x8000)
	{
		u16 *sprites = (u16 *)oam;
		// u16 *spritePalette = &((u16 *)paletteRAM)[256];
		for (int x = 0; x < 128; x++)
		{
			u16 a0 = READ16LE(sprites++);
			u16 a1 = READ16LE(sprites++);
			u16 a2 = READ16LE(sprites++);
			sprites++;

			// ignore non OBJ-WIN
			if ((a0 & 0x0c00) != 0x0800)
				continue;

			int sizeY = 8;
			int sizeX = 8;

			switch (((a0 >>12) & 0x0c)|(a1>>14))
			{
			case 0:
				break;
			case 1:
				sizeX = sizeY = 16;
				break;
			case 2:
				sizeX = sizeY = 32;
				break;
			case 3:
				sizeX = sizeY = 64;
				break;
			case 4:
				sizeX = 16;
				break;
			case 5:
				sizeX = 32;
				break;
			case 6:
				sizeX = 32;
				sizeY = 16;
				break;
			case 7:
				sizeX = 64;
				sizeY = 32;
				break;
			case 8:
				sizeY = 16;
				break;
			case 9:
				sizeY = 32;
				break;
			case 10:
				sizeX = 16;
				sizeY = 32;
				break;
			case 11:
				sizeX = 32;
				sizeY = 64;
				break;
			default:
				continue;
			}

			int sy = (a0 & 255);

			if (sy > 160)
				sy -= 256;

			if (a0 & 0x0100)
			{
				int fieldX = sizeX;
				int fieldY = sizeY;
				if (a0 & 0x0200)
				{
					fieldX <<= 1;
					fieldY <<= 1;
				}

				int t = VCOUNT - sy;
				if ((t >= 0) && (t < fieldY))
				{
					int sx = (a1 & 0x1FF);
					if ((sx < 240) || (((sx + fieldX) & 511) < 240))
					{
						// int t2 = t - (fieldY >> 1);
						int  rot = (a1 >> 9) & 0x1F;
						u16 *OAM = (u16 *)oam;
						int  dx  = READ16LE(&OAM[3 + (rot << 4)]);
						if (dx & 0x8000)
							dx |= 0xFFFF8000;
						int dmx = READ16LE(&OAM[7 + (rot << 4)]);
						if (dmx & 0x8000)
							dmx |= 0xFFFF8000;
						int dy = READ16LE(&OAM[11 + (rot << 4)]);
						if (dy & 0x8000)
							dy |= 0xFFFF8000;
						int dmy = READ16LE(&OAM[15 + (rot << 4)]);
						if (dmy & 0x8000)
							dmy |= 0xFFFF8000;

						int realX = ((sizeX) << 7) - (fieldX >> 1)*dx - (fieldY>>1)*dmx
						            + t * dmx;
						int realY = ((sizeY) << 7) - (fieldX >> 1)*dy - (fieldY>>1)*dmy
						            + t * dmy;

						// u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);

						if (a0 & 0x2000)
						{
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;
							int inc = 32;
							if (DISPCNT & 0x40)
								inc = sizeX >> 2;
							else
								c &= 0x3FE;
							for (int x = 0; x < fieldX; x++)
							{
								int xxx = realX >> 8;
								int yyy = realY >> 8;

								if (xxx < 0 || xxx >= sizeX ||
								    yyy < 0 || yyy >= sizeY)
								{}
								else
								{
									u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
									                             + ((yyy & 7)<<3) + ((xxx >> 3)<<6) +
									                             (xxx & 7))&0x7fff)];
									if (color)
									{
										lineOBJWin[sx] = 1;
									}
								}
								sx     = (sx+1)&511;;
								realX += dx;
								realY += dy;
							}
						}
						else
						{
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;

							int inc = 32;
							if (DISPCNT & 0x40)
								inc = sizeX >> 3;
							// int palette = (a2 >> 8) & 0xF0;
							for (int x = 0; x < fieldX; x++)
							{
								int xxx = realX >> 8;
								int yyy = realY >> 8;

								//              if(x == 0 || x == (sizeX-1) ||
								//                 t == 0 || t == (sizeY-1)) {
								//                lineOBJ[sx] = 0x001F | prio;
								//              } else {
								if (xxx < 0 || xxx >= sizeX ||
								    yyy < 0 || yyy >= sizeY)
								{}
								else
								{
									u32 color = vram[0x10000 + ((((c + (yyy>>3) * inc)<<5)
									                             + ((yyy & 7)<<2) + ((xxx >> 3)<<5) +
									                             ((xxx & 7)>>1))&0x7fff)];
									if (xxx & 1)
										color >>= 4;
									else
										color &= 0x0F;

									if (color)
									{
										lineOBJWin[sx] = 1;
									}
								}
								//            }
								sx     = (sx+1)&511;;
								realX += dx;
								realY += dy;
							}
						}
					}
				}
			}
			else
			{
				int t = VCOUNT - sy;
				if ((t >= 0) && (t < sizeY))
				{
					int sx = (a1 & 0x1FF);
					if (((sx < 240) || (((sx+sizeX)&511) < 240)) && !(a0 & 0x0200))
					{
						if (a0 & 0x2000)
						{
							if (a1 & 0x2000)
								t = sizeY - t - 1;
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;

							int inc = 32;
							if (DISPCNT & 0x40)
							{
								inc = sizeX >> 2;
							}
							else
							{
								c &= 0x3FE;
							}
							int xxx = 0;
							if (a1 & 0x1000)
								xxx = sizeX-1;
							int address = 0x10000 + ((((c+ (t>>3) * inc) << 5)
							                          + ((t & 7) << 3) + ((xxx>>3)<<6) + (xxx & 7))&0x7fff);
							if (a1 & 0x1000)
								xxx = 7;
							// u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
							for (int xx = 0; xx < sizeX; xx++)
							{
								if (sx < 240)
								{
									u8 color = vram[address];
									if (color)
									{
										lineOBJWin[sx] = 1;
									}
								}

								sx = (sx+1) & 511;
								if (a1 & 0x1000)
								{
									xxx--;
									address--;
									if (xxx == -1)
									{
										address -= 56;
										xxx      = 7;
									}
									if (address < 0x10000)
										address += 0x8000;
								}
								else
								{
									xxx++;
									address++;
									if (xxx == 8)
									{
										address += 56;
										xxx      = 0;
									}
									if (address > 0x17fff)
										address -= 0x8000;
								}
							}
						}
						else
						{
							if (a1 & 0x2000)
								t = sizeY - t - 1;
							int c = (a2 & 0x3FF);
							if ((DISPCNT & 7) > 2 && (c < 512))
								continue;

							int inc = 32;
							if (DISPCNT & 0x40)
							{
								inc = sizeX >> 3;
							}
							int xxx = 0;
							if (a1 & 0x1000)
								xxx = sizeX - 1;
							int address = 0x10000 + ((((c + (t>>3) * inc)<<5)
							                          + ((t & 7)<<2) + ((xxx>>3)<<5) + ((xxx & 7) >> 1))&0x7fff);
							// u32 prio = (((a2 >> 10) & 3) << 25) | ((a0 & 0x0c00)<<6);
							// int palette = (a2 >> 8) & 0xF0;
							if (a1 & 0x1000)
							{
								xxx = 7;
								for (int xx = sizeX - 1; xx >= 0; xx--)
								{
									if (sx < 240)
									{
										u8 color = vram[address];
										if (xx & 1)
										{
											color = (color >> 4);
										}
										else
											color &= 0x0F;

										if (color)
										{
											lineOBJWin[sx] = 1;
										}
									}
									sx = (sx+1) & 511;
									xxx--;
									if (!(xx & 1))
										address--;
									if (xxx == -1)
									{
										xxx      = 7;
										address -= 28;
									}
									if (address < 0x10000)
										address += 0x8000;
								}
							}
							else
							{
								for (int xx = 0; xx < sizeX; xx++)
								{
									if (sx < 240)
									{
										u8 color = vram[address];
										if (xx & 1)
										{
											color = (color >> 4);
										}
										else
											color &= 0x0F;

										if (color)
										{
											lineOBJWin[sx] = 1;
										}
									}
									sx = (sx+1) & 511;
									xxx++;
									if (xx & 1)
										address++;
									if (xxx == 8)
									{
										address += 28;
										xxx      = 0;
									}
									if (address > 0x17fff)
										address -= 0x8000;
								}
							}
						}
					}
				}
			}
		}
	}
}

inline u32 gfxIncreaseBrightness(u32 color, int coeff)
{
	int r = (color & 0x1F);
	int g = ((color >> 5) & 0x1F);
	int b = ((color >> 10) & 0x1F);

	r = r + (((31 - r) * coeff) >> 4);
	g = g + (((31 - g) * coeff) >> 4);
	b = b + (((31 - b) * coeff) >> 4);
	if (r > 31)
		r = 31;
	if (g > 31)
		g = 31;
	if (b > 31)
		b = 31;
	color = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
	return color;
}

inline void gfxIncreaseBrightness(u32 *line, int coeff)
{
	for (int x = 0; x < 240; x++)
	{
		u32 color = *line;
		int r     = (color & 0x1F);
		int g     = ((color >> 5) & 0x1F);
		int b     = ((color >> 10) & 0x1F);

		r = r + (((31 - r) * coeff) >> 4);
		g = g + (((31 - g) * coeff) >> 4);
		b = b + (((31 - b) * coeff) >> 4);
		if (r > 31)
			r = 31;
		if (g > 31)
			g = 31;
		if (b > 31)
			b = 31;
		*line++ = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
	}
}

inline u32 gfxDecreaseBrightness(u32 color, int coeff)
{
	int r = (color & 0x1F);
	int g = ((color >> 5) & 0x1F);
	int b = ((color >> 10) & 0x1F);

	r = r - ((r * coeff) >> 4);
	g = g - ((g * coeff) >> 4);
	b = b - ((b * coeff) >> 4);
	if (r < 0)
		r = 0;
	if (g < 0)
		g = 0;
	if (b < 0)
		b = 0;
	color = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;

	return color;
}

inline void gfxDecreaseBrightness(u32 *line, int coeff)
{
	for (int x = 0; x < 240; x++)
	{
		u32 color = *line;
		int r     = (color & 0x1F);
		int g     = ((color >> 5) & 0x1F);
		int b     = ((color >> 10) & 0x1F);

		r = r - ((r * coeff) >> 4);
		g = g - ((g * coeff) >> 4);
		b = b - ((b * coeff) >> 4);
		if (r < 0)
			r = 0;
		if (g < 0)
			g = 0;
		if (b < 0)
			b = 0;
		*line++ = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
	}
}

inline u32 gfxAlphaBlend(u32 color, u32 color2, int ca, int cb)
{
	if (color < 0x80000000)
	{
		int r  = (color & 0x1F);
		int g  = ((color >> 5) & 0x1F);
		int b  = ((color >> 10) & 0x1F);
		int r0 = (color2 & 0x1F);
		int g0 = ((color2 >> 5) & 0x1F);
		int b0 = ((color2 >> 10) & 0x1F);

		r = ((r * ca) >> 4) + ((r0 * cb) >> 4);
		g = ((g * ca) >> 4) + ((g0 * cb) >> 4);
		b = ((b * ca) >> 4) + ((b0 * cb) >> 4);

		if (r > 31)
			r = 31;
		if (g > 31)
			g = 31;
		if (b > 31)
			b = 31;

		return (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
	}
	return color;
}

inline void gfxAlphaBlend(u32 *ta, u32 *tb, int ca, int cb)
{
	for (int x = 0; x < 240; x++)
	{
		u32 color = *ta;
		if (color < 0x80000000)
		{
			int r      = (color & 0x1F);
			int g      = ((color >> 5) & 0x1F);
			int b      = ((color >> 10) & 0x1F);
			u32 color2 = (*tb++);
			int r0     = (color2 & 0x1F);
			int g0     = ((color2 >> 5) & 0x1F);
			int b0     = ((color2 >> 10) & 0x1F);

			r = ((r * ca) >> 4) + ((r0 * cb) >> 4);
			g = ((g * ca) >> 4) + ((g0 * cb) >> 4);
			b = ((b * ca) >> 4) + ((b0 * cb) >> 4);

			if (r > 31)
				r = 31;
			if (g > 31)
				g = 31;
			if (b > 31)
				b = 31;

			*ta++ = (color & 0xFFFF0000) | (b << 10) | (g << 5) | r;
		}
		else
		{
			ta++;
			tb++;
		}
	}
}

#endif // VBA_GBA_GFX_H
