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



//  Win32 GUI code
//  (c) Copyright 2003-2006 blip, Nach, Matthew Kendora, funkyass and nitsuja

// all windows-specific command line and config file parsing/saving/loading
// this stuff was moved out of wsnes.cpp, to keep things a little tidier

// note:
//  if you want to force all users of a new version to have a
//  particular setting reset to its given default,
//  change the name string of that setting (in WinRegisterConfigItems)
//  to something a little different...
//  but if it's not in a windows-specific category, make sure you change its name elsewhere too

#include "../port.h"
#include "../snes9x.h"
#include "wsnes9x.h"
#include "wlanguage.h"
#include "lazymacro.h"
#include "../display.h"
#include "../conffile.h"
#include "../spc7110.h"
#include "../gfx.h"
#include "../snapshot.h"
#include "../s9xlua.h"
#ifdef NETPLAY_SUPPORT
	#include "../netplay.h"
	extern SNPServer NPServer;
#endif
#include <assert.h>

static void WinDeleteRegistryEntries ();
void WinSetDefaultValues ();
void WinDeleteRecentGamesList ();

HANDLE configMutex = NULL;

void S9xParseArg (char **argv, int &i, int argc)
{
	if (strcasecmp (argv [i], "-restore") == 0)
	{
		WinDeleteRegistryEntries ();
		WinSetDefaultValues	();
	}
	if (strcasecmp (argv[i], "-hidemenu") == 0)
	{
		GUI.HideMenu = true;
	}
}

extern char multiRomA [MAX_PATH]; // lazy, should put in sGUI and add init to {0} somewhere
extern char multiRomB [MAX_PATH];

void WinSetDefaultValues ()
{
	// TODO: delete the parts that are already covered by the default values in WinRegisterConfigItems

	char temp[4];
	strcpy(temp,"C:\\");

	GUI.ControllerOption = SNES_JOYPAD;
	GUI.ValidControllerOptions = 0xFFFF;
	GUI.IgnoreNextMouseMove	= false;

	GUI.HideMenu = false;
	GUI.window_size.left = 0;
	GUI.window_size.right =	524;
	GUI.window_size.top	= 0;
	GUI.window_size.bottom = 524;
	GUI.Width =	640;
	GUI.Height = 480;
	GUI.Depth =	16;
	GUI.Scale =	FILTER_NONE;
	GUI.NextScale =	FILTER_NONE;
	GUI.ScaleHiRes =	FILTER_NONE;
	GUI.NextScaleHiRes =	FILTER_NONE;
	GUI.FullScreen = false;
	GUI.Stretch	= false;
	//GUI.PausedFramesBeforeMutingSound =	20;
	GUI.FlipCounter	= 0;
	GUI.NumFlipFrames =	1;
	GUI.ddrawUseVideoMemory	= false;
	GUI.tripleBuffering = false;
	GUI.ScreenCleared =	true;
	GUI.LockDirectories = false;
	GUI.window_maximized = false;

	WinDeleteRecentGamesList ();

	// ROM Options
	memset (&Settings, 0, sizeof (Settings));

	Settings.ForceLoROM	= false;
	Settings.ForceInterleaved =	false;

	Settings.ForceNotInterleaved = false;
	Settings.ForceInterleaved =	false;
	Settings.ForceInterleaved2 = false;

	Settings.ForcePAL =	false;
	Settings.ForceNTSC = false;
	Settings.ForceHeader = false;
	Settings.ForceNoHeader = false;

	// Sound options
	Settings.SoundSync = FALSE;
	Settings.InterpolatedSound = TRUE;
	Settings.SoundEnvelopeHeightReading	= TRUE;
	Settings.DisableSoundEcho =	FALSE;
	Settings.DisableMasterVolume = FALSE;
	Settings.Mute =	FALSE;
	Settings.SoundSkipMethod = 0;
	Settings.SoundPlaybackRate = 32000;
	Settings.SixteenBitSound = TRUE;
	Settings.Stereo	= TRUE;
//	Settings.AltSampleDecode = FALSE;
	Settings.ReverseStereo = FALSE;
	Settings.SoundDriver = WIN_SNES9X_DIRECT_SOUND_DRIVER;
	Settings.SoundBufferSize = 4;
	Settings.SoundMixInterval =	20;
//	Settings.DisableSampleCaching=TRUE;
	GUI.SoundChannelEnable=255;
	GUI.FAMute	= FALSE;
	GUI.NotifySoundDSPRead = FALSE;

	// Tracing options
	Settings.TraceDMA =	false;
	Settings.TraceHDMA = false;
	Settings.TraceVRAM = false;
	Settings.TraceUnknownRegisters = false;
	Settings.TraceDSP =	false;

	// Joystick	options
//	Settings.SwapJoypads = false;
	Settings.JoystickEnabled = false;

	// ROM timing options (see also	H_Max above)
	Settings.PAL = false;
	Settings.FrameTimePAL =	20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.FrameTime = !Settings.PAL ? Settings.FrameTimeNTSC : Settings.FrameTimePAL;

	// CPU options
	Settings.HDMATimingHack = 100;
	Settings.Shutdown =	false;
	Settings.ShutdownMaster	= false;
	Settings.BlockInvalidVRAMAccess = true;
	Settings.NextAPUEnabled	= Settings.APUEnabled =	TRUE;
	Settings.DisableIRQ	= false;
	Settings.Paused	= false;
	Timings.H_Max =	SNES_CYCLES_PER_SCANLINE;
	Timings.HBlankStart	= (256 * Timings.H_Max)	/ SNES_HCOUNTER_MAX;
	Settings.SkipFrames	= AUTO_FRAMERATE;

	// ROM image and peripheral	options
	Settings.ForceSuperFX =	false;
	Settings.ForceNoSuperFX	= false;
	Settings.MultiPlayer5Master	= false;
	Settings.SuperScopeMaster =	false;
	Settings.MouseMaster = false;
	Settings.SuperFX = false;

	// SNES	graphics options
	Settings.BGLayering	= false;
	Settings.DisableGraphicWindows = false;
	Settings.ForceTransparency = false;
	Settings.ForceNoTransparency = false;
	Settings.DisableHDMA = false;
	Settings.Mode7Interpolate =	false;
	GUI.HeightExtend = false;
	Settings.DisplayFrameRate =	false;
//	Settings.SixteenBit =	true;
	Settings.Transparency =	true;
	Settings.SupportHiRes =	true;
	Settings.AutoDisplayMessages = false;
	GUI.MessagesInImage = false; // this port supports text display on post-rendered surface

	Settings.DisplayPressedKeys	= 0;
	GUI.CurrentSaveSlot = 0;
	Settings.AutoSaveDelay = 15;
	Settings.ApplyCheats = true;

	Settings.TurboMode = false;
	Settings.TurboSkipFrames = 15;
	Settings.AutoMaxSkipFrames = 1;

#ifdef NETPLAY_SUPPORT
	Settings.Port =	1996;
	NetPlay.MaxFrameSkip = 10;
	NetPlay.MaxBehindFrameCount	= 10;
	NPServer.SyncByReset = true;
	NPServer.SendROMImageOnConnect = false;
#endif

	GUI.FreezeFileDir [0] =	0;
	Settings.SampleCatchup=false;
	Settings.TakeScreenshot=false;

	GUI.EmulatedFullScreen = false;

	GUI.AVIDoubleScale = false;

	GUI.MacroInputMode = MACRO_INPUT_MOV;
	GUI.PauseWithMacro = false;
	GUI.SaveMacroSnap = true;
	GUI.PlatformSnapIntoTempDir = true;
	for(int i = 0; i < MAX_RECENT_MACROS_LIST_SIZE; i++)
		GUI.RecentMacros[i][0] = '\0';

	GUI.Language=0;

	InitCustomKeys(&CustomKeys);
}


static bool	try_save(const char	*fname,	ConfigFile &conf){
	STREAM fp;
	if((fp=OPEN_STREAM(fname, "w"))!=NULL){
		fprintf(stdout,	"Saving	config file	%s\n", fname);
		CLOSE_STREAM(fp);
		conf.SaveTo(fname);
		return true;
	}
	return false;
}

bool using_conf_not_cfg = false;
static char rom_filename [MAX_PATH] = {0,};

