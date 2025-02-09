/**********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002  Gary Henderson (gary.henderson@ntlworld.com),
                             Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2002 - 2004  Matthew Kendora

  (c) Copyright 2002 - 2005  Peter Bortas (peter@bortas.org)

  (c) Copyright 2004 - 2005  Joel Yliluoma (http://iki.fi/bisqwit/)

  (c) Copyright 2001 - 2006  John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2006  funkyass (funkyass@spam.shaw.ca),
                             Kris Bleakley (codeviolation@hotmail.com)

  (c) Copyright 2002 - 2007  Brad Jorsch (anomie@users.sourceforge.net),
                             Nach (n-a-c-h@users.sourceforge.net),
                             zones (kasumitokoduck@yahoo.com)

  (c) Copyright 2006 - 2007  nitsuja


  BS-X C emulator code
  (c) Copyright 2005 - 2006  Dreamer Nom,
                             zones

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003  _Demo_ (_demo_@zsnes.com),
                             Nach,
                             zsKnight (zsknight@zsnes.com)

  C4 C++ code
  (c) Copyright 2003 - 2006  Brad Jorsch,
                             Nach

  DSP-1 emulator code
  (c) Copyright 1998 - 2006  _Demo_,
                             Andreas Naive (andreasnaive@gmail.com)
                             Gary Henderson,
                             Ivar (ivar@snes9x.com),
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora,
                             Nach,
                             neviksti (neviksti@hotmail.com)

  DSP-2 emulator code
  (c) Copyright 2003         John Weidman,
                             Kris Bleakley,
                             Lord Nightmare (lord_nightmare@users.sourceforge.net),
                             Matthew Kendora,
                             neviksti


  DSP-3 emulator code
  (c) Copyright 2003 - 2006  John Weidman,
                             Kris Bleakley,
                             Lancer,
                             z80 gaiden

  DSP-4 emulator code
  (c) Copyright 2004 - 2006  Dreamer Nom,
                             John Weidman,
                             Kris Bleakley,
                             Nach,
                             z80 gaiden

  OBC1 emulator code
  (c) Copyright 2001 - 2004  zsKnight,
                             pagefault (pagefault@zsnes.com),
                             Kris Bleakley,
                             Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code
  (c) Copyright 2002         Matthew Kendora with research by
                             zsKnight,
                             John Weidman,
                             Dark Force

  S-DD1 C emulator code
  (c) Copyright 2003         Brad Jorsch with research by
                             Andreas Naive,
                             John Weidman

  S-RTC C emulator code
  (c) Copyright 2001-2006    byuu,
                             John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003         Feather,
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003  _Demo_,
                             pagefault,
                             zsKnight,

  Super FX C emulator code
  (c) Copyright 1997 - 1999  Ivar,
                             Gary Henderson,
                             John Weidman

  Sound DSP emulator code is derived from SNEeSe and OpenSPC:
  (c) Copyright 1998 - 2003  Brad Martin
  (c) Copyright 1998 - 2006  Charles Bilyue'

  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004  Marcus Comstedt (marcus@mc.pp.se)

  2xSaI filter
  (c) Copyright 1999 - 2001  Derek Liauw Kie Fa

  HQ2x, HQ3x, HQ4x filters
  (c) Copyright 2003         Maxim Stepin (maxim@hiend3d.com)

  Win32 GUI code
  (c) Copyright 2003 - 2006  blip,
                             funkyass,
                             Matthew Kendora,
                             Nach,
                             nitsuja

  Mac OS GUI code
  (c) Copyright 1998 - 2001  John Stiles
  (c) Copyright 2001 - 2007  zones


  Specific ports contains the works of other authors. See headers in
  individual files.


  Snes9x homepage: http://www.snes9x.com

  Permission to use, copy, modify and/or distribute Snes9x in both binary
  and source form, for non-commercial purposes, is hereby granted without
  fee, providing that this license information and copyright notice appear
  with all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software or it's derivatives.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes,
  but is not limited to, charging money for Snes9x or software derived from
  Snes9x, including Snes9x or derivatives in commercial game bundles, and/or
  using Snes9x as a promotion for your commercial product.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
**********************************************************************************/



