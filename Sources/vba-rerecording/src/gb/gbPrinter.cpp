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

#include <stdio.h>
#include <memory.h>
#include "../common/System.h"
#include "gbPrinter.h"

u8  gbPrinterStatus = 0;
int gbPrinterState  = 0;
u8  gbPrinterData[0x280*9];
u8  gbPrinterPacket[0x400];
int gbPrinterCount     = 0;
int gbPrinterDataCount = 0;
int gbPrinterDataSize  = 0;
int gbPrinterResult    = 0;

bool gbPrinterCheckCRC()
{
	u16 crc = 0;

	for (int i = 2; i < (6+gbPrinterDataSize); i++)
	{
		crc += gbPrinterPacket[i];
	}

	int msgCrc = gbPrinterPacket[6+gbPrinterDataSize] +
	             (gbPrinterPacket[7+gbPrinterDataSize]<<8);

	return msgCrc == crc;
}

void gbPrinterReset()
{
	gbPrinterState     = 0;
	gbPrinterDataSize  = 0;
	gbPrinterDataCount = 0;
	gbPrinterCount     = 0;
	gbPrinterStatus    = 0;
	gbPrinterResult    = 0;
}

void gbPrinterShowData()
{
	systemGbPrint(gbPrinterData,
	              gbPrinterPacket[6],
	              gbPrinterPacket[7],
	              gbPrinterPacket[8],
	              gbPrinterPacket[9]);
	/*
	   allegro_init();
	   install_keyboard();
	   set_gfx_mode(GFX_AUTODETECT, 160, 144, 0, 0);
	   PALETTE pal;
	   pal[0].r = 255;
	   pal[0].g = 255;
	   pal[0].b = 255;
	   pal[1].r = 168;
	   pal[1].g = 168;
	   pal[1].b = 168;
	   pal[2].r = 96;
	   pal[2].g = 96;
	   pal[2].b = 96;
	   pal[3].r = 0;
	   pal[3].g = 0;
	   pal[3].b = 0;
	   set_palette(pal);
	   acquire_screen();
	   u8 *data = gbPrinterData;
	   for(int y = 0; y < 0x12; y++) {
	   for(int x = 0; x < 0x14; x++) {
	    for(int k = 0; k < 8; k++) {
	      int a = *data++;
	      int b = *data++;
	      for(int j = 0; j < 8; j++) {
	        int mask = 1 << (7-j);
	        int c = 0;
	        if(a & mask)
	          c++;
	        if(b & mask)
	          c+=2;
	        putpixel(screen, x*8+j, y*8+k, c);
	      }
	    }
	   }
	   }
	   release_screen();
	   while(!keypressed()) {
	   }
	 */
}

void gbPrinterReceiveData()
{
	if (gbPrinterPacket[3]) // compressed
	{
		u8 *data = &gbPrinterPacket[6];
		u8 *dest = &gbPrinterData[gbPrinterDataCount];
		int len  = 0;
		while (len < gbPrinterDataSize)
		{
			u8 control = *data++;
			if (control & 0x80) // repeated data
			{
				control &= 0x7f;
				control += 2;
				memset(dest, *data++, control);
				len  += control;
				dest += control;
			}
			else // raw data
			{
				control++;
				memcpy(dest, data, control);
				dest += control;
				data += control;
				len  += control;
			}
		}
	}
	else
	{
		memcpy(&gbPrinterData[gbPrinterDataCount],
		       &gbPrinterPacket[6],
		       gbPrinterDataSize);
		gbPrinterDataCount += gbPrinterDataSize;
	}
}

void gbPrinterCommand()
{
	switch (gbPrinterPacket[2])
	{
	case 0x01:
		// reset/initialize packet
		gbPrinterDataCount = 0;
		gbPrinterStatus    = 0;
		break;
	case 0x02:
		// print packet
		gbPrinterShowData();
		break;
	case 0x04:
		// data packet
		gbPrinterReceiveData();
		break;
	case 0x0f:
		// NUL packet
		break;
	}
}

u8 gbPrinterSend(u8 byte)
{
	switch (gbPrinterState)
	{
	case 0:
		gbPrinterCount = 0;
		// receiving preamble
		if (byte == 0x88)
		{
			gbPrinterPacket[gbPrinterCount++] = byte;
			gbPrinterState++;
		}
		else
		{
			// todo: handle failure
			gbPrinterReset();
		}
		break;
	case 1:
		// receiving preamble
		if (byte == 0x33)
		{
			gbPrinterPacket[gbPrinterCount++] = byte;
			gbPrinterState++;
		}
		else
		{
			// todo: handle failure
			gbPrinterReset();
		}
		break;
	case 2:
		// receiving header
		gbPrinterPacket[gbPrinterCount++] = byte;
		if (gbPrinterCount == 6)
		{
			gbPrinterState++;
			gbPrinterDataSize = gbPrinterPacket[4] + (gbPrinterPacket[5]<<8);
		}
		break;
	case 3:
		// receiving data
		if (gbPrinterDataSize)
		{
			gbPrinterPacket[gbPrinterCount++] = byte;
			if (gbPrinterCount == (6+gbPrinterDataSize))
			{
				gbPrinterState++;
			}
			break;
		}
		gbPrinterState++;
	// intentionally move to next if no data to receive
	case 4:
		// receiving CRC
		gbPrinterPacket[gbPrinterCount++] = byte;
		gbPrinterState++;
		break;
	case 5:
		// receiving CRC-2
		gbPrinterPacket[gbPrinterCount++] = byte;
		if (gbPrinterCheckCRC())
		{
			gbPrinterCommand();
		}
		gbPrinterState++;
		break;
	case 6:
		// receiving dummy 1
		gbPrinterPacket[gbPrinterCount++] = byte;
		gbPrinterResult = 0x81;
		gbPrinterState++;
		break;
	case 7:
		// receiving dummy 2
		gbPrinterPacket[gbPrinterCount++] = byte;
		gbPrinterResult = gbPrinterStatus;
		gbPrinterState  = 0;
		gbPrinterCount  = 0;
		break;
	}
	return gbPrinterResult;
}