static bool S9xSaveConfigFile(ConfigFile &conf){

	configMutex = CreateMutex(NULL, FALSE, "Snes9xConfigMutex");
	int times = 0;
	DWORD waitVal = WAIT_TIMEOUT;
	while(waitVal == WAIT_TIMEOUT && ++times <= 150) // wait at most 15 seconds
		waitVal = WaitForSingleObject(configMutex, 100);

	// save over the .conf file if it already exists, otherwise save over the .cfg file
	std::string	fname;
	fname=S9xGetDirectory(DEFAULT_DIR);
	fname+=SLASH_STR "snes9x.conf";
	STREAM fp;
	if((fp=OPEN_STREAM(fname.c_str(), "r"))!=NULL){
		CLOSE_STREAM(fp);
		using_conf_not_cfg = true;
	} else {
		fname=S9xGetDirectory(DEFAULT_DIR);
		fname+=SLASH_STR "snes9x.cfg";
		using_conf_not_cfg = false;
	}

	// ensure previous config file is not lost if we crash while writing the new one
	std::string	ftemp;
	{
		CopyFile(fname.c_str(), (fname + ".autobak").c_str(), FALSE);

		ftemp=S9xGetDirectory(DEFAULT_DIR);
		ftemp+=SLASH_STR "config_error";
		FILE* tempfile = fopen(ftemp.c_str(), "wb");
		if(tempfile) fclose(tempfile);
	}

	bool ret = try_save(fname.c_str(), conf);

	remove(ftemp.c_str());
	remove((fname + ".autobak").c_str());

	ReleaseMutex(configMutex);
	CloseHandle(configMutex);

	return ret;
}


static void	WinDeleteRegistryEntries ()
{
//	WinDeleteRegKey (HKEY_CURRENT_USER, S9X_REG_KEY_BASE);
}

static inline char *SkipSpaces (char *p)
{
	while (*p && isspace (*p))
		p++;

	return (p);
}