#ifndef _CHEATS_H_
#define _CHEATS_H_

struct SCheat
{
    uint32  address;
    uint8   byte;
    uint8   saved_byte;
    bool8   enabled;
    bool8   saved;
    char    name [22];
};

#define MAX_CHEATS 150

struct SCheatData
{
    struct SCheat   c [MAX_CHEATS];
    uint32	    num_cheats;
    uint8	    CWRAM [0x20000];
    uint8	    CSRAM [0x10000];
    uint8	    CIRAM [0x2000];
    uint8           *RAM;
    uint8           *FillRAM;
    uint8           *SRAM;
	uint32		ALL_BITS [(0x32000 >> 5)];
#define			WRAM_BITS					ALL_BITS
#define			SRAM_BITS					ALL_BITS + (0x20000 >> 5)
#define			IRAM_BITS					ALL_BITS + (0x30000 >> 5)
    uint8	    CWatchRAM [0x32000];
};


struct Watch {
	bool on;
	int size;
	int format;
	uint32 address;
	char buf[12];
	char desc[256];
};
#define MAX_WATCH_COUNT_S9X 256
extern Watch watches [MAX_WATCH_COUNT_S9X];

typedef enum
{
    S9X_LESS_THAN, S9X_GREATER_THAN, S9X_LESS_THAN_OR_EQUAL,
    S9X_GREATER_THAN_OR_EQUAL, S9X_EQUAL, S9X_NOT_EQUAL
} S9xCheatComparisonType;

typedef enum
{
    S9X_8_BITS, S9X_16_BITS, S9X_24_BITS, S9X_32_BITS
} S9xCheatDataSize;

void S9xInitCheatData ();

const char *S9xGameGenieToRaw (const char *code, uint32 &address, uint8 &byte);
const char *S9xProActionReplayToRaw (const char *code, uint32 &address, uint8 &byte);
const char *S9xGoldFingerToRaw (const char *code, uint32 &address, bool8 &sram,
				uint8 &num_bytes, uint8 bytes[3]);
void S9xApplyCheats ();
void S9xApplyCheat (uint32 which1);
void S9xRemoveCheats ();
void S9xRemoveCheat (uint32 which1);
void S9xEnableCheat (uint32 which1);
void S9xDisableCheat (uint32 which1);
void S9xAddCheat (bool8 enable, bool8 save_current_value, uint32 address,
		  uint8 byte);
void S9xDeleteCheats ();
void S9xDeleteCheat (uint32 which1);
bool8 S9xLoadCheatFile (const char *filename);
bool8 S9xSaveCheatFile (const char *filename);

void S9xStartCheatSearch (SCheatData *);
void S9xSearchForChange (SCheatData *, S9xCheatComparisonType cmp,
                         S9xCheatDataSize size, bool8 is_signed, bool8 update);
void S9xSearchForValue (SCheatData *, S9xCheatComparisonType cmp,
                        S9xCheatDataSize size, uint32 value,
                        bool8 is_signed, bool8 update);
void S9xSearchForAddress (SCheatData *, S9xCheatComparisonType cmp,
                        S9xCheatDataSize size, uint32 address, bool8 update);
void S9xOutputCheatSearchResults (SCheatData *);

#ifdef __linux
#include <ctype.h>
#endif
template<typename IntType>
int ScanAddress(const char* str, IntType& value)
{
	int ret = 0;
	if(tolower(*str) == 's')
	{
		ret = sscanf(str+1, "%x", &value);
		value += 0x7E0000 + 0x20000;
	}
	else if(tolower(*str) == 'i')
	{
		ret = sscanf(str+1, "%x", &value);
		value += 0x7E0000 + 0x30000;
	}
	else
	{
		int plus = (*str == '0' && tolower(str[1]) == 'x') ? 2 : 0;
		ret = sscanf(str+plus, "%x", &value);
	}
	return ret;
}

#endif