const char*	WinParseCommandLineAndLoadConfigFile (char *line)
{
	// Break the command line up into an array of string pointers, each	pointer
	// points at a separate	word or	character sequence enclosed	in quotes.

#define	MAX_PARAMETERS 100
	char *p	= line;
	static char	*parameters	[MAX_PARAMETERS];
	int	count =	0;

	parameters [count++] = "Snes9XW";

	while (count < MAX_PARAMETERS && *p)
	{
		p =	SkipSpaces (p);
		if (*p == '"')
		{
			p++;
			parameters [count++] = p;
			while (*p && *p	!= '"')
				p++;
			*p++ = 0;
		}
		else
			if (*p == '\'')
			{
				p++;
				parameters [count++] = p;
				while (*p && *p	!= '\'')
					p++;
				*p++ = 0;
			}
			else
			{
				parameters [count++] = p;
				while (*p && !isspace (*p))
					p++;
				if (!*p)
					break;
				*p++ = 0;
			}
	}

	configMutex = CreateMutex(NULL, FALSE, "Snes9xConfigMutex");
	int times = 0;
	DWORD waitVal = WAIT_TIMEOUT;
	while(waitVal == WAIT_TIMEOUT && ++times <= 150) // wait at most 15 seconds
		waitVal = WaitForSingleObject(configMutex, 100);

	// ensure previous config file is not lost if we crashed while writing a new one
	{
		std::string	ftemp;
		ftemp=S9xGetDirectory(DEFAULT_DIR);
		ftemp+=SLASH_STR "config_error";
		FILE* tempfile = fopen(ftemp.c_str(), "rb");
		if(tempfile)
		{
			fclose(tempfile);

			std::string	fname;
			for(int i=0; i<2; i++)
			{
				fname=S9xGetDirectory(DEFAULT_DIR);
				if(i == 0)      fname+=SLASH_STR "snes9x.conf";
				else if(i == 1) fname+=SLASH_STR "snes9x.cfg";

				tempfile = fopen((fname + ".autobak").c_str(), "rb");
				if(tempfile)
				{
					fclose(tempfile);
					MoveFileEx((fname + ".autobak").c_str(), fname.c_str(), MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
				}
			}
		  remove(ftemp.c_str());
		}
	}

	S9xLoadConfigFiles(parameters, count);

	ReleaseMutex(configMutex);
	CloseHandle(configMutex);

	const char* rf = S9xParseArgs (parameters, count);
	if(rf)
		strcpy(rom_filename, rf);
	else
		rom_filename[0] = '\0';

	return rf;
}

#define S(x) GAMEDEVICE_VK_##x
#define SO(x) GAMEDEVICE_VK_OEM_##x
static const char* keyToString [256] =
{
	"Unassigned","LMB","RMB","Break","MMB","XMB1","XMB2","0x07",
	S(BACK),S(TAB),"0x0A","0x0B",S(CLEAR),S(RETURN),"0x0E","0x0F",
	S(SHIFT),S(CONTROL),S(MENU),S(PAUSE),S(CAPITAL),"Kana","0x16","Junja",
	"Final","Kanji","0x1A",S(ESCAPE),"Convert","NonConvert","Accept","ModeChange",
	S(SPACE),S(PRIOR),S(NEXT),S(END),S(HOME),S(LEFT),S(UP),S(RIGHT),
	S(DOWN),S(SELECT),S(PRINT),S(EXECUTE),S(SNAPSHOT),S(INSERT),S(DELETE),S(HELP),
	"0","1","2","3","4","5","6","7",
	"8","9","0x3A","0x3B","0x3C","0x3D","0x3E","0x3F",
	"0x40","A","B","C","D","E","F","G",
	"H","I","J","K","L","M","N","O",
	"P","Q","R","S","T","U","V","W",
	"X","Y","Z",S(LWIN),S(RWIN),S(APPS),"0x5E","Sleep",
	"Pad0","Pad1","Pad2","Pad3","Pad4","Pad5","Pad6","Pad7",
	"Pad8","Pad9",S(MULTIPLY),S(ADD),S(SEPARATOR),S(SUBTRACT),S(DECIMAL),S(DIVIDE),
	S(F1),S(F2),S(F3),S(F4),S(F5),S(F6),S(F7),S(F8),
	S(F9),S(F10),S(F11),S(F12),"F13","F14","F15","F16",
	"F17","F18","F19","F20","F21","F22","F23","F24",
	"0x88","0x89","0x8A","0x8B","0x8C","0x8D","0x8E","0x8F",
	S(NUMLOCK),S(SCROLL),"PadEqual","Masshou","Touroku","Loya","Roya","0x97",
	"0x98","0x99","0x9A","0x9B","0x9C","0x9D","0x9E","0x9F",
	S(LSHIFT),S(RSHIFT),S(LCONTROL),S(RCONTROL),S(LMENU),S(RMENU),"BrowserBack","BrowserForward",
	"BrowserRefresh","BrowserStop","BrowserSearch","BrowserFavorites","BrowserHome","VolumeMute","VolumeDown","VolumeUp",
	"MediaNextTrack","MediaPrevTrack","MediaStop","MediaPlayPause","LaunchMail","MediaSelect","LaunchApp1","LaunchApp2",
	"0xB8","0xB9",";","+",",",SO(MINUS),".",SO(2),
	SO(3),"0xC1","0xC2","0xC3","0xC4","0xC5","0xC6","0xC7",
	"0xC8","0xC9","0xCA","0xCB","0xCC","0xCD","0xCE","0xCF",
	"0xD0","0xD1","0xD2","0xD3","0xD4","0xD5","0xD6","0xD7",
	"0xD8","0xD9","0xDA",SO(4),"Backslash",SO(6),"'","OEM8",
	"0xE0","AX","<>","ICOHelp","ICO00","Process","ICOClear","Packet",
	"0xE8","Reset","Jump","PA1_2","PA2","PA3","WSCTRL","CUSEL",
	"Attention2","Finish","Copy","Auto","ENLW","Backtab","Attention","CRSEL",
	"EXSEL","EREOF","Play","Zoom","NoName","PA1","Clear2","0xFF"
};
static const char* keyToAlternateString [256] =
{
	"none","LeftClick","RightClick","Cancel","MiddleClick","","","","Back","","","","","Return","","",
	"","","Menu","","","","","","","","","Escape","","","","",
	"","PageUp","PageDown","","","","","","","","PrintScreen","","","Ins","Del","",
	"","","","","","","","","","","","","","","","",
	"","","","","","","","","","","","","","","","",
	"","","","","","","","","","","","","","","","",
	"","","","","","","","","","","","","","","","",
	"","","","","","","","","","","","","","","","",
	"","","","","","","","","","","","","","","","",
	"NumLock","ScrollLock","","","","","","","","","","","","","","",
	"","","","","","","","","","","","","","","","",
	"","","","","","","","","","",SO(1),SO(PLUS),SO(COMMA),"Minus",SO(PERIOD),"?",
	"","","","","","","","","","","","","","","","",
	"","","","oem_pa1","","","","","","","","LBracket","|","RBracket",SO(7),"",
	"ATTN2","","","","","","","","","","","","","","ATTN","",
	"","","","","","","","","","","","","","","",""
};
#undef S
#undef SO




// We will maintain	our	own	list of	items to put in	the	config file,
// so that each	config item	has	only one point of change across	both saving	and	loading
// and to allow us to manage custom types of config items, such as virtual key codes represented as strings

enum ConfigItemType	{
	CIT_BOOL, CIT_INT, CIT_UINT, CIT_STRING, CIT_INVBOOL, CIT_BOOLONOFF, CIT_INVBOOLONOFF, CIT_VKEY, CIT_VKEYMOD
};

struct ConfigItem
{
	const char* name;
	void* addr;
	int	size;
	void* def;
	const char*	comment;
	ConfigItemType type;

	ConfigItem(const char* nname, void*	naddr, int nsize, void*	ndef, const	char* ncomment,	ConfigItemType ntype) {
		addr = naddr; name = nname;	size = nsize; def =	ndef; comment =	ncomment; type = ntype;
	}

	void Get(ConfigFile& conf) {
		switch(type)
		{
			case CIT_BOOL:
			case CIT_BOOLONOFF:
				if(size	== 1) *(uint8 *)addr = (uint8) conf.GetBool(name, def!=0);
				if(size	== 2) *(uint16*)addr = (uint16)conf.GetBool(name, def!=0);
				if(size	== 4) *(uint32*)addr = (uint32)conf.GetBool(name, def!=0);
				if(size	== 8) *(uint64*)addr = (uint64)conf.GetBool(name, def!=0);
				break;
			case CIT_INT:
				if(size	== 1) *(uint8 *)addr = (uint8) conf.GetInt(name, reinterpret_cast<int32>(def));
				if(size	== 2) *(uint16*)addr = (uint16)conf.GetInt(name, reinterpret_cast<int32>(def));
				if(size	== 4) *(uint32*)addr = (uint32)conf.GetInt(name, reinterpret_cast<int32>(def));
				if(size	== 8) *(uint64*)addr = (uint64)conf.GetInt(name, reinterpret_cast<int32>(def));
				break;
			case CIT_UINT:
				if(size	== 1) *(uint8 *)addr = (uint8) conf.GetUInt(name, reinterpret_cast<uint32>(def));
				if(size	== 2) *(uint16*)addr = (uint16)conf.GetUInt(name, reinterpret_cast<uint32>(def));
				if(size	== 4) *(uint32*)addr = (uint32)conf.GetUInt(name, reinterpret_cast<uint32>(def));
				if(size	== 8) *(uint64*)addr = (uint64)conf.GetUInt(name, reinterpret_cast<uint32>(def));
				break;
			case CIT_STRING:
				strncpy((char*)addr, conf.GetString(name, reinterpret_cast<const char*>(def)), size-1);
				((char*)addr)[size-1] = '\0';
				break;
			case CIT_INVBOOL:
			case CIT_INVBOOLONOFF:
				if(size	== 1) *(uint8 *)addr = (uint8) !conf.GetBool(name, def!=0);
				if(size	== 2) *(uint16*)addr = (uint16)!conf.GetBool(name, def!=0);
				if(size	== 4) *(uint32*)addr = (uint32)!conf.GetBool(name, def!=0);
				if(size	== 8) *(uint64*)addr = (uint64)!conf.GetBool(name, def!=0);
				break;
			case CIT_VKEY:
				{
					uint16 keyNum = (uint16)conf.GetUInt(name, reinterpret_cast<uint32>(def));
					const char* keyStr = conf.GetString(name);
					if(keyStr)
					{
						for(int i=0;i<512;i++)
						{
							if(i<256) // keys
							{
								if(!strcasecmp(keyStr,keyToString[i]) ||
								(*keyToAlternateString[i] && !strcasecmp(keyStr,keyToAlternateString[i])))
								{
									keyNum = i;
									break;
								}
							}
							else // joystick:
							{
								char temp [128];
								extern void TranslateKey(WORD keyz,char *out);
								TranslateKey(0x8000|(i-256),temp);
								if(strlen(keyStr)>3 && !strcasecmp(keyStr+3,temp+3))
								{
									for(int j = 0 ; j < 16 ; j++)
									{
										if(keyStr[2]-'0' == j || keyStr[2]-'a' == j-10)
										{
											keyNum = 0x8000|(i-256)|(j<<8);
											i = 512;
											break;
										}
									}
								}
							}
						}
					}
					if(size	== 1) *(uint8 *)addr = (uint8) keyNum;
					if(size	== 2) *(uint16*)addr = (uint16)keyNum;
					if(size	== 4) *(uint32*)addr = (uint32)keyNum;
					if(size	== 8) *(uint64*)addr = (uint64)keyNum;
				}
				break;
			case CIT_VKEYMOD:
				{
					uint16 modNum = 0;
					const char* modStr = conf.GetString(name);
					if(modStr) {
						if(strstr(modStr, "ft") || strstr(modStr, "FT")) modNum |= CUSTKEY_SHIFT_MASK;
						if(strstr(modStr, "tr") || strstr(modStr, "TR")) modNum |= CUSTKEY_CTRL_MASK;
						if(strstr(modStr, "lt") || strstr(modStr, "LT")) modNum |= CUSTKEY_ALT_MASK;
					}
					if(!modNum && (!modStr || strcasecmp(modStr, "none")))
						modNum = conf.GetUInt(name, reinterpret_cast<uint32>(def));
					if(size	== 1) *(uint8 *)addr = (uint8) modNum;
					if(size	== 2) *(uint16*)addr = (uint16)modNum;
					if(size	== 4) *(uint32*)addr = (uint32)modNum;
					if(size	== 8) *(uint64*)addr = (uint64)modNum;
				}
				break;
		}

		// if it had a comment, override our own with it
		const char* newComment = conf.GetComment(name);
		if(newComment && *newComment)
			comment = newComment;
	}

	void Set(ConfigFile& conf) {
		switch(type)
		{
			case CIT_BOOL:
				if(size	== 1) conf.SetBool(name, 0!=(*(uint8 *)addr), "TRUE","FALSE", comment);
				if(size	== 2) conf.SetBool(name, 0!=(*(uint16*)addr), "TRUE","FALSE", comment);
				if(size	== 4) conf.SetBool(name, 0!=(*(uint32*)addr), "TRUE","FALSE", comment);
				if(size	== 8) conf.SetBool(name, 0!=(*(uint64*)addr), "TRUE","FALSE", comment);
				break;
			case CIT_BOOLONOFF:
				if(size	== 1) conf.SetBool(name, 0!=(*(uint8 *)addr), "ON","OFF", comment);
				if(size	== 2) conf.SetBool(name, 0!=(*(uint16*)addr), "ON","OFF", comment);
				if(size	== 4) conf.SetBool(name, 0!=(*(uint32*)addr), "ON","OFF", comment);
				if(size	== 8) conf.SetBool(name, 0!=(*(uint64*)addr), "ON","OFF", comment);
				break;
			case CIT_INT:
				if(size	== 1) conf.SetInt(name,	(int32)(*(uint8	*)addr), comment);
				if(size	== 2) conf.SetInt(name,	(int32)(*(uint16*)addr), comment);
				if(size	== 4) conf.SetInt(name,	(int32)(*(uint32*)addr), comment);
				if(size	== 8) conf.SetInt(name,	(int32)(*(uint64*)addr), comment);
				break;
			case CIT_UINT:
				if(size	== 1) conf.SetUInt(name, (uint32)(*(uint8 *)addr), 10, comment);
				if(size	== 2) conf.SetUInt(name, (uint32)(*(uint16*)addr), 10, comment);
				if(size	== 4) conf.SetUInt(name, (uint32)(*(uint32*)addr), 10, comment);
				if(size	== 8) conf.SetUInt(name, (uint32)(*(uint64*)addr), 10, comment);
				break;
			case CIT_STRING:
				if((char*)addr)
					conf.SetString(name, (char*)addr, comment);
				break;
			case CIT_INVBOOL:
				if(size	== 1) conf.SetBool(name, 0==(*(uint8 *)addr), "TRUE","FALSE", comment);
				if(size	== 2) conf.SetBool(name, 0==(*(uint16*)addr), "TRUE","FALSE", comment);
				if(size	== 4) conf.SetBool(name, 0==(*(uint32*)addr), "TRUE","FALSE", comment);
				if(size	== 8) conf.SetBool(name, 0==(*(uint64*)addr), "TRUE","FALSE", comment);
				break;
			case CIT_INVBOOLONOFF:
				if(size	== 1) conf.SetBool(name, 0==(*(uint8 *)addr), "ON","OFF", comment);
				if(size	== 2) conf.SetBool(name, 0==(*(uint16*)addr), "ON","OFF", comment);
				if(size	== 4) conf.SetBool(name, 0==(*(uint32*)addr), "ON","OFF", comment);
				if(size	== 8) conf.SetBool(name, 0==(*(uint64*)addr), "ON","OFF", comment);
				break;
			case CIT_VKEY:
				{
					uint16 keyNum = 0;
					if(size	== 1) keyNum = (uint8)(*(uint8 *)addr);
					if(size	== 2) keyNum = (uint16)(*(uint16*)addr);
					if(size	== 4) keyNum = (uint16)(*(uint32*)addr);
					if(size	== 8) keyNum = (uint16)(*(uint64*)addr);
					if(keyNum < 256) conf.SetString(name, keyToString[keyNum], comment);
					else if(keyNum & 0x8000) {
						char temp [128];
						extern void TranslateKey(WORD keyz,char *out);
						TranslateKey(keyNum,temp);
						conf.SetString(name, temp, comment);
					}
					else conf.SetUInt(name, keyNum, 16, comment);
				}
				break;
			case CIT_VKEYMOD:
				{
					uint16 modNum = 0;
					if(size	== 1) modNum = (uint8)(*(uint8 *)addr);
					if(size	== 2) modNum = (uint16)(*(uint16*)addr);
					if(size	== 4) modNum = (uint16)(*(uint32*)addr);
					if(size	== 8) modNum = (uint16)(*(uint64*)addr);
					std::string modStr;
					if(modNum & CUSTKEY_CTRL_MASK) modStr += "Ctrl ";
					if(modNum & CUSTKEY_ALT_MASK) modStr += "Alt ";
					if(modNum & CUSTKEY_SHIFT_MASK) modStr += "Shift ";
					if(!(modNum & (CUSTKEY_CTRL_MASK|CUSTKEY_ALT_MASK|CUSTKEY_SHIFT_MASK))) modStr = "none";
					else modStr.erase(modStr.length()-1);
					conf.SetString(name, modStr, comment);
				}
				break;
		}
	}
};

std::vector<ConfigItem> configItems;
// var must be a persistent variable. In the case of strings, it must point to a writeable character array.
#define AddItemC(name, var, def, comment, type) configItems.push_back(ConfigItem((const char*)(CATEGORY "::" name), (void*)(pint)(&var), sizeof(var), (void*)(pint)def, (const char*)comment, (ConfigItemType)type))
#define AddItem(name, var, def, type) AddItemC(name,var,def,"",type)
#define AddUInt(name, var, def) AddItem(name,var,def,CIT_UINT)
#define AddInt(name, var, def) AddItem(name,var,def,CIT_INT)
#define AddBool(name, var, def) AddItem(name,var,def,CIT_BOOL)
#define AddBool2(name, var, def) AddItem(name,var,def,CIT_BOOLONOFF)
#define AddInvBool(name, var, def) AddItem(name,var,def,CIT_INVBOOL)
#define AddInvBool2(name, var, def) AddItem(name,var,def,CIT_INVBOOLONOFF)
#define AddVKey(name, var, def) AddItem(name,var,def,CIT_VKEY)
#define AddVKMod(name, var, def) AddItem(name,var,def,CIT_VKEYMOD)
#define AddUIntC(name, var, def, comment) AddItemC(name,var,def,comment,CIT_UINT)
#define AddIntC(name, var, def, comment) AddItemC(name,var,def,comment,CIT_INT)
#define AddBoolC(name, var, def, comment) AddItemC(name,var,def,comment,CIT_BOOL)
#define AddBool2C(name, var, def, comment) AddItemC(name,var,def,comment,CIT_BOOLONOFF)
#define AddInvBoolC(name, var, def, comment) AddItemC(name,var,def,comment,CIT_INVBOOL)
#define AddInvBool2C(name, var, def, comment) AddItemC(name,var,def,comment,CIT_INVBOOLONOFF)
#define AddStringC(name, var, buflen, def, comment) configItems.push_back(ConfigItem((const char*)(CATEGORY "::" name), (void*)(pint)var, buflen, (void*)(pint)def, (const char*)comment, CIT_STRING))
#define AddString(name, var, buflen, def) AddStringC(name, var, buflen, def, "")

static char filterString [1024], filterString2 [1024], snapVerString [256];
static bool niceAlignment, showComments, readOnlyConfig;
static int configSort;
static DWORD mk_temp;
static BOOL loadedShutdownMaster;
static BOOL preSaveShutdownMaster;

void WinPreSave(ConfigFile& conf)
{
	strcpy(filterString, "output filter: ");
	for(int i=0;i<NUM_FILTERS;i++)
	{
		static char temp [256];
		sprintf(temp, "%d=%s%c ", i, GetFilterName((RenderFilter)i), i!=NUM_FILTERS-1?',':' ');
		strcat(filterString, temp);
	}
	strcpy(filterString2, "hi-res output filter: ");
	for(int i=0;i<NUM_FILTERS;i++)
	{
		if(GetFilterHiResSupport((RenderFilter)i))
		{
			static char temp [256];
			sprintf(temp, "%d=%s%c ", i, GetFilterName((RenderFilter)i), i!=NUM_FILTERS-1?',':' ');
			strcat(filterString2, temp);
		}
	}
	sprintf(snapVerString, "Snapshot save version. Must be between 1 and %d (inclusive)", SNAPSHOT_VERSION);

	preSaveShutdownMaster = Settings.ShutdownMaster;
	Settings.ShutdownMaster &= loadedShutdownMaster; // never save change-to-true of speed hacks during execution

//	GetWindowRect (GUI.hWnd, &GUI.window_size);
	GUI.window_size.right -= GUI.window_size.left;
	GUI.window_size.bottom -= GUI.window_size.top;

	int extra_width  = 2*(GetSystemMetrics(SM_CXBORDER) +
						  GetSystemMetrics(SM_CXDLGFRAME));
	GUI.window_size.right -= extra_width;

	int extra_height = 2*(GetSystemMetrics(SM_CYBORDER) +
						  GetSystemMetrics(SM_CYDLGFRAME)) +
						 GetSystemMetrics(SM_CYCAPTION) +
						 GetSystemMetrics(SM_CYMENU) +
						 (GUI.HideMenu ? 0 : (
						  (GUI.window_size.right <= 392 ? GetSystemMetrics(SM_CYMENU) : 0) + // HACK: accounts for menu wrapping (when width is small)
						  (GUI.window_size.right <= 208 ? GetSystemMetrics(SM_CYMENU) : 0) +
						  (GUI.window_size.right <= 148 ? GetSystemMetrics(SM_CYMENU) : 0)));
	GUI.window_size.bottom -= extra_height;

	if(GUI.window_size.bottom < 10) GUI.window_size.bottom = 10;
	if(GUI.window_size.right < 10) GUI.window_size.right = 10;

	     if(LoadUp7110==&SPC7110Open) mk_temp=1;
	else if(LoadUp7110==&SPC7110Grab) mk_temp=2;
	else                              mk_temp=0;

	if(!Settings.SoundPlaybackRate) {Settings.SoundPlaybackRate = 6; Settings.Mute = TRUE;}
	else if(Settings.SoundPlaybackRate <= 8000) Settings.SoundPlaybackRate = 1;
	else if(Settings.SoundPlaybackRate <= 11025) Settings.SoundPlaybackRate = 2;
	else if(Settings.SoundPlaybackRate <= 16000) Settings.SoundPlaybackRate = 3;
	else if(Settings.SoundPlaybackRate <= 22050) Settings.SoundPlaybackRate = 4;
	else if(Settings.SoundPlaybackRate <= 30000) Settings.SoundPlaybackRate = 5;
	else if(Settings.SoundPlaybackRate <= 32000) Settings.SoundPlaybackRate = 6;
	else if(Settings.SoundPlaybackRate <= 35000) Settings.SoundPlaybackRate = 7;
	else if(Settings.SoundPlaybackRate <= 44100) Settings.SoundPlaybackRate = 8;
	else Settings.SoundPlaybackRate = 9;
	conf.DeleteKey("Sound::Mono");
	if(configSort == 2)
		conf.ClearLines();
}
void WinPostSave(ConfigFile& conf)
{
	/*if(GUI.window_size.left < -30) GUI.window_size.left = -30;
	if(GUI.window_size.top < -30) GUI.window_size.top = -30;
	if(GUI.window_size.left+GUI.window_size.right > GetSystemMetrics(SM_CXSCREEN)+30) GUI.window_size.left = GetSystemMetrics(SM_CXSCREEN)+30-GUI.window_size.right;
	if(GUI.window_size.top+GUI.window_size.bottom > GetSystemMetrics(SM_CYSCREEN)+30) GUI.window_size.top = GetSystemMetrics(SM_CYSCREEN)+30-GUI.window_size.bottom;*/
	int extra_width  = 2*(GetSystemMetrics(SM_CXBORDER) +
						  GetSystemMetrics(SM_CXDLGFRAME));
	int extra_height = 2*(GetSystemMetrics(SM_CYBORDER) +
						  GetSystemMetrics(SM_CYDLGFRAME)) +
						 GetSystemMetrics(SM_CYCAPTION) +
						 (GUI.HideMenu ? 0 : (GetSystemMetrics(SM_CYMENU) +
						  (GUI.window_size.right <= 392 ? GetSystemMetrics(SM_CYMENU) : 0) + // HACK: accounts for menu wrapping (when width is small)
						  (GUI.window_size.right <= 208 ? GetSystemMetrics(SM_CYMENU) : 0) +
						  (GUI.window_size.right <= 148 ? GetSystemMetrics(SM_CYMENU) : 0)));
	GUI.window_size.right += GUI.window_size.left;
	GUI.window_size.bottom += GUI.window_size.top;
	GUI.window_size.right += extra_width;
	GUI.window_size.bottom += extra_height;
	switch(Settings.SoundPlaybackRate){
		case 0: Settings.SoundPlaybackRate = 32000; Settings.Mute = true; break;
		case 1: Settings.SoundPlaybackRate = 8000; break;
		case 2: Settings.SoundPlaybackRate = 11025; break;
		case 3: Settings.SoundPlaybackRate = 16000; break;
		case 4: Settings.SoundPlaybackRate = 22050; break;
		case 5: Settings.SoundPlaybackRate = 30000; break;
		case 6: Settings.SoundPlaybackRate = 32000; break;
		case 7: Settings.SoundPlaybackRate = 35000; break;
		case 8: Settings.SoundPlaybackRate = 44100; break;
		case 9: Settings.SoundPlaybackRate = 48000; break;
	}
	Settings.ShutdownMaster = preSaveShutdownMaster; // revert temp change
}
void WinPostLoad(ConfigFile& conf)
{
	int i;
	GUI.NextScale =	GUI.Scale;
    Settings.NextAPUEnabled = Settings.APUEnabled;
	if(Settings.DisplayPressedKeys) Settings.DisplayPressedKeys = 2;
	for(i=0;i<8;i++) Joypad[i+8].Enabled = Joypad[i].Enabled;
	if(GUI.MaxRecentGames < 1) GUI.MaxRecentGames = 1;
	if(GUI.MaxRecentGames > MAX_RECENT_GAMES_LIST_SIZE) GUI.MaxRecentGames = MAX_RECENT_GAMES_LIST_SIZE;
	bool gap = false;
	for(i=0;i<MAX_RECENT_GAMES_LIST_SIZE;i++) // remove gaps in recent games list
	{
		if(!*GUI.RecentGames[i])
			gap = true;
		else if(gap)
		{
			memmove(GUI.RecentGames[i-1], GUI.RecentGames[i], MAX_PATH);
			*GUI.RecentGames[i] = '\0';
			gap = false;
			i = -1;
		}
	}
	if(conf.Exists("Sound::Mono")) Settings.Stereo = !conf.GetBool("Sound::Mono"); // special case
	switch(mk_temp)
	{
		default:
		case 0: LoadUp7110=&SPC7110Load; break;
		case 1: LoadUp7110=&SPC7110Open; break;
		case 2: LoadUp7110=&SPC7110Grab; break;
	}
	ConfigFile::SetNiceAlignment(niceAlignment);
	ConfigFile::SetShowComments(showComments);
	ConfigFile::SetAlphaSort(configSort==2);
	ConfigFile::SetTimeSort(configSort==1);
	loadedShutdownMaster = Settings.ShutdownMaster;
	preSaveShutdownMaster = Settings.ShutdownMaster;

	if(GUI.MaxRecentMacros < 1) GUI.MaxRecentMacros = 1;
	if(GUI.MaxRecentMacros > MAX_RECENT_MACROS_LIST_SIZE) GUI.MaxRecentMacros = MAX_RECENT_MACROS_LIST_SIZE;
	gap = false;
	for(i=0;i<MAX_RECENT_MACROS_LIST_SIZE;i++) // remove gaps in recent macros list
	{
		if(!*GUI.RecentMacros[i])
			gap = true;
		else if(gap)
		{
			memmove(GUI.RecentMacros[i-1], GUI.RecentMacros[i], MACRO_MAX_TEXT_LENGTH);
			*GUI.RecentMacros[i] = '\0';
			gap = false;
			i = -1;
		}
	}

	if(Settings.CompressionLevel < 0) Settings.CompressionLevel = 0;
	if(Settings.CompressionLevel > 9) Settings.CompressionLevel = 9;

	// realtime is not allowed anyway
	if ((GUI.threadPriority < -7 || GUI.threadPriority > 6) && GUI.threadPriority != THREAD_PRIORITY_IDLE)
		GUI.threadPriority = THREAD_PRIORITY_NORMAL;
	HANDLE hThread = GetCurrentThread();
	SetThreadPriority(hThread, GUI.threadPriority);

	GUI.BackgroundInput &= !GUI.InactivePause;

	WinPostSave(conf);
}

void WinPreLoad(ConfigFile& conf)
{
	if(conf.Exists("Config::Sort"))
	{
		if(conf.GetUInt("Config::Sort") == 1)
		{
			configSort = 1;
			ConfigFile::SetAlphaSort(configSort==2);
			ConfigFile::SetTimeSort(configSort==1);
			conf.ClearLines();
		}
	}
	else
	{
		conf.DeleteKey("Config::Sort");
	}
}


// and now for the important part,
// which determines what goes into and comes out of the config file:
// add all of the things we want to save and load to a list
// (but don't actually save or load anything yet)
void WinRegisterConfigItems()
{
#define CATEGORY "Config"
	AddBool2C("NiceAlignment", niceAlignment, true, "on to line up the =, :, and # in each section of this config file");
	AddBool2C("Comments", showComments, true, "on to keep comments such as this in this config file. To update/refresh all comments, set this to false and run Snes9x, then set it to true and run Snes9x again.");
	AddUIntC("Sort", configSort, 1, "ordering within sections: 0=allow reordering, 1=force default order, 2=sort alphabetically");
	AddBoolC("Lock", readOnlyConfig, false, "if true, prevents Snes9x from editing this configuration file (or making it read-only while it is running)");
#undef CATEGORY
#define CATEGORY "Display"
	AddBool2C("HiRes", Settings.SupportHiRes, true, "on to support the hi-res mode that a few games use, off to render them in low-res");
	AddBool2("Transparency", Settings.Transparency, true);
//	AddBoolC("Mode7Interpolate", Settings.Mode7Interpolate, true, "true for bi-linear filter mode7"); // NYI
	Settings.AutoDisplayMessages = false;
	AddBoolC("MessagesInImage", GUI.MessagesInImage, false, "true to draw text inside the SNES image (will get into AVIs, screenshots, and filters)");
	AddBoolC("LuaDrawingsInImage", Settings.LuaDrawingsInScreen, false, "true to draw Lua graphics inside the SNES image (will get into AVIs, screenshots, and filters)");
	AddBool2C("FrameRate", Settings.DisplayFrameRate, false, "on to display the framerate (will be inaccurate if AutoMaxSkipFrames is too small)");
	AddBoolC("DisplayInput", Settings.DisplayPressedKeys, false, "true to show which buttons are pressed");
	AddBoolC("DisplayFrameCount", Settings.DisplayFrame, true, "true to show the frame count when a movie is playing");
	AddBoolC("DisplayLagCounter", Settings.DisplayLagCounter, true, "true to notify when input was not used by the SNES (such as lag or loading frames)");
	AddBool("OldFashionedFrameCounter", Settings.OldFashionedFrameCounter, false);
	AddBoolC("CounterInFrames", Settings.CounterInFrames, true, "true to show frame count instead of h:mm:ss.ms");
#undef CATEGORY
#define CATEGORY "Display\\Win"
	AddUIntC("OutputMethod", GUI.outputMethod, 0, "0=DirectDraw, 1=Direct3D");
	AddUIntC("FilterType", GUI.Scale, 0, filterString);
	AddUIntC("FilterHiRes", GUI.ScaleHiRes, 0, filterString2);
	AddBoolC("ExtendHeight", GUI.HeightExtend, false, "true to display an extra 15 pixels at the bottom, which few games use. Also increases AVI output size from 256x224 to 256x240.");
	AddIntC("Window:Width", GUI.window_size.right, 512, "256=1x, 512=2x, 768=3x, 1024=4x, etc. (usually)");
	AddIntC("Window:Height", GUI.window_size.bottom, 448, "224=1x, 448=2x, 672=3x,  896=4x, etc. (usually)");
	AddIntC("Window:Left", GUI.window_size.left, 0, "in pixels from left edge of screen");
	AddIntC("Window:Top", GUI.window_size.top, 0, "in pixels from top edge of screen");
	AddBool("Window:Maximized", GUI.window_maximized, false);
	AddBool("Window:LockResize", GUI.windowResizeLocked, false);
	AddBoolC("Stretch:Enabled", GUI.Stretch, true, "true to stretch the game image to fill the window or screen");
	AddBoolC("Stretch:MaintainAspectRatio", GUI.AspectRatio, true, "prevents stretching from changing the aspect ratio");
	AddUIntC("Stretch:AspectRatioBaseWidth", GUI.AspectWidth, 256, "base width for aspect ratio calculation (AR=AspectRatioBaseWidth/224), default is 256");
	AddBoolC("Stretch:UseVideoMemory", GUI.ddrawUseVideoMemory, true, "allows bilinear filtering of stretching. Depending on your video card and the window size, this may result in a lower framerate.");
	AddBoolC("Stretch:LocalVidMem", GUI.ddrawUseLocalVidMem, true, "determines the location of video memory, if UseVideoMemory = true. May increase or decrease rendering performance, depending on your setup and which filter and stretching options are active.");
	AddUIntC("Stretch:D3DFilter", GUI.d3dFilter, /*D3DFilter::*/BILINEAR, "0=Nearest, 1=Bilinear");
	AddBool("Fullscreen:Enabled", GUI.FullScreen, false);
	AddUInt("Fullscreen:Width", GUI.Width, 640);
	AddUInt("Fullscreen:Height", GUI.Height, 480);
	AddUInt("Fullscreen:Depth", GUI.Depth, 16);
	AddUInt("Fullscreen:RefreshRate", GUI.RefreshRate, 60);
	AddBool("Fullscreen:TripleBuffering", GUI.tripleBuffering, false);
	AddBoolC("Fullscreen:EmulateFullscreen", GUI.EmulateFullscreen, false,"true makes snes9x create a window that spans the entire screen when going fullscreen");
	AddBoolC("HideMenu", GUI.HideMenu, false, "true to auto-hide the menu bar on startup.");
	AddBoolC("Vsync", GUI.Vsync, false, "true to enable Vsync, only available with Direct3D");
#undef CATEGORY
#define CATEGORY "Settings"
	AddUIntC("FrameSkip", Settings.SkipFrames, AUTO_FRAMERATE, "200=automatic, 0=none, 1=skip every other, ...");
	AddUIntC("AutoMaxSkipFramesAtOnce", Settings.AutoMaxSkipFrames, 1, "most frames to skip at once to maintain speed, don't set to more than 1 or 2 frames because the skipping algorithm isn't very smart");
	AddUIntC("TurboFrameSkip", Settings.TurboSkipFrames, 15, "how many frames to skip when in fast-forward mode");
	AddUInt("AutoSaveDelay", Settings.AutoSaveDelay, 30);
	AddBool2C("SpeedHacks", Settings.ShutdownMaster, false, "on to skip emulating the CPU when it is not being used ... recommended OFF");
	AddBool("BlockInvalidVRAMAccess", Settings.BlockInvalidVRAMAccess, true);
	AddBool2C("SnapshotScreenshots", Settings.SnapshotScreenshots, true, "on to save the screenshot in each snapshot, for loading-when-paused display");
	AddBoolC("MovieTruncateAtEnd", Settings.MovieTruncate, true, "true to truncate any leftover data in the movie file after the current frame when recording stops");
	AddBool("DisplayWatchedAddresses", Settings.DisplayWatchedAddresses, true);
	AddBool2C("WrongMovieStateProtection", Settings.WrongMovieStateProtection, true, "off to allow states to be loaded for recording from a different movie than they were made in");
	AddInt("StretchScreenshots", Settings.StretchScreenshots, 1);
	AddUIntC("MessageDisplayTime", Settings.InitialInfoStringTimeout, 120, "display length of messages, in frames. set to 0 to disable all message text");
	AddIntC("SavestateCompressionLevel", Settings.CompressionLevel, 3, "0=none, 1=low(fastest), 4=medium(slow), 9=maximum(slowest)");
#undef CATEGORY
#define CATEGORY "Settings\\Script"
	AddStringC("LastScriptFile", S9xGetLuaScriptName(), _MAX_PATH, "", "filename of the last Lua script that ran, for the Reload Lua Script menu option");

#undef CATEGORY
#define CATEGORY "Settings\\Win"
	AddBoolC("PauseWhenInactive", GUI.InactivePause, true, "true to pause Snes9x when it is not the active window");
	AddBool2C("Joypads:Background", GUI.BackgroundInput, false, "on to detect game keypresses and hotkeys while window is inactive, if PauseWhenInactive = FALSE.");
	AddUIntC("Joypads:RepeatDelay", GUI.KeyInDelayMSec, 0, "keyboard/joypad's repeat-delay setting in milliseconds. set to 0 to use system setting.");
	AddUIntC("Joypads:RepeatSpeed", GUI.KeyInRepeatMSec, 0, "keyboard/joypad's repeat-speed setting in milliseconds. set to 0 to use system setting.");
	AddBoolC("CustomRomOpenDialog", GUI.CustomRomOpen, TRUE, "false to use standard Windows open dialog for the ROM open dialog");
//	AddUIntC("Language", GUI.Language, 0, "0=English, 1=Nederlands"); // NYI
	AddBoolC("FrameAdvanceSkipsNonInput", GUI.FASkipsNonInput, false, "causes frame advance to fast-forward past frames where the game is definitely not checking input, such as during lag or loading time. EXPERIMENTAL");
	AddUIntC("SPC7110LoadMethod", mk_temp, 0, "for graphics packs: 0=load all into memory, 1=load nothing into memory, 2=load CacheSize at a time into memory");
	AddUIntC("SPC7110CacheSize", cacheMegs, 5, "number of megabytes used if SPC7110LoadMethod = 2");
	AddBool("MovieDefaultClearSRAM", GUI.MovieClearSRAM, true);
	AddBool("MovieDefaultStartFromReset", GUI.MovieStartFromReset, true);
	AddBool("MovieDefaultReadOnly", GUI.MovieReadOnly, true);
	AddUInt("CurrentSaveSlot", GUI.CurrentSaveSlot, 0);
	AddUIntC("MaxRecentGames", GUI.MaxRecentGames, 14, "max recent games to show in the recent games menu (must be <= 32)");
	AddBoolC("AVIDoubleScale", GUI.AVIDoubleScale, false, "true to record AVI with double scale");
	AddBool2C("DragAndDrop", GUI.allowDropFiles, true, "off to disable drag and drop in main window");
	AddIntC("ThreadPriority", GUI.threadPriority, THREAD_PRIORITY_NORMAL, "thread priority, 0=THREAD_PRIORITY_NORMAL, don't set to more than 1 or 2, or your computer will get stuck");
#undef CATEGORY

#undef CATEGORY
#define CATEGORY "Settings\\Win\\Macro"
	AddUIntC("MacroInputMode", GUI.MacroInputMode, MACRO_INPUT_MOV, "for input macro: 0=disable, 1=overwrite, 2=toggle");
	AddBoolC("PauseWithMacro", GUI.PauseWithMacro, false, "true to pause when input macro stops");
	AddBoolC("SaveMacroSnapshot", GUI.SaveMacroSnap, true, "true to save macro settings with snapshot");
	AddBoolC("PlatformSnapIntoTempDir", GUI.PlatformSnapIntoTempDir, true, "true to platform-depend snapshot into temporary directory instead of snapshot directory");
	AddUIntC("MaxRecentMacros", GUI.MaxRecentMacros, 14, "max recent macros to show in the dropdown (must be <= 32)");
	#define ADD(n) AddString("RecentMacro" #n, GUI.RecentMacros[n-1], MACRO_MAX_TEXT_LENGTH, "")
		ADD(1);  ADD(2);  ADD(3);  ADD(4);  ADD(5);  ADD(6);  ADD(7);  ADD(8);
		ADD(9);  ADD(10); ADD(11); ADD(12); ADD(13); ADD(14); ADD(15); ADD(16);
		ADD(17); ADD(18); ADD(19); ADD(20); ADD(21); ADD(22); ADD(23); ADD(24);
		ADD(25); ADD(26); ADD(27); ADD(28); ADD(29); ADD(30); ADD(31); ADD(32);
		assert(MAX_RECENT_MACROS_LIST_SIZE == 32);
	#undef ADD
#undef CATEGORY

#define CATEGORY "Settings\\Win\\Files"
	AddStringC("Dir:Roms", GUI.RomDir, _MAX_PATH, ".\\Roms", "directory where the Open ROM dialog will start");
	AddStringC("Dir:Screenshots", GUI.ScreensDir, _MAX_PATH, ".\\Screenshots", "directory where screenshots will be saved");
	AddStringC("Dir:Movies", GUI.MovieDir, _MAX_PATH, ".\\Movies", "the default directory for recorded movie (.smv) files");
	AddStringC("Dir:SPCs", GUI.SPCDir, _MAX_PATH, ".\\SPCs", "directory where SPCs will be saved");
	AddStringC("Dir:Savestates", GUI.FreezeFileDir, _MAX_PATH, ".\\Saves", "directory where savestates will be created and loaded from");
	AddStringC("Dir:SRAM", GUI.SRAMFileDir, _MAX_PATH, ".\\Saves", "directory where battery saves will be created and loaded from");
	AddStringC("Dir:Patches", GUI.PatchDir, _MAX_PATH, ".\\Cheats", "directory in which ROM patches (.ips files) and cheats (.cht files) will be looked for");
	AddStringC("Dir:Bios", GUI.BiosDir, _MAX_PATH, ".\\BIOS", "directory where BIOS files (such as \"BS-X.bios\") will be located");
	AddBoolC("Dir:Lock", GUI.LockDirectories, false, "true to prevent Snes9x from changing configured directories when you browse to a new location");
	#define ADD(n) AddString("Rom:RecentGame" #n, GUI.RecentGames[n-1], MAX_PATH, "")
		ADD(1);  ADD(2);  ADD(3);  ADD(4);  ADD(5);  ADD(6);  ADD(7);  ADD(8);
		ADD(9);  ADD(10); ADD(11); ADD(12); ADD(13); ADD(14); ADD(15); ADD(16);
		ADD(17); ADD(18); ADD(19); ADD(20); ADD(21); ADD(22); ADD(23); ADD(24);
		ADD(25); ADD(26); ADD(27); ADD(28); ADD(29); ADD(30); ADD(31); ADD(32);
		assert(MAX_RECENT_GAMES_LIST_SIZE == 32);
	#undef ADD
	AddString("Pack:StarOcean", GUI.StarOceanPack, _MAX_PATH, "");
	AddString("Pack:FarEast", GUI.FEOEZPack, _MAX_PATH, "");
	AddString("Pack:SFA2NTSC", GUI.SFA2NTSCPack, _MAX_PATH, "");
	AddString("Pack:SFA2PAL", GUI.SFA2PALPack, _MAX_PATH, "");
	AddString("Pack:Momotarou", GUI.MDHPack, _MAX_PATH, "");
	AddString("Pack:SFZ2", GUI.SFZ2Pack, _MAX_PATH, "");
	AddString("Pack:ShounenJump", GUI.SJNSPack, _MAX_PATH, "");
	AddString("Pack:SPL4", GUI.SPL4Pack, _MAX_PATH, "");
	AddString("Rom:MultiCartA", multiRomA, _MAX_PATH, "");
	AddString("Rom:MultiCartB", multiRomB, _MAX_PATH, "");
#undef CATEGORY
#define	CATEGORY "Sound"
	AddBoolC("APUEnabled", Settings.APUEnabled, true, "true to enable the sound CPU");
	AddIntC("Sync", Settings.SoundSync, 1, "1 to enable sound sync to CPU, 0 to disable. Necessary for some sounds to be accurate. Not supported unless SoundDriver=0. May cause sound problems on certain setups.");
	AddBool2("Stereo", Settings.Stereo, true);
	AddBool("SixteenBitSound", Settings.SixteenBitSound, true);
//	AddUIntC("AltDecode", Settings.AltSampleDecode, 0, "use alternate sample decoder: valid options are 0, 1, or 2");
	AddUIntC("BufferSize", Settings.SoundBufferSize, 4, "sound buffer size - the mixing interval is multiplied by this (and an additional *4 in case of DirectSound) ");
	AddInvBool2C("Echo", Settings.DisableSoundEcho, true, "on to enable DSP echo effects");
	AddBool("FixFrequency", Settings.FixFrequency, false);
	AddBoolC("Interpolate", Settings.InterpolatedSound, true, "true for gaussian interpolation of sound samples");
	AddInvBool2C("MasterVolume", Settings.DisableMasterVolume, true, "on to allow games to change the sound volume normally, off to lock master volume at 100%");
	AddUIntC("Rate", Settings.SoundPlaybackRate, 6, "sound playback quality, in Hz: 1=8000, 2=11025, 3=16000, 4=22050, 5=30000, 6=32000, 7=35000, 8=44100, 9=48000");
	AddUIntC("SoundSkip", Settings.SoundSkipMethod, 0, "sound CPU skip-waiting method (0, 1, 2, or 3)");
	AddBoolC("ReverseStereo", Settings.ReverseStereo, false, "true to swap speaker outputs");
	AddBool2C("EnvelopeHeightReading", Settings.SoundEnvelopeHeightReading, true, "on to allow game to read the sound volume level (needed for some sound effects to be accurate)");
	AddBool2C("FakeMuteFix", Settings.FakeMuteFix, false, "\"fake mute\" movie desync workaround for certain games");
//	AddInvBool2("SampleCaching", Settings.DisableSampleCaching, false);
	AddBoolC("Mute", Settings.Mute, false, "true to mute sound output (does not disable the sound CPU)");
#undef CATEGORY
#define	CATEGORY "Sound\\Win"
	AddUIntC("SoundDriver", Settings.SoundDriver, 0, "0=Snes9xDirectSound (recommended), 1=fmodDirectSound, 2=fmodWaveSound, 3=fmodA3DSound, 4=XAudio2");
	AddBoolC("MuteFrameAdvance", GUI.FAMute, false, "true to prevent Snes9x from outputting sound when the Frame Advance command is in use");
	//AddUInt("PausedFramesBeforeMutingSound", GUI.PausedFramesBeforeMutingSound, 20);
	AddUInt("SoundMixInterval", Settings.SoundMixInterval, 10);
	AddBoolC("NotifySoundDSPRead", GUI.NotifySoundDSPRead, false, "true to report sound DSP read (help to decide movie sync settings)");
	AddBoolC("FlexibleSoundMix", GUI.FlexibleSoundMixMaster, true, "false to generate sound samples more precisely, that may prevent desyncs on some games, but the playing sound will be noisy (still it's safe for avi recording)");
#undef CATEGORY
#define	CATEGORY "Controls"
	AddBoolC("AllowLeftRight", Settings.UpAndDown, false, "true to allow left+right and up+down");
#undef CATEGORY
#define	CATEGORY "ROM"
	AddBoolC("Cheat", Settings.ApplyCheats, true, "true to allow enabled cheats to be applied");
	AddInvBoolC("Patch", Settings.NoPatch, true, "true to allow IPS patches to be applied (\"soft patching\")");
	AddBoolC("BS", Settings.BS, false, "Broadcast Satellaview emulation");
	AddStringC("Filename", rom_filename, MAX_PATH, "", "filename of ROM to run when Snes9x opens");
#undef CATEGORY
#ifdef NETPLAY_SUPPORT
#define	CATEGORY "Netplay"
	AddUInt("Port", Settings.Port, NP_DEFAULT_PORT);
	AddString("Server", Settings.ServerName, 128, Settings.ServerName);
	AddBool2("SyncByReset", NPServer.SyncByReset, true);
	AddBool2("SendROMImageOnConnect", NPServer.SendROMImageOnConnect, false);
	AddUInt("MaxFrameSkip", NetPlay.MaxFrameSkip, 10);
	AddUInt("MaxBehindFrameCount", NetPlay.MaxBehindFrameCount, 10);
	AddBoolC("UseJoypad1", GUI.NetplayUseJoypad1, true, "if false, player 2 has to use their joypad #2 controls, etc.");
	#define ADD(n,d) AddString("RecentHost" #n, GUI.RecentHostNames[n-1], MAX_PATH, d)
		ADD(1,"localhost");  ADD(2,"");  ADD(3,"");  ADD(4,"");  ADD(5,"");  ADD(6,"");  ADD(7,"");  ADD(8,"");
		ADD(9,"");  ADD(10,"");  ADD(11,"");  ADD(12,"");  ADD(13,"");  ADD(14,"");  ADD(15,"");  ADD(16,"");
		assert(MAX_RECENT_HOSTS_LIST_SIZE == 16);
	#undef ADD
#undef CATEGORY
#endif
#define	CATEGORY "Controls\\Win"
#define ADD(n,x) AddVKey("Joypad" #n ":" #x, Joypad[n-1].x, Joypad[n-1].x)
#define ADDN(n,x,n2) AddVKey("Joypad" #n ":" #n2, Joypad[n-1].x, Joypad[n-1].x)
#define ADDB(n,x) AddBool("Joypad" #n ":" #x, Joypad[n-1].x, Joypad[n-1].x)
#define ADD2(n) ADDB(n,Enabled); ADD(n,Up); ADD(n,Down); ADD(n,Left); ADD(n,Right); ADD(n,A); ADD(n,B); ADD(n,Y); ADD(n,X); ADD(n,L); ADD(n,R); ADD(n,Start); ADD(n,Select); ADDN(n,Left_Up,Left+Up); ADDN(n,Right_Up,Right+Up); ADDN(n,Right_Down,Right+Down); ADDN(n,Left_Down,Left+Down)
#define ADDT(n,x) AddVKey("Joypad" #n "Turbo:" #x, Joypad[n-1+8].x, Joypad[n-1+8].x)
#define ADDTN(n,x,n2) AddVKey("Joypad" #n "Turbo:" #n2, Joypad[n-1+8].x, Joypad[n-1+8].x)
#define ADDT2(n) ADDTN(n,Down,AutoFire); ADDTN(n,Left,AutoHold); ADDTN(n,Up,TempTurbo); ADDTN(n,Right,ClearAll)
#define ADDT3(n) ADDT(n,A); ADDT(n,B); ADDT(n,Y); ADDT(n,X); ADDT(n,L); ADDT(n,R); ADDT(n,Start); ADDT(n,Select)
#define ADD2T2(n) ADD2(n); ADDT2(n)
	// NOTE: for now, the windows port doesn't actually allow 8 different controllers to be set configured, only 5
	ADD2T2(1); ADD2T2(2); ADD2T2(3); ADD2T2(4); ADD2T2(5); ADD2T2(6); ADD2T2(7); ADD2T2(8);
	ADDT3(1); ADDT3(2); ADDT3(3); ADDT3(4); ADDT3(5); ADDT3(6); ADDT3(7); ADDT3(8);
#undef ADD
#undef ADD2
#undef ADDN
#undef ADDB
#undef ADDT
#undef ADDT2
#undef ADDT3
#undef ADDTN
#undef ADD2T2
#undef CATEGORY
#define	CATEGORY "Controls\\Win\\Hotkeys"
	AddBool2C("Handler:Joystick", GUI.JoystickHotkeys, true, "on to detect game controller buttons assigned to hotkeys.");
#define ADD(x) AddVKey("Key:" #x , CustomKeys.x.key, CustomKeys.x.key); AddVKMod("Mods:" #x, CustomKeys.x.modifiers, CustomKeys.x.modifiers)
#define ADDN(x,n2) AddVKey("Key:" #n2, CustomKeys.x.key, CustomKeys.x.key); AddVKMod("Mods:" #n2, CustomKeys.x.modifiers, CustomKeys.x.modifiers)
	ADDN(RecentROM[0],RecentROM0); ADDN(RecentROM[1],RecentROM1); ADDN(RecentROM[2],RecentROM2); ADDN(RecentROM[3],RecentROM3); ADDN(RecentROM[4],RecentROM4); ADDN(RecentROM[5],RecentROM5); ADDN(RecentROM[6],RecentROM6); ADDN(RecentROM[7],RecentROM7); ADDN(RecentROM[8],RecentROM8); ADDN(RecentROM[9],RecentROM9);
	ADD(OpenROM); ADD(OpenMultiCart); ADD(Pause); ADD(ResetGame); ADD(SaveScreenShot); ADD(SaveSPC); ADD(SaveSRAM); ADD(SaveSPC7110Log); ADD(RecordAVI);
	ADDN(Save[0],SaveSlot0); ADDN(Save[1],SaveSlot1); ADDN(Save[2],SaveSlot2); ADDN(Save[3],SaveSlot3); ADDN(Save[4],SaveSlot4); ADDN(Save[5],SaveSlot5); ADDN(Save[6],SaveSlot6); ADDN(Save[7],SaveSlot7); ADDN(Save[8],SaveSlot8); ADDN(Save[9],SaveSlot9);
	ADDN(Load[0],LoadSlot0); ADDN(Load[1],LoadSlot1); ADDN(Load[2],LoadSlot2); ADDN(Load[3],LoadSlot3); ADDN(Load[4],LoadSlot4); ADDN(Load[5],LoadSlot5); ADDN(Load[6],LoadSlot6); ADDN(Load[7],LoadSlot7); ADDN(Load[8],LoadSlot8); ADDN(Load[9],LoadSlot9);
	ADDN(SelectSave[0],SelectSlot0); ADDN(SelectSave[1],SelectSlot1); ADDN(SelectSave[2],SelectSlot2); ADDN(SelectSave[3],SelectSlot3); ADDN(SelectSave[4],SelectSlot4); ADDN(SelectSave[5],SelectSlot5); ADDN(SelectSave[6],SelectSlot6); ADDN(SelectSave[7],SelectSlot7); ADDN(SelectSave[8],SelectSlot8); ADDN(SelectSave[9],SelectSlot9);
	ADD(SlotPlus); ADD(SlotMinus); ADD(SlotSave); ADD(SlotLoad);
	ADDN(ToggleJoypad[0],ToggleJoypad0); ADDN(ToggleJoypad[1],ToggleJoypad1); ADDN(ToggleJoypad[2],ToggleJoypad2); ADDN(ToggleJoypad[3],ToggleJoypad3); ADDN(ToggleJoypad[4],ToggleJoypad4); ADDN(ToggleJoypad[5],ToggleJoypad5); ADDN(ToggleJoypad[6],ToggleJoypad6); ADDN(ToggleJoypad[7],ToggleJoypad7);
	ADD(JoypadSwap); ADD(SwitchControllers);
	ADD(TurboA); ADD(TurboB); ADD(TurboY); ADD(TurboX); ADD(TurboL); ADD(TurboR); ADD(TurboStart); ADD(TurboSelect); ADD(TurboUp); ADD(TurboDown); ADD(TurboLeft); ADD(TurboRight);
	ADD(ScopeTurbo); ADD(ScopePause);
	ADD(SpeedUp); ADD(SpeedDown); ADD(SkipUp); ADD(SkipDown); ADD(FastForward); ADD(ToggleFastForward); ADD(FrameAdvance);
	ADD(BGL1); ADD(BGL2); ADD(BGL3); ADD(BGL4); ADD(BGL5);
	ADD(ClippingWindows); ADD(Transparency); ADD(HDMA);
	ADD(BGLHack); ADD(InterpMode7); ADD(GLCube);
	ADDN(ToggleSound[0],ToggleSound0); ADDN(ToggleSound[1],ToggleSound1); ADDN(ToggleSound[2],ToggleSound2); ADDN(ToggleSound[3],ToggleSound3); ADDN(ToggleSound[4],ToggleSound4); ADDN(ToggleSound[5],ToggleSound5); ADDN(ToggleSound[6],ToggleSound6); ADDN(ToggleSound[7],ToggleSound7);
	ADD(MoviePlay); ADD(MovieRecord);  ADD(MovieStop); ADD(ShowPressed); ADD(FrameCount); ADD(FrameCountOnly); ADD(LagCountOnly); ADD(ReadOnly);
	ADDN(ToggleMacro[0],ToggleMacro0); ADDN(ToggleMacro[1],ToggleMacro1); ADDN(ToggleMacro[2],ToggleMacro2); ADDN(ToggleMacro[3],ToggleMacro3); ADDN(ToggleMacro[4],ToggleMacro4); ADDN(ToggleMacro[5],ToggleMacro5); ADDN(ToggleMacro[6],ToggleMacro6); ADDN(ToggleMacro[7],ToggleMacro7); ADD(EditMacro); ADD(ResetLagCounter);
	ADD(ToggleCheats); ADD(LoadLuaScript); ADD(ReloadLuaScript); ADD(StopLuaScript);
#undef ADD
#undef ADDN
#undef CATEGORY

}




// These let us give the config file read-only status to other applications while snes9x is running,
// to prevent the situation where the user saves a modified .cfg file while snes9x is running
// only to have the changes overwritten by snes9x upon closing snes9x.
static HANDLE locked_file = NULL;
void WinLockConfigFile ()
{
	if(readOnlyConfig) return; // if user has lock on file, don't let Snes9x lock it

	static std::string fname;
	fname=S9xGetDirectory(DEFAULT_DIR);
	fname+=SLASH_STR "snes9x.conf";
	STREAM fp;
	if((fp=OPEN_STREAM(fname.c_str(), "r"))!=NULL){
		CLOSE_STREAM(fp);
		locked_file=CreateFile(fname.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	} else {
		fname=S9xGetDirectory(DEFAULT_DIR);
		fname+=SLASH_STR "snes9x.cfg";
		if((fp=OPEN_STREAM(fname.c_str(), "r"))!=NULL){
			CLOSE_STREAM(fp);
			locked_file=CreateFile(fname.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		}
	}
}
void WinUnlockConfigFile ()
{
	if(locked_file){
		CloseHandle(locked_file);
		locked_file=NULL;
	}
}


/*****************************************************************************/
/* WinSave/WinLoad - Save/Load the settings	to/from	the	registry			 */
/*****************************************************************************/



static ConfigFile loaded_config_file;

void WinLoadConfigFile(ConfigFile& conf)
{
	EnterCriticalSection(&GUI.SoundCritSect);

	WinPreLoad(conf);

	WinSetDefaultValues();

	for(unsigned int i = 0 ; i < configItems.size()	; i++)
		configItems[i].Get(conf);

	loaded_config_file = conf;

	WinPostLoad(conf);

	LeaveCriticalSection(&GUI.SoundCritSect);
}

void WinSaveConfigFile()
{
	if(readOnlyConfig) return; // if user has lock on file, don't let Snes9x overwrite it

	EnterCriticalSection(&GUI.SoundCritSect);

	ConfigFile&	conf = loaded_config_file;
	conf.ClearUnused();

	WinPreSave(conf);

	for(unsigned int i = 0 ; i < configItems.size()	; i++)
		configItems[i].Set(conf);

	bool wasLocked = locked_file!=NULL;
	if(wasLocked) WinUnlockConfigFile();

	S9xSaveConfigFile(conf);

	if(wasLocked) WinLockConfigFile();

	WinPostSave(conf);

	LeaveCriticalSection(&GUI.SoundCritSect);
}


void S9xParsePortConfig(ConfigFile &conf, int pass)
{
	WinLoadConfigFile(conf);
	return;
}

void WinCleanupConfigData()
{
	configItems.clear();
}
