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

// MainWnd.cpp : implementation file
//

#include "stdafx.h"
#include <winsock.h>

#include "resource.h"
#include "MainWnd.h"

#include "CmdAccelOb.h"
#include "FileDlg.h"
#include "Reg.h"
#include "WinResUtil.h"
#include "GBACheats.h"
#include "GBCheatsDlg.h"
#include "Input.h"
#include "7zip/7zip.h"
#include "7zip/OpenArchive.h"
#include "ram_search.h"
#include "LuaOpenDialog.h"
#include "ramwatch.h"
#include "Sound.h"
#include "VBA.h"

#include "../AutoBuild.h"
#include "../gba/Cheats.h"
#include "../gba/CheatSearch.h"
#include "../gba/GBA.h"
#include "../gba/Globals.h"
#include "../gba/Flash.h"
#include "../gba/Globals.h"
#include "../gba/RTC.h"
#include "../gba/Sound.h"
#include "../gb/GB.h"
#include "../gb/gbCheats.h"
#include "../gb/gbGlobals.h"
#include "../common/Util.h"
#include "../common/movie.h"
#include "../common/vbalua.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define VBA_CONFIRM_MODE WM_APP + 100

extern void remoteCleanUp();

/////////////////////////////////////////////////////////////////////////////
// MainWnd

MainWnd::MainWnd()
{
	m_hAccelTable = NULL;
	arrow         = LoadCursor(NULL, IDC_ARROW);

	InitDecoder();
}

MainWnd::~MainWnd()
{
	CleanupDecoder();
}

BEGIN_MESSAGE_MAP(MainWnd, CWnd)
//{{AFX_MSG_MAP(MainWnd)
ON_WM_CLOSE()
ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
ON_COMMAND(ID_FILE_OPENGAMEBOY, OnFileOpenGBx)
ON_WM_INITMENUPOPUP()
ON_COMMAND(ID_FILE_PAUSE, OnFilePause)
ON_UPDATE_COMMAND_UI(ID_FILE_PAUSE, OnUpdateFilePause)
ON_COMMAND(ID_FILE_RESET, OnFileReset)
ON_UPDATE_COMMAND_UI(ID_FILE_RESET, OnUpdateFileReset)
ON_UPDATE_COMMAND_UI(ID_FILE_RECENT_FREEZE, OnUpdateFileRecentFreeze)
ON_COMMAND(ID_FILE_RECENT_RESET, OnFileRecentReset)
ON_COMMAND(ID_FILE_RECENT_FREEZE, OnFileRecentFreeze)
ON_COMMAND(ID_FILE_EXIT, OnFileExit)
ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
ON_UPDATE_COMMAND_UI(ID_FILE_CLOSE, OnUpdateFileClose)
ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
ON_UPDATE_COMMAND_UI(ID_FILE_LOAD, OnUpdateFileLoad)
ON_COMMAND(ID_FILE_SAVE, OnFileSave)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
ON_COMMAND(ID_FILE_IMPORT_BATTERYFILE, OnFileImportBatteryfile)
ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT_BATTERYFILE, OnUpdateFileImportBatteryfile)
ON_COMMAND(ID_FILE_IMPORT_GAMESHARKCODEFILE, OnFileImportGamesharkcodefile)
ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT_GAMESHARKCODEFILE, OnUpdateFileImportGamesharkcodefile)
ON_COMMAND(ID_FILE_IMPORT_GAMESHARKSNAPSHOT, OnFileImportGamesharksnapshot)
ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT_GAMESHARKSNAPSHOT, OnUpdateFileImportGamesharksnapshot)
ON_COMMAND(ID_FILE_EXPORT_BATTERYFILE, OnFileExportBatteryfile)
ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_BATTERYFILE, OnUpdateFileExportBatteryfile)
ON_COMMAND(ID_FILE_EXPORT_GAMESHARKSNAPSHOT, OnFileExportGamesharksnapshot)
ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT_GAMESHARKSNAPSHOT, OnUpdateFileExportGamesharksnapshot)
ON_COMMAND(ID_FILE_QUICKSCREENCAPTURE, OnFileQuickScreencapture)
ON_COMMAND(ID_FILE_SCREENCAPTURE, OnFileScreencapture)
ON_UPDATE_COMMAND_UI(ID_FILE_SCREENCAPTURE, OnUpdateFileScreencapture)
ON_COMMAND(ID_FILE_ROMINFORMATION, OnFileRominformation)
ON_UPDATE_COMMAND_UI(ID_FILE_ROMINFORMATION, OnUpdateFileRominformation)
ON_COMMAND(ID_FILE_TOGGLEMENU, OnFileTogglemenu)
ON_UPDATE_COMMAND_UI(ID_FILE_TOGGLEMENU, OnUpdateFileTogglemenu)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_6, OnUpdateOptionsFrameskipThrottle6)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_15, OnUpdateOptionsFrameskipThrottle15)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_25, OnUpdateOptionsFrameskipThrottle25)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_37, OnUpdateOptionsFrameskipThrottle37)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_50, OnUpdateOptionsFrameskipThrottle50)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_75, OnUpdateOptionsFrameskipThrottle75)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_87, OnUpdateOptionsFrameskipThrottle87)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_100, OnUpdateOptionsFrameskipThrottle100)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_112, OnUpdateOptionsFrameskipThrottle112)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_125, OnUpdateOptionsFrameskipThrottle125)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_150, OnUpdateOptionsFrameskipThrottle150)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_200, OnUpdateOptionsFrameskipThrottle200)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_300, OnUpdateOptionsFrameskipThrottle300)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_OTHER, OnUpdateOptionsFrameskipThrottleOther)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_INCREASE, OnUpdateOptionsFrameskipThrottleIncrease)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_THROTTLE_DECREASE, OnUpdateOptionsFrameskipThrottleDecrease)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_6, OnOptionsFrameskipThrottle6)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_15, OnOptionsFrameskipThrottle15)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_25, OnOptionsFrameskipThrottle25)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_37, OnOptionsFrameskipThrottle37)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_50, OnOptionsFrameskipThrottle50)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_75, OnOptionsFrameskipThrottle75)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_87, OnOptionsFrameskipThrottle87)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_100, OnOptionsFrameskipThrottle100)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_112, OnOptionsFrameskipThrottle112)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_125, OnOptionsFrameskipThrottle125)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_150, OnOptionsFrameskipThrottle150)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_200, OnOptionsFrameskipThrottle200)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_300, OnOptionsFrameskipThrottle300)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_OTHER, OnOptionsFrameskipThrottleOther)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_INCREASE, OnOptionsFrameskipThrottleIncrease)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_THROTTLE_DECREASE, OnOptionsFrameskipThrottleDecrease)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_ACCURATEPITCH, OnOptionsFrameskipAccuratePitch)
ON_COMMAND(ID_OPTIONS_FRAMESKIP_ACCURATESPEED, OnOptionsFrameskipAccurateSpeed)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_ACCURATEPITCH, OnUpdateOptionsFrameskipAccuratePitch)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FRAMESKIP_ACCURATESPEED, OnUpdateOptionsFrameskipAccurateSpeed)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_0, OnUpdateOptionsVideoFrameskip0)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_1, OnUpdateOptionsVideoFrameskip1)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_2, OnUpdateOptionsVideoFrameskip2)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_3, OnUpdateOptionsVideoFrameskip3)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_4, OnUpdateOptionsVideoFrameskip4)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_5, OnUpdateOptionsVideoFrameskip5)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_6, OnUpdateOptionsVideoFrameskip6)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_7, OnUpdateOptionsVideoFrameskip7)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_8, OnUpdateOptionsVideoFrameskip8)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FRAMESKIP_9, OnUpdateOptionsVideoFrameskip9)
ON_COMMAND(ID_OPTIONS_VIDEO_VSYNC, OnOptionsVideoVsync)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_VSYNC, OnUpdateOptionsVideoVsync)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X1, OnUpdateOptionsVideoX1)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X2, OnUpdateOptionsVideoX2)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X3, OnUpdateOptionsVideoX3)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_X4, OnUpdateOptionsVideoX4)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREEN320X240, OnUpdateOptionsVideoFullscreen320x240)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREEN640X480, OnUpdateOptionsVideoFullscreen640x480)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREEN800X600, OnUpdateOptionsVideoFullscreen800x600)
ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREEN320X240, OnOptionsVideoFullscreen320x240)
ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREEN640X480, OnOptionsVideoFullscreen640x480)
ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREEN800X600, OnOptionsVideoFullscreen800x600)
ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREEN, OnOptionsVideoFullscreen)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREEN, OnUpdateOptionsVideoFullscreen)
ON_WM_MOVE()
ON_WM_SIZE()
ON_COMMAND(ID_OPTIONS_VIDEO_DISABLESFX, OnOptionsVideoDisablesfx)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_DISABLESFX, OnUpdateOptionsVideoDisablesfx)
ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREENSTRETCHTOFIT, OnOptionsVideoFullscreenstretchtofit)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_FULLSCREENSTRETCHTOFIT, OnUpdateOptionsVideoFullscreenstretchtofit)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDERMETHOD_GDI, OnOptionsVideoRendermethodGdi)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDERMETHOD_GDI, OnUpdateOptionsVideoRendermethodGdi)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDERMETHOD_DIRECTDRAW, OnOptionsVideoRendermethodDirectdraw)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDERMETHOD_DIRECTDRAW, OnUpdateOptionsVideoRendermethodDirectdraw)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDERMETHOD_DIRECT3D, OnOptionsVideoRendermethodDirect3d)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDERMETHOD_DIRECT3D, OnUpdateOptionsVideoRendermethodDirect3d)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDERMETHOD_OPENGL, OnOptionsVideoRendermethodOpengl)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDERMETHOD_OPENGL, OnUpdateOptionsVideoRendermethodOpengl)
ON_COMMAND(ID_OPTIONS_VIDEO_TRIPLEBUFFERING, OnOptionsVideoTriplebuffering)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_TRIPLEBUFFERING, OnUpdateOptionsVideoTriplebuffering)
ON_COMMAND(ID_OPTIONS_VIDEO_DDRAWEMULATIONONLY, OnOptionsVideoDdrawemulationonly)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_DDRAWEMULATIONONLY, OnUpdateOptionsVideoDdrawemulationonly)
ON_COMMAND(ID_OPTIONS_VIDEO_DDRAWUSEVIDEOMEMORY, OnOptionsVideoDdrawusevideomemory)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_DDRAWUSEVIDEOMEMORY, OnUpdateOptionsVideoDdrawusevideomemory)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_D3DNOFILTER, OnOptionsVideoRenderoptionsD3dnofilter)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_D3DNOFILTER, OnUpdateOptionsVideoRenderoptionsD3dnofilter)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_D3DBILINEAR, OnOptionsVideoRenderoptionsD3dbilinear)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_D3DBILINEAR, OnUpdateOptionsVideoRenderoptionsD3dbilinear)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLNEAREST, OnOptionsVideoRenderoptionsGlnearest)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLNEAREST, OnUpdateOptionsVideoRenderoptionsGlnearest)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLBILINEAR, OnOptionsVideoRenderoptionsGlbilinear)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLBILINEAR, OnUpdateOptionsVideoRenderoptionsGlbilinear)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLTRIANGLE, OnOptionsVideoRenderoptionsGltriangle)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLTRIANGLE, OnUpdateOptionsVideoRenderoptionsGltriangle)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLQUADS, OnOptionsVideoRenderoptionsGlquads)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_GLQUADS, OnUpdateOptionsVideoRenderoptionsGlquads)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_SELECTSKIN, OnOptionsVideoRenderoptionsSelectskin)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_SELECTSKIN, OnUpdateOptionsVideoRenderoptionsSelectskin)
ON_COMMAND(ID_OPTIONS_VIDEO_RENDEROPTIONS_SKIN, OnOptionsVideoRenderoptionsSkin)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_RENDEROPTIONS_SKIN, OnUpdateOptionsVideoRenderoptionsSkin)
ON_WM_CONTEXTMENU()
ON_COMMAND(ID_OPTIONS_EMULATOR_ASSOCIATE, OnOptionsEmulatorAssociate)
ON_COMMAND(ID_OPTIONS_EMULATOR_DIRECTORIES, OnOptionsEmulatorDirectories)
ON_COMMAND_RANGE(ID_OPTIONS_PREFER_ARCHIVE_NAME, ID_OPTIONS_PREFER_ROM_NAME, OnOptionsEmulatorFilenamePreference)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_PREFER_ARCHIVE_NAME, ID_OPTIONS_PREFER_ROM_NAME, OnUpdateOptionsEmulatorFilenamePreference)
ON_COMMAND(ID_OPTIONS_VIDEO_DISABLESTATUSMESSAGES, OnOptionsVideoDisablestatusmessages)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_DISABLESTATUSMESSAGES, OnUpdateOptionsVideoDisablestatusmessages)
ON_COMMAND(ID_OPTIONS_EMULATOR_SYNCHRONIZE, OnOptionsEmulatorSynchronize)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SYNCHRONIZE, OnUpdateOptionsEmulatorSynchronize)
ON_COMMAND(ID_OPTIONS_EMULATOR_PAUSEWHENINACTIVE, OnOptionsEmulatorPausewheninactive)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_PAUSEWHENINACTIVE, OnUpdateOptionsEmulatorPausewheninactive)
ON_COMMAND(ID_OPTIONS_EMULATOR_SPEEDUPTOGGLE, OnOptionsEmulatorSpeeduptoggle)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SPEEDUPTOGGLE, OnUpdateOptionsEmulatorSpeeduptoggle)
ON_COMMAND(ID_OPTIONS_EMULATOR_REMOVEINTROSGBA, OnOptionsEmulatorRemoveintrosgba)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_REMOVEINTROSGBA, OnUpdateOptionsEmulatorRemoveintrosgba)
ON_COMMAND(ID_OPTIONS_EMULATOR_AUTOMATICALLYIPSPATCH, OnOptionsEmulatorAutomaticallyipspatch)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_AUTOMATICALLYIPSPATCH, OnUpdateOptionsEmulatorAutomaticallyipspatch)
ON_COMMAND(ID_OPTIONS_EMULATOR_AGBPRINT, OnOptionsEmulatorAgbprint)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_AGBPRINT, OnUpdateOptionsEmulatorAgbprint)
ON_COMMAND(ID_OPTIONS_EMULATOR_REALTIMECLOCK, OnOptionsEmulatorRealtimeclock)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_REALTIMECLOCK, OnUpdateOptionsEmulatorRealtimeclock)
ON_COMMAND(ID_OPTIONS_EMULATOR_AUTOHIDEMENU, OnOptionsEmulatorAutohidemenu)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_AUTOHIDEMENU, OnUpdateOptionsEmulatorAutohidemenu)
ON_COMMAND(ID_OPTIONS_EMULATOR_REWINDINTERVAL, OnOptionsEmulatorRewindinterval)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_AUTOMATIC, OnOptionsEmulatorSavetypeAutomatic)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_AUTOMATIC, OnUpdateOptionsEmulatorSavetypeAutomatic)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_EEPROM, OnOptionsEmulatorSavetypeEeprom)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_EEPROM, OnUpdateOptionsEmulatorSavetypeEeprom)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_SRAM, OnOptionsEmulatorSavetypeSram)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_SRAM, OnUpdateOptionsEmulatorSavetypeSram)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_FLASH, OnOptionsEmulatorSavetypeFlash)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_FLASH, OnUpdateOptionsEmulatorSavetypeFlash)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_EEPROMSENSOR, OnOptionsEmulatorSavetypeEepromsensor)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_EEPROMSENSOR, OnUpdateOptionsEmulatorSavetypeEepromsensor)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_NONE, OnOptionsEmulatorSavetypeNone)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_NONE, OnUpdateOptionsEmulatorSavetypeNone)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_FLASH512K, OnOptionsEmulatorSavetypeFlash512k)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_FLASH512K, OnUpdateOptionsEmulatorSavetypeFlash512k)
ON_COMMAND(ID_OPTIONS_EMULATOR_SAVETYPE_FLASH1M, OnOptionsEmulatorSavetypeFlash1m)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SAVETYPE_FLASH1M, OnUpdateOptionsEmulatorSavetypeFlash1m)
ON_COMMAND(ID_OPTIONS_EMULATOR_USEBIOSFILE, OnOptionsEmulatorUsebiosfile)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_USEBIOSFILE, OnUpdateOptionsEmulatorUsebiosfile)
ON_COMMAND(ID_OPTIONS_EMULATOR_SKIPBIOS, OnOptionsEmulatorSkipbios)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_SKIPBIOS, OnUpdateOptionsEmulatorSkipbios)
ON_COMMAND(ID_OPTIONS_EMULATOR_SELECTBIOSFILE, OnOptionsEmulatorSelectbiosfile)
ON_COMMAND(ID_EMULATOR_USEOLDGBTIMING, OnOptionsEmulatorUseOldGBTiming)
ON_UPDATE_COMMAND_UI(ID_EMULATOR_USEOLDGBTIMING, OnUpdateOptionsEmulatorUseOldGBTiming)
ON_COMMAND(ID_EMULATOR_GBALAG, OnOptionsEmulatorGBALag)
ON_UPDATE_COMMAND_UI(ID_EMULATOR_GBALAG, OnUpdateOptionsEmulatorGBALag)
ON_COMMAND(ID_OPTIONS_EMULATOR_PNGFORMAT, OnOptionsEmulatorPngformat)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_PNGFORMAT, OnUpdateOptionsEmulatorPngformat)
ON_COMMAND(ID_OPTIONS_EMULATOR_BMPFORMAT, OnOptionsEmulatorBmpformat)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_EMULATOR_BMPFORMAT, OnUpdateOptionsEmulatorBmpformat)
ON_COMMAND(ID_OPTIONS_SOUND_OFF, OnOptionsSoundOff)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_OFF, OnUpdateOptionsSoundOff)
ON_COMMAND(ID_OPTIONS_SOUND_MUTE, OnOptionsSoundMute)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_MUTE, OnUpdateOptionsSoundMute)
ON_COMMAND(ID_OPTIONS_SOUND_ON, OnOptionsSoundOn)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_ON, OnUpdateOptionsSoundOn)
ON_COMMAND(ID_OPTIONS_SOUND_USEOLDSYNCHRONIZATION, OnOptionsSoundUseoldsynchronization)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_USEOLDSYNCHRONIZATION, OnUpdateOptionsSoundUseoldsynchronization)
ON_COMMAND(ID_OPTIONS_SOUND_ECHO, OnOptionsSoundEcho)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_ECHO, OnUpdateOptionsSoundEcho)
ON_COMMAND(ID_OPTIONS_SOUND_LOWPASSFILTER, OnOptionsSoundLowpassfilter)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_LOWPASSFILTER, OnUpdateOptionsSoundLowpassfilter)
ON_COMMAND(ID_OPTIONS_SOUND_REVERSESTEREO, OnOptionsSoundReversestereo)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_REVERSESTEREO, OnUpdateOptionsSoundReversestereo)
ON_COMMAND(ID_OPTIONS_SOUND_MUTEFRAMEADVANCE, OnOptionsSoundMuteFrameAdvance)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_MUTEFRAMEADVANCE, OnUpdateOptionsSoundMuteFrameAdvance)
ON_COMMAND(ID_OPTIONS_SOUND_11KHZ, OnOptionsSound11khz)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_11KHZ, OnUpdateOptionsSound11khz)
ON_COMMAND(ID_OPTIONS_SOUND_22KHZ, OnOptionsSound22khz)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_22KHZ, OnUpdateOptionsSound22khz)
ON_COMMAND(ID_OPTIONS_SOUND_44KHZ, OnOptionsSound44khz)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_44KHZ, OnUpdateOptionsSound44khz)
ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL1, OnOptionsSoundChannel1)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL1, OnUpdateOptionsSoundChannel1)
ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL2, OnOptionsSoundChannel2)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL2, OnUpdateOptionsSoundChannel2)
ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL3, OnOptionsSoundChannel3)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL3, OnUpdateOptionsSoundChannel3)
ON_COMMAND(ID_OPTIONS_SOUND_CHANNEL4, OnOptionsSoundChannel4)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_CHANNEL4, OnUpdateOptionsSoundChannel4)
ON_COMMAND(ID_OPTIONS_SOUND_DIRECTSOUNDA, OnOptionsSoundDirectsounda)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_DIRECTSOUNDA, OnUpdateOptionsSoundDirectsounda)
ON_COMMAND(ID_OPTIONS_SOUND_DIRECTSOUNDB, OnOptionsSoundDirectsoundb)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_DIRECTSOUNDB, OnUpdateOptionsSoundDirectsoundb)
ON_COMMAND(ID_OPTIONS_GAMEBOY_BORDER, OnOptionsGameboyBorder)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_BORDER, OnUpdateOptionsGameboyBorder)
ON_COMMAND(ID_OPTIONS_GAMEBOY_PRINTER, OnOptionsGameboyPrinter)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_PRINTER, OnUpdateOptionsGameboyPrinter)
ON_COMMAND(ID_OPTIONS_GAMEBOY_BORDERAUTOMATIC, OnOptionsGameboyBorderAutomatic)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_BORDERAUTOMATIC, OnUpdateOptionsGameboyBorderAutomatic)
ON_COMMAND(ID_OPTIONS_GAMEBOY_AUTOMATIC, OnOptionsGameboyAutomatic)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_AUTOMATIC, OnUpdateOptionsGameboyAutomatic)
ON_COMMAND(ID_OPTIONS_GAMEBOY_GBA, OnOptionsGameboyGba)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_GBA, OnUpdateOptionsGameboyGba)
ON_COMMAND(ID_OPTIONS_GAMEBOY_CGB, OnOptionsGameboyCgb)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_CGB, OnUpdateOptionsGameboyCgb)
ON_COMMAND(ID_OPTIONS_GAMEBOY_SGB, OnOptionsGameboySgb)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_SGB, OnUpdateOptionsGameboySgb)
ON_COMMAND(ID_OPTIONS_GAMEBOY_SGB2, OnOptionsGameboySgb2)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_SGB2, OnUpdateOptionsGameboySgb2)
ON_COMMAND(ID_OPTIONS_GAMEBOY_GB, OnOptionsGameboyGb)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_GB, OnUpdateOptionsGameboyGb)
ON_COMMAND(ID_OPTIONS_GAMEBOY_REALCOLORS, OnOptionsGameboyRealcolors)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_REALCOLORS, OnUpdateOptionsGameboyRealcolors)
ON_COMMAND(ID_OPTIONS_GAMEBOY_GAMEBOYCOLORS, OnOptionsGameboyGameboycolors)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_GAMEBOY_GAMEBOYCOLORS, OnUpdateOptionsGameboyGameboycolors)
ON_COMMAND(ID_OPTIONS_GAMEBOY_COLORS, OnOptionsGameboyColors)
ON_COMMAND(ID_OPTIONS_FILTER_DISABLEMMX, OnOptionsFilterDisablemmx)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_FILTER_DISABLEMMX, OnUpdateOptionsFilterDisablemmx)
ON_COMMAND(ID_OPTIONS_LANGUAGE_SYSTEM, OnOptionsLanguageSystem)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_SYSTEM, OnUpdateOptionsLanguageSystem)
ON_COMMAND(ID_OPTIONS_LANGUAGE_ENGLISH, OnOptionsLanguageEnglish)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_ENGLISH, OnUpdateOptionsLanguageEnglish)
ON_COMMAND(ID_OPTIONS_LANGUAGE_OTHER, OnOptionsLanguageOther)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_LANGUAGE_OTHER, OnUpdateOptionsLanguageOther)
ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_1, OnOptionsJoypadConfigure1)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_1, OnUpdateOptionsJoypadConfigure1)
ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_2, OnOptionsJoypadConfigure2)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_2, OnUpdateOptionsJoypadConfigure2)
ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_3, OnOptionsJoypadConfigure3)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_3, OnUpdateOptionsJoypadConfigure3)
ON_COMMAND(ID_OPTIONS_JOYPAD_CONFIGURE_4, OnOptionsJoypadConfigure4)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_CONFIGURE_4, OnUpdateOptionsJoypadConfigure4)
ON_COMMAND(ID_OPTIONS_JOYPAD_MOTIONCONFIGURE, OnOptionsJoypadMotionconfigure)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_MOTIONCONFIGURE, OnUpdateOptionsJoypadMotionconfigure)
ON_COMMAND(ID_OPTIONS_JOYPAD_ALLOWLEFTRIGHT, OnOptionsJoypadAllowLeftRight)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_JOYPAD_ALLOWLEFTRIGHT, OnUpdateOptionsJoypadAllowLeftRight)
ON_COMMAND(ID_CHEATS_SEARCHFORCHEATS, OnCheatsSearchforcheats)
ON_UPDATE_COMMAND_UI(ID_CHEATS_SEARCHFORCHEATS, OnUpdateCheatsSearchforcheats)
ON_COMMAND(ID_CHEATS_CHEATLIST, OnCheatsCheatlist)
ON_UPDATE_COMMAND_UI(ID_CHEATS_CHEATLIST, OnUpdateCheatsCheatlist)
ON_COMMAND(ID_CHEATS_AUTOMATICSAVELOADCHEATS, OnCheatsAutomaticsaveloadcheats)
ON_COMMAND(ID_CHEATS_PAUSEDURINGCHEATSEARCH, OnCheatsPauseDuringCheatSearch)
ON_COMMAND(ID_CHEATS_LOADCHEATLIST, OnCheatsLoadcheatlist)
ON_UPDATE_COMMAND_UI(ID_CHEATS_LOADCHEATLIST, OnUpdateCheatsLoadcheatlist)
ON_COMMAND(ID_CHEATS_SAVECHEATLIST, OnCheatsSavecheatlist)
ON_UPDATE_COMMAND_UI(ID_CHEATS_SAVECHEATLIST, OnUpdateCheatsSavecheatlist)
ON_COMMAND(ID_TOOLS_DISASSEMBLE, OnToolsDisassemble)
ON_UPDATE_COMMAND_UI(ID_TOOLS_DISASSEMBLE, OnUpdateToolsDisassemble)
ON_COMMAND(ID_TOOLS_LOGGING, OnToolsLogging)
ON_UPDATE_COMMAND_UI(ID_TOOLS_LOGGING, OnUpdateToolsLogging)
ON_COMMAND(ID_TOOLS_IOVIEWER, OnToolsIoviewer)
ON_UPDATE_COMMAND_UI(ID_TOOLS_IOVIEWER, OnUpdateToolsIoviewer)
ON_COMMAND(ID_TOOLS_MAPVIEW, OnToolsMapview)
ON_UPDATE_COMMAND_UI(ID_TOOLS_MAPVIEW, OnUpdateToolsMapview)
ON_COMMAND(ID_TOOLS_MEMORYVIEWER, OnToolsMemoryviewer)
ON_UPDATE_COMMAND_UI(ID_TOOLS_MEMORYVIEWER, OnUpdateToolsMemoryviewer)
ON_COMMAND(ID_TOOLS_OAMVIEWER, OnToolsOamviewer)
ON_UPDATE_COMMAND_UI(ID_TOOLS_OAMVIEWER, OnUpdateToolsOamviewer)
ON_COMMAND(ID_TOOLS_PALETTEVIEW, OnToolsPaletteview)
ON_UPDATE_COMMAND_UI(ID_TOOLS_PALETTEVIEW, OnUpdateToolsPaletteview)
ON_COMMAND(ID_TOOLS_TILEVIEWER, OnToolsTileviewer)
ON_UPDATE_COMMAND_UI(ID_TOOLS_TILEVIEWER, OnUpdateToolsTileviewer)
ON_COMMAND(ID_DEBUG_NEXTFRAME, OnDebugNextframe)
ON_UPDATE_COMMAND_UI(ID_DEBUG_NEXTFRAME, OnUpdateDebugNextframe)
ON_COMMAND(ID_DEBUG_FRAMESEARCH, OnDebugFramesearch)
ON_UPDATE_COMMAND_UI(ID_DEBUG_FRAMESEARCH, OnUpdateDebugFramesearch)
ON_COMMAND(ID_DEBUG_FRAMESEARCHPREV, OnDebugFramesearchPrev)
ON_UPDATE_COMMAND_UI(ID_DEBUG_FRAMESEARCHPREV, OnUpdateDebugFramesearchPrev)
ON_COMMAND(ID_DEBUG_FRAMESEARCHLOAD, OnDebugFramesearchLoad)
ON_UPDATE_COMMAND_UI(ID_DEBUG_FRAMESEARCHLOAD, OnUpdateDebugFramesearchLoad)
ON_UPDATE_COMMAND_UI(ID_CHEATS_AUTOMATICSAVELOADCHEATS, OnUpdateCheatsAutomaticsaveloadcheats)
ON_UPDATE_COMMAND_UI(ID_CHEATS_PAUSEDURINGCHEATSEARCH, OnUpdateCheatsPauseDuringCheatSearch)
ON_COMMAND(ID_TOOLS_FRAMECOUNTER, OnToolsFrameCounter)
ON_UPDATE_COMMAND_UI(ID_TOOLS_FRAMECOUNTER, OnUpdateToolsFrameCounter)
ON_COMMAND(ID_TOOLS_LAGCOUNTER, OnToolsLagCounter)
ON_UPDATE_COMMAND_UI(ID_TOOLS_LAGCOUNTER, OnUpdateToolsLagCounter)
ON_COMMAND(ID_TOOLS_LAGCOUNTER_RESET, OnToolsLagCounterReset)
ON_COMMAND(ID_TOOLS_INPUTDISPLAY, OnToolsInputDisplay)
ON_UPDATE_COMMAND_UI(ID_TOOLS_INPUTDISPLAY, OnUpdateToolsInputDisplay)
ON_COMMAND(ID_TOOLS_DEBUG_GDB, OnToolsDebugGdb)
ON_UPDATE_COMMAND_UI(ID_TOOLS_DEBUG_GDB, OnUpdateToolsDebugGdb)
ON_COMMAND(ID_TOOLS_DEBUG_LOADANDWAIT, OnToolsDebugLoadandwait)
ON_UPDATE_COMMAND_UI(ID_TOOLS_DEBUG_LOADANDWAIT, OnUpdateToolsDebugLoadandwait)
ON_COMMAND(ID_TOOLS_DEBUG_BREAK, OnToolsDebugBreak)
ON_UPDATE_COMMAND_UI(ID_TOOLS_DEBUG_BREAK, OnUpdateToolsDebugBreak)
ON_COMMAND(ID_TOOLS_DEBUG_DISCONNECT, OnToolsDebugDisconnect)
ON_UPDATE_COMMAND_UI(ID_TOOLS_DEBUG_DISCONNECT, OnUpdateToolsDebugDisconnect)
ON_COMMAND(ID_TOOLS_SOUNDRECORDING, OnToolsSoundRecording)
ON_UPDATE_COMMAND_UI(ID_TOOLS_SOUNDRECORDING, OnUpdateToolsSoundRecording)
ON_COMMAND(ID_TOOLS_AVIRECORDING, OnToolsAVIRecording)
ON_UPDATE_COMMAND_UI(ID_TOOLS_AVIRECORDING, OnUpdateToolsAVIRecording)
ON_WM_PAINT()
ON_COMMAND(ID_MOVIE_RECORD, OnToolsRecordMovie)
ON_UPDATE_COMMAND_UI(ID_MOVIE_RECORD, OnUpdateToolsRecordMovie)
ON_COMMAND(ID_MOVIE_STOP, OnToolsStopMovie)
ON_UPDATE_COMMAND_UI(ID_MOVIE_STOP, OnUpdateToolsStopMovie)
ON_COMMAND(ID_MOVIE_PLAY, OnToolsPlayMovie)
ON_UPDATE_COMMAND_UI(ID_MOVIE_PLAY, OnUpdateToolsPlayMovie)
ON_COMMAND(ID_MOVIE_READONLY, OnToolsPlayReadOnly)
ON_UPDATE_COMMAND_UI(ID_MOVIE_READONLY, OnUpdateToolsPlayReadOnly)
ON_COMMAND(ID_MOVIE_RESUME_RECORD, OnToolsResumeRecord)
ON_UPDATE_COMMAND_UI(ID_MOVIE_RESUME_RECORD, OnUpdateToolsResumeRecord)
ON_COMMAND(ID_MOVIE_RESTART_PLAY, OnToolsPlayRestart)
ON_UPDATE_COMMAND_UI(ID_MOVIE_RESTART_PLAY, OnUpdateToolsPlayRestart)

ON_COMMAND(ID_MOVIE_END_PAUSE, OnToolsOnMovieEndPause)
ON_UPDATE_COMMAND_UI(ID_MOVIE_END_PAUSE, OnUpdateToolsOnMovieEndPause)
ON_COMMAND(ID_MOVIE_END_STOP, OnToolsOnMovieEndStop)
ON_UPDATE_COMMAND_UI(ID_MOVIE_END_STOP, OnUpdateToolsOnMovieEndStop)
ON_COMMAND(ID_MOVIE_END_RESTART, OnToolsOnMovieEndRestart)
ON_UPDATE_COMMAND_UI(ID_MOVIE_END_RESTART, OnUpdateToolsOnMovieEndRestart)
ON_COMMAND(ID_MOVIE_END_RERECORD, OnToolsOnMovieEndRerecord)
ON_UPDATE_COMMAND_UI(ID_MOVIE_END_RERECORD, OnUpdateToolsOnMovieEndRerecord)

ON_COMMAND(ID_TOOLS_REWIND, OnToolsRewind)
ON_UPDATE_COMMAND_UI(ID_TOOLS_REWIND, OnUpdateToolsRewind)
ON_COMMAND(ID_TOOLS_CUSTOMIZE, OnToolsCustomize)
ON_UPDATE_COMMAND_UI(ID_TOOLS_CUSTOMIZE, OnUpdateToolsCustomize)
// ON_COMMAND(ID_TOOLS_CUSTOMIZE_COMMON, OnToolsCustomizeCommon)
// ON_UPDATE_COMMAND_UI(ID_TOOLS_CUSTOMIZE_COMMON, OnUpdateToolsCustomizeCommon)
ON_COMMAND(ID_HELP_BUGREPORT, OnHelpBugreport)
ON_WM_MOUSEMOVE()
ON_WM_INITMENU()
ON_WM_ACTIVATE()
ON_WM_ACTIVATEAPP()
ON_WM_DROPFILES()

ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE10, OnFileRecentFile)
ON_COMMAND_EX_RANGE(ID_FILE_LOADGAME_SLOT1, ID_FILE_LOADGAME_SLOT10, OnFileLoadSlot)
ON_COMMAND_EX_RANGE(ID_FILE_SAVEGAME_SLOT1, ID_FILE_SAVEGAME_SLOT10, OnFileSaveSlot)
ON_COMMAND_EX_RANGE(ID_SELECT_SLOT1, ID_SELECT_SLOT10, OnSelectSlot)
ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE10, OnUpdateFileRecentFile)
ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_LOADGAME_SLOT1, ID_FILE_LOADGAME_SLOT10, OnUpdateFileLoadSlot)
ON_UPDATE_COMMAND_UI_RANGE(ID_FILE_SAVEGAME_SLOT1, ID_FILE_SAVEGAME_SLOT10, OnUpdateFileSaveSlot)
ON_UPDATE_COMMAND_UI_RANGE(ID_SELECT_SLOT1, ID_SELECT_SLOT10, OnUpdateSelectSlot)

ON_COMMAND(ID_FILE_SAVEGAME_OLDESTSLOT, OnFileSavegameOldestslot)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEGAME_OLDESTSLOT, OnUpdateFileSavegameOldestslot)
ON_COMMAND(ID_FILE_LOADGAME_MOSTRECENT, OnFileLoadgameMostrecent)
ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_MOSTRECENT, OnUpdateFileLoadgameMostrecent)
ON_COMMAND(ID_FILE_SAVEGAME_CURRENT, OnFileSavegameCurrent)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEGAME_CURRENT, OnUpdateFileSavegameCurrent)
ON_COMMAND(ID_FILE_LOADGAME_CURRENT, OnFileLoadgameCurrent)
ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_CURRENT, OnUpdateFileLoadgameCurrent)
ON_COMMAND(ID_FILE_LOADGAME_MAKECURRENT, OnFileLoadgameMakeCurrent)
ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_MAKECURRENT, OnUpdateFileLoadgameMakeCurrent)
ON_COMMAND(ID_FILE_SAVEGAME_MAKECURRENT, OnFileSavegameMakeCurrent)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEGAME_MAKECURRENT, OnUpdateFileSavegameMakeCurrent)

ON_COMMAND(ID_FILE_SAVEGAME_INCREMENTSLOT, OnFileSavegameIncrementSlot)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEGAME_INCREMENTSLOT, OnUpdateFileSavegameIncrementSlot)
ON_COMMAND(ID_FILE_SAVEGAME_DECREMENTSLOT, OnFileSavegameDecrementSlot)
ON_UPDATE_COMMAND_UI(ID_FILE_SAVEGAME_DECREMENTSLOT, OnUpdateFileSavegameDecrementSlot)
ON_COMMAND(ID_FILE_SLOT_DISPLAYMODIFICATIONTIME, OnFileSlotDisplayModificationTime)
ON_UPDATE_COMMAND_UI(ID_FILE_SLOT_DISPLAYMODIFICATIONTIME, OnUpdateFileSlotDisplayModificationTime)

ON_COMMAND(ID_FILE_LOADGAME_AUTOLOADMOSTRECENT, OnFileLoadgameAutoloadmostrecent)
ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_AUTOLOADMOSTRECENT, OnUpdateFileLoadgameAutoloadmostrecent)
ON_COMMAND(ID_FILE_LOADGAME_MAKERECENT, OnFileLoadgameMakeRecent)
ON_UPDATE_COMMAND_UI(ID_FILE_LOADGAME_MAKERECENT, OnUpdateFileLoadgameMakeRecent)

ON_COMMAND(ID_OPTIONS_SOUND_VOLUME_25X, OnOptionsSoundVolume25x)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_VOLUME_25X, OnUpdateOptionsSoundVolume25x)
ON_COMMAND(ID_OPTIONS_SOUND_VOLUME_5X, OnOptionsSoundVolume5x)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_SOUND_VOLUME_5X, OnUpdateOptionsSoundVolume5x)
ON_COMMAND(ID_CHEATS_DISABLECHEATS, OnCheatsDisablecheats)
ON_UPDATE_COMMAND_UI(ID_CHEATS_DISABLECHEATS, OnUpdateCheatsDisablecheats)
ON_COMMAND(ID_OPTIONS_VIDEO_FULLSCREENMAXSCALE, OnOptionsVideoFullscreenmaxscale)

ON_COMMAND_EX_RANGE(ID_OPTIONS_VIDEO_FRAMESKIP_0, ID_OPTIONS_VIDEO_FRAMESKIP_9, OnOptionsFrameskip)
ON_COMMAND_EX_RANGE(ID_OPTIONS_VIDEO_X1, ID_OPTIONS_VIDEO_X4, OnOptionVideoSize)
ON_COMMAND_EX_RANGE(ID_OPTIONS_VIDEO_LAYERS_BG0, ID_OPTIONS_VIDEO_LAYERS_OBJWIN, OnVideoLayer)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_VIDEO_LAYERS_BG0, ID_OPTIONS_VIDEO_LAYERS_OBJWIN, OnUpdateVideoLayer)
ON_COMMAND(ID_SYSTEM_MINIMIZE, OnSystemMinimize)
ON_COMMAND(ID_SYSTEM_MAXIMIZE, OnSystemMaximize)
ON_COMMAND_EX_RANGE(ID_OPTIONS_EMULATOR_SHOWSPEED_NONE, ID_OPTIONS_EMULATOR_SHOWSPEED_TRANSPARENT, OnOptionsEmulatorShowSpeed)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_EMULATOR_SHOWSPEED_NONE,
                           ID_OPTIONS_EMULATOR_SHOWSPEED_TRANSPARENT,
                           OnUpdateOptionsEmulatorShowSpeed)
ON_COMMAND_EX_RANGE(ID_OPTIONS_SOUND_VOLUME_1X, ID_OPTIONS_SOUND_VOLUME_4X, OnOptionsSoundVolume)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_SOUND_VOLUME_1X, ID_OPTIONS_SOUND_VOLUME_4X, OnUpdateOptionsSoundVolume)
ON_COMMAND_EX_RANGE(ID_OPTIONS_PRIORITY_HIGHEST, ID_OPTIONS_PRIORITY_BELOWNORMAL, OnOptionsPriority)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_PRIORITY_HIGHEST, ID_OPTIONS_PRIORITY_BELOWNORMAL, OnUpdateOptionsPriority)

ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER_NORMAL, ID_OPTIONS_FILTER_TVMODE, OnOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER16BIT_PIXELATEEXPERIMENTAL, ID_OPTIONS_FILTER16BIT_MOTIONBLUREXPERIMENTAL, OnOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER16BIT_ADVANCEMAMESCALE2X, ID_OPTIONS_FILTER16BIT_SIMPLE2X, OnOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER_BILINEAR, ID_OPTIONS_FILTER_BILINEARPLUS, OnOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER_SCANLINES, ID_OPTIONS_FILTER_SCANLINES, OnOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER_LQ2X, ID_OPTIONS_FILTER_HQ3X2, OnOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER16BIT_SIMPLE3X, ID_OPTIONS_FILTER16BIT_PIXELATEEXPERIMENTAL4X, OnOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER_NORMAL, ID_OPTIONS_FILTER_TVMODE, OnUpdateOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER16BIT_PIXELATEEXPERIMENTAL, ID_OPTIONS_FILTER16BIT_MOTIONBLUREXPERIMENTAL, OnUpdateOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER16BIT_ADVANCEMAMESCALE2X, ID_OPTIONS_FILTER16BIT_SIMPLE2X, OnUpdateOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER_BILINEAR, ID_OPTIONS_FILTER_BILINEARPLUS, OnUpdateOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER_SCANLINES, ID_OPTIONS_FILTER_SCANLINES, OnUpdateOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER_LQ2X, ID_OPTIONS_FILTER_HQ3X2, OnUpdateOptionsFilter)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER16BIT_SIMPLE3X, ID_OPTIONS_FILTER16BIT_PIXELATEEXPERIMENTAL4X, OnUpdateOptionsFilter)
ON_COMMAND_EX_RANGE(ID_OPTIONS_FILTER_INTERFRAMEBLENDING_NONE, ID_OPTIONS_FILTER_INTERFRAMEBLENDING_SMART, OnOptionsFilterIFB)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_FILTER_INTERFRAMEBLENDING_NONE,
                           ID_OPTIONS_FILTER_INTERFRAMEBLENDING_SMART,
                           OnUpdateOptionsFilterIFB)

ON_COMMAND_EX_RANGE(ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_1, ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_4, OnOptionsJoypadDefault)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_1, ID_OPTIONS_JOYPAD_DEFAULTJOYPAD_4, OnUpdateOptionsJoypadDefault)
ON_COMMAND_EX_RANGE(ID_OPTIONS_JOYPAD_AUTOFIRE_A, ID_OPTIONS_JOYPAD_AUTOFIRE_CLEAR, OnOptionsJoypadAutofire)
ON_UPDATE_COMMAND_UI_RANGE(ID_OPTIONS_JOYPAD_AUTOFIRE_A, ID_OPTIONS_JOYPAD_AUTOFIRE_CLEAR, OnUpdateOptionsJoypadAutofire)
ON_COMMAND_EX_RANGE(ID_STICKY_A, ID_STICKY_CLEAR, OnOptionsJoypadSticky)
ON_UPDATE_COMMAND_UI_RANGE(ID_STICKY_A, ID_STICKY_CLEAR, OnUpdateOptionsJoypadSticky)
ON_MESSAGE(VBA_CONFIRM_MODE, OnConfirmMode)
ON_MESSAGE(WM_SYSCOMMAND, OnMySysCommand)
ON_COMMAND(ID_OPTIONS_VIDEO_TEXTDISPLAYOPTIONS, OnOptionsVideoTextdisplayoptions)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_VIDEO_TEXTDISPLAYOPTIONS, OnUpdateOptionsVideoTextdisplayoptions)

ON_COMMAND(ID_FILE_LUA_OPEN, OnFileLuaOpen)
ON_UPDATE_COMMAND_UI(ID_FILE_LUA_OPEN, OnUpdateFileLuaOpen)
ON_COMMAND(ID_FILE_LUA_CLOSE_ALL, OnFileLuaCloseAll)
ON_UPDATE_COMMAND_UI(ID_FILE_LUA_CLOSE_ALL, OnUpdateFileLuaCloseAll)
ON_COMMAND(ID_FILE_LUA_RELOAD, OnFileLuaReload)
ON_COMMAND(ID_FILE_LUA_STOP, OnFileLuaStop)
ON_COMMAND(ID_RAM_SEARCH, OnFileRamSearch)
ON_UPDATE_COMMAND_UI(ID_RAM_SEARCH, OnUpdateFileRamSearch)
ON_COMMAND(ID_RAM_WATCH, OnFileRamWatch)
ON_UPDATE_COMMAND_UI(ID_RAM_WATCH, OnUpdateFileRamWatch)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MainWnd message handlers

bool vbaShuttingDown = false;

void MainWnd::OnClose()
{
	vbaShuttingDown = true; // HACK to fix crash on exit while memory viewer is open

	CWnd::OnClose();

	delete this;
}

// some extensions that might commonly be near emulation-related files that we almost certainly can't open, or at least not directly.
// also includes definitely non-ROM extensions we know about, since we only use this variable in a ROM opening function.
// we do this by exclusion instead of inclusion because we don't want to exclude extensions used for any archive files, even extensionless or unusually-named archives.
static const char* s_romIgnoreExtensions [] = {"vbm", "sgm", "clt", "dat", "gbs", "gcf", "spc", "xpc", "pal", "act", "dmp", "avi", "ini",
	"txt", "nfo", "htm", "html", "jpg", "jpeg", "png", "bmp", "gif", "mp3", "wav", "lnk", "exe", "bat", "luasav", "sav"};

bool noWriteNextBatteryFile = false;
bool MainWnd::FileRun()
{
	int prevCartridgeType = theApp.cartridgeType;

	// save battery file before we change the filename...
	if (rom != NULL || gbRom != NULL)
	{
		if (theApp.autoSaveLoadCheatList)
			winSaveCheatListDefault();
		if (!noWriteNextBatteryFile)
			writeBatteryFile();
		cheatSearchCleanup(&cheatSearchData);
		theApp.emulator.emuCleanUp();
		remoteCleanUp();
		if (VBAMovieActive())
			VBAMovieStop(false); // will only get here on user selecting to open a ROM, canceling movie
		emulating = false;
		theApp.frameSearching      = false;
		theApp.frameSearchSkipping = false;
	}
	noWriteNextBatteryFile = false;
	char tempName[2048];

#if 1
	// use ObtainFile to support opening files within archives (.7z, .rar, .zip, .zip.rar.7z, etc.)

	if(theApp.szFile.GetLength() > 2048) theApp.szFile.Truncate(2048);

	char LogicalName[2048], PhysicalName[2048];
	// FIXME: assertion failure in fopen.c if canceled
	if(ObtainFile(theApp.szFile, LogicalName, PhysicalName, "rom", s_romIgnoreExtensions, sizeof(s_romIgnoreExtensions)/sizeof(*s_romIgnoreExtensions)))
	{
		// theApp.szFile is exactly the filename used for opening, while theApp.filename is always the logical name
		theApp.szFile = theApp.filename = LogicalName;
		ReleaseTempFileCategory("rom", PhysicalName);
	}
	else
	{
		return false;
	}
#else
	// old version that only supports uncompressed, zip, and gzip formats, and doesn't handle multi-file archives well
	char file[2048];
	utilGetBaseName(theApp.szFile, tempName);

	_fullpath(file, tempName, 1024);
	theApp.filename = file;

	const char* LogicalName = theApp.szFile;
	const char* PhysicalName = theApp.szFile;
#endif

	theApp.dir		= getDirFromFile(LogicalName);

	CString ipsname = getRelatedFilename(LogicalName, IDS_IPS_DIR, ".ips");

	IMAGE_TYPE type = utilFindType(PhysicalName);

	if (type == IMAGE_UNKNOWN)
	{
		systemMessage(IDS_UNSUPPORTED_FILE_TYPE,
		              "The file \"%s\" is an unsupported type.", LogicalName);
		return false;
	}
	systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;
	theApp.cartridgeType    = (int)type;
	if (type == IMAGE_GB)
	{
		if (!gbLoadRom(PhysicalName))
			return false;
		theApp.emulator = GBSystem;
		gbBorderOn      = theApp.winGbBorderOn;
		theApp.romSize  = gbRomSize;
		if (theApp.autoIPS)
		{
			int size = gbRomSize;
			utilApplyIPS(ipsname, &gbRom, &size);
			if (size != gbRomSize)
			{
				extern bool gbUpdateSizes();
				gbUpdateSizes();
				gbReset();
				theApp.romSize = size;
			}
		}
	}
	else
	{
		int size = CPULoadRom(PhysicalName);
		if (!size)
			return false;

		theApp.romSize = size;

		flashSetSize(theApp.winFlashSize);
		rtcEnable(theApp.winRtcEnable);
		cpuSaveType = theApp.winSaveType;

		//    if(cpuEnhancedDetection && winSaveType == 0) {
		//      utilGBAFindSave(rom, size);
		//    }

		GetModuleFileName(NULL, tempName, 2048);

		char *p = strrchr(tempName, '\\');
		if (p)
			*p = 0;

		char buffer[5];
		strncpy(buffer, (const char *)&rom[0xac], 4);
		buffer[4] = 0;

		strcat(tempName, "\\vba-over.ini");

		UINT i = GetPrivateProfileInt(buffer,
		                              "rtcEnabled",
		                              -1,
		                              tempName);
		if (i != (UINT)-1)
			rtcEnable(i == 0 ? false : true);

		i = GetPrivateProfileInt(buffer,
		                         "flashSize",
		                         -1,
		                         tempName);
		if (i != (UINT)-1 && (i == 0x10000 || i == 0x20000))
			flashSetSize((int)i);

		i = GetPrivateProfileInt(buffer,
		                         "saveType",
		                         -1,
		                         tempName);
		if (i != (UINT)-1 && (i <= 5))
			cpuSaveType = (int)i;

		theApp.emulator = GBASystem;
		/* disabled due to problems
		   if(theApp.removeIntros && rom != NULL) {
		 *((u32 *)rom)= 0xea00002e;
		   }
		 */

		if (theApp.autoIPS)
		{
			int size = 0x2000000;
			utilApplyIPS(ipsname, &rom, &size);
			if (size != 0x2000000)
			{
				CPUReset();
			}
		}
	}

	if (theApp.soundInitialized)
	{
		if (theApp.cartridgeType == 1)
			gbSoundReset();
		else
			soundReset();
	}
	else
	{
		if (!soundOffFlag)
			soundInit();
		theApp.soundInitialized = true;
	}

	if (type == IMAGE_GBA)
	{
		skipBios = theApp.skipBiosFile ? true : false;
		CPUInit((char *)(LPCTSTR)theApp.biosFileName, theApp.useBiosFile ? true : false);
		CPUReset();
	}

	readBatteryFile();

	if (theApp.autoSaveLoadCheatList)
		winLoadCheatListDefault();

	if (theApp.filenamePreference == 0)
		theApp.addRecentFile(getDiskFilename(LogicalName));
	else
		theApp.addRecentFile(LogicalName);

	theApp.updateWindowSize(theApp.videoOption);

	theApp.updateFrameSkip();

	if (theApp.autoHideMenu && theApp.videoOption > VIDEO_4X && theApp.menuToggle)
		OnFileTogglemenu();

	emulating = true;

	if (theApp.autoLoadMostRecent && !VBAMovieActive() && !VBAMovieLoading()) // would cause desync in movies...
		OnFileLoadgameMostrecent();

	theApp.renderedFrames  = 0;

	theApp.rewindCount      = 0;
	theApp.rewindCounter    = 0;
	theApp.rewindSaveNeeded = false;

	{
		extern bool playMovieFile, playMovieFileReadOnly, outputWavFile, outputAVIFile, flagHideMenu; // from VBA.cpp
		extern char movieFileToPlay [1024], wavFileToOutput [1024]; // from VBA.cpp
		extern int  pauseAfterTime; // from VBA.cpp
		if (playMovieFile)
		{
			playMovieFile = false;
			VBAMovieOpen(movieFileToPlay, playMovieFileReadOnly);
		}
		if (outputWavFile)
		{
			outputWavFile = false;
			theApp.soundRecordName = wavFileToOutput;
			theApp.soundRecording  = true;
		}
		if (outputAVIFile)
		{
			outputAVIFile = false;
			OnToolsStartAVIRecording();
		}
		if (pauseAfterTime >= 0)
		{
			VBAMovieSetPauseAt(pauseAfterTime);
		}
		if (flagHideMenu)
		{
			OnFileTogglemenu();
			theApp.updateWindowSize(theApp.videoOption);
		}
	}

	if (theApp.cartridgeType != prevCartridgeType)
	{
		extern GBACheatSearch gbaDlg;
		extern GBCheatSearch  gbDlg;
		if (!theApp.pauseDuringCheatSearch && theApp.modelessCheatDialogIsOpen)
		{
			gbaDlg.DestroyWindow();
			gbDlg.DestroyWindow();
			theApp.modelessCheatDialogIsOpen = false;
		}
	}

	ReopenRamWindows();
	reset_address_info();

	if (AutoRWLoad)
		MainWnd::OnFileRamWatch();     //auto load ramwatch

	return true;
}

void MainWnd::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

	if (!theApp.changingVideoSize)
	{
		if (this)
		{
			if (!IsIconic())
			{
				RECT r;

				GetWindowRect(&r);
				theApp.windowPositionX = r.left;
				theApp.windowPositionY = r.top;
				theApp.adjustDestRect();
				regSetDwordValue("windowX", theApp.windowPositionX);
				regSetDwordValue("windowY", theApp.windowPositionY);
			}
		}
	}
}

static bool wasPaused = false;

void MainWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	static int  lastType  = -1;

	// hack to re-maximize window after it auto-unmaximizes while loading a ROM
	if (nType == SIZE_MAXIMIZED && lastType == SIZE_MAXIMIZED)
	{
		lastType = -1;
		ShowWindow(SW_SHOWMAXIMIZED);
		MoveWindow(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		return;
	}
	else
		lastType = nType;

	if (!theApp.changingVideoSize)
	{
		if (this)
		{
			if (!IsIconic())
			{
				if (theApp.iconic)
				{
					if (emulating)
					{
						if (!wasPaused)
						{
							soundResume();
							theApp.paused = false;
						}
					}
				}
				if (theApp.videoOption <= VIDEO_4X)
				{
					theApp.surfaceSizeX = cx;
					theApp.surfaceSizeY = cy;
					theApp.adjustDestRect();
					if (theApp.display)
						theApp.display->resize(theApp.dest.right-theApp.dest.left, theApp.dest.bottom-theApp.dest.top);
				}
			}
			else
			{
				if (emulating)
				{
					if (!theApp.paused)
					{
						wasPaused     = false;
						theApp.paused = true;
						soundPause();
					}
				}
				theApp.iconic = true;
			}
		}
	}
}

void MainWnd::winSaveCheatListDefault()
{
	CString cheatName = getRelatedFilename(theApp.filename, IDS_CHEAT_DIR, ".clt");

	winSaveCheatList(cheatName);
}

void MainWnd::winSaveCheatList(const char *name)
{
	if (theApp.cartridgeType == 0)
		cheatsSaveCheatList(name);
	else
		gbCheatsSaveCheatList(name);
}

void MainWnd::winLoadCheatListDefault()
{
	CString cheatName = getRelatedFilename(theApp.filename, IDS_CHEAT_DIR, ".clt");

	winLoadCheatList(cheatName);
}

void MainWnd::winLoadCheatList(const char *name)
{
	bool res = false;

	if (theApp.cartridgeType == 0)
		res = cheatsLoadCheatList(name);
	else
		res = gbCheatsLoadCheatList(name);

	if (res)
		systemScreenMessage(winResLoadString(IDS_LOADED_CHEATS));
}

void MainWnd::writeBatteryFile()
{
	CString batteryName = getRelatedFilename(theApp.filename, IDS_BATTERY_DIR, ".sav");

	if (theApp.emulator.emuWriteBattery)
		theApp.emulator.emuWriteBattery(batteryName);
}

void MainWnd::readBatteryFile()
{
	CString batteryName = getRelatedFilename(theApp.filename, IDS_BATTERY_DIR, ".sav");

	bool res = false;

	if (theApp.emulator.emuReadBattery)
		res = theApp.emulator.emuReadBattery(batteryName);

	if (res)
		systemScreenMessage(winResLoadString(IDS_LOADED_BATTERY));
}

CString MainWnd::winLoadFilter(UINT id)
{
	CString res = winResLoadString(id);
	res.Replace('_', '|');

	return res;
}

bool MainWnd::loadSaveGame(const char *name)
{
	if (theApp.emulator.emuReadState)
		return theApp.emulator.emuReadState(name);
	return false;
}

bool MainWnd::writeSaveGame(const char *name)
{
	if (theApp.emulator.emuWriteState)
		return theApp.emulator.emuWriteState(name);
	return false;
}

void MainWnd::OnContextMenu(CWnd*pWnd, CPoint point)
{
	winMouseOn();
}

void MainWnd::OnSystemMinimize()
{
	ShowWindow(SW_SHOWMINIMIZED);
}
void MainWnd::OnSystemMaximize()
{
	ShowWindow(SW_SHOWMAXIMIZED);
}

bool MainWnd::fileOpenSelect(int cartridgeType)
{
	int selectedFilter = regQueryDwordValue("selectedFilter", 0);
	if (selectedFilter < 0 || selectedFilter > 2)
		selectedFilter = 0;

	LPCTSTR exts[] = { NULL };
	CString filter = winLoadFilter(IDS_FILTER_ROM);
	CString title  = winResLoadString(IDS_SELECT_ROM);

	bool isOverrideEmpty = false;
	CString initialDir = regQueryStringValue(cartridgeType == 0 ? IDS_ROM_DIR : IDS_GBXROM_DIR, ".");
	if (initialDir.IsEmpty())
	{
		isOverrideEmpty = true;
		initialDir = theApp.dir;
	}

	FileDlg dlg(this, "", filter, selectedFilter, "ROM", exts, initialDir, title, false, true);

	if (dlg.DoModal() == IDOK)
	{
		regSetDwordValue("selectedFilter", dlg.m_ofn.nFilterIndex);
		theApp.szFile = dlg.GetPathName();
		initialDir = getDirFromFile(theApp.szFile);

		// we have directory override for that purpose
		// but this can be...desirable
		if (isOverrideEmpty)
			regSetStringValue(cartridgeType == 0 ? IDS_ROM_DIR : IDS_GBXROM_DIR, initialDir);
		return true;
	}
	return false;
}

void MainWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	if (emulating)
	{
		theApp.painting = true;
		systemDrawScreen();
		theApp.painting = false;
		theApp.renderedFrames--;
	}
}

static bool translatingAccelerator = false;

// FIXME: this fix for accel keys is ugly
//   using too many static variables for a single accel key kludge
static bool recursiveCall = true;
static bool fullUpdated = false;
static WPARAM lastKey = 0;

BOOL MainWnd::PreTranslateMessage(MSG*pMsg)
{
	if (RamSearchHWnd && ::IsDialogMessage(RamSearchHWnd, pMsg))
	{
		return TRUE;
	}
	else if (RamWatchHWnd && ::IsDialogMessage(RamWatchHWnd, pMsg))
	{
		if (RamWatchAccels)
			TranslateAccelerator(RamWatchHWnd, RamWatchAccels, pMsg);
		return TRUE;
	}
	else if(LuaConsoleHWnd && ::IsDialogMessage(LuaConsoleHWnd, pMsg))
	{
		return TRUE;
	}
	else if (CWnd::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}
	else if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN)
	{
		translatingAccelerator = true;

		bool bHit = theApp.hAccel != NULL &&  ::TranslateAccelerator(m_hWnd, theApp.hAccel, pMsg);

		// HACK to get around the fact that TranslateAccelerator can't handle modifier-only accelerators
		// (it would be better to fix TranslateAccelerator, but its code is in a Microsoft library...)
		if (!bHit)
		{
			if (pMsg->wParam == VK_SHIFT || pMsg->wParam == VK_CONTROL || pMsg->wParam == VK_MENU)
			{
				// do a linear loop through all accelerators to find modifier-only ones...
				CCmdAccelOb*pCmdAccel;
				WORD        wKey;
				CAccelsOb*  pAccelOb;
				POSITION    pos       = theApp.winAccelMgr.m_mapAccelTable.GetStartPosition();
				const int   modifiers = ((pMsg->wParam == VK_SHIFT) ? FSHIFT : ((pMsg->wParam == VK_CONTROL) ? FCONTROL : FALT));
				while (pos != NULL)
				{
					theApp.winAccelMgr.m_mapAccelTable.GetNextAssoc(pos, wKey, pCmdAccel);
					POSITION pos = pCmdAccel->m_Accels.GetHeadPosition();
					while (pos != NULL)
					{
						pAccelOb = pCmdAccel->m_Accels.GetNext(pos);

						if (pAccelOb->m_wKey == 0) // if accelerator-only
						{
							if ((pAccelOb->m_cVirt & modifiers) == modifiers) // if modifier matches
							{
								SendMessage(WM_COMMAND, pCmdAccel->m_wIDCommand, 0); // tell Windows to call the right function
							}
						}
					}
				}
			}
		}
		else if (lastKey != pMsg->wParam)
		{
			fullUpdated = false;
			lastKey = pMsg->wParam;
		}

		translatingAccelerator = false;
		return bHit;
	}

	return FALSE;
}

void MainWnd::screenCapture(int captureNumber)
{
	CString ext;
	if (theApp.captureFormat != 0)
		ext.Format("_%02d.bmp", captureNumber);
	else
		ext.Format("_%02d.png", captureNumber);

	CString captureName = getRelatedFilename(theApp.filename, IDS_CAPTURE_DIR, ext);

	if (theApp.captureFormat == 0)
		theApp.emulator.emuWritePNG(captureName);
	else
		theApp.emulator.emuWriteBMP(captureName);

	CString msg = winResLoadString(IDS_SCREEN_CAPTURE);
	systemScreenMessage(msg);
}

void MainWnd::winMouseOn()
{
	SetCursor(arrow);
	if (theApp.videoOption > VIDEO_4X)
	{
		theApp.mouseCounter = 120;
	}
	else
		theApp.mouseCounter = 0;
}

void MainWnd::OnMouseMove(UINT nFlags, CPoint point)
{
	winMouseOn();

	CWnd::OnMouseMove(nFlags, point);
}

// recursive kludge
static void InitMenuKludge(CMenu *pParentMenu, CMenu *pMenu, CCmdTarget *pWnd)
{
	ASSERT(pMenu != NULL);

	CCmdUI state;
	state.m_pParentMenu = pParentMenu;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	     state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL)
			{
				continue; // first item of popup can't be routed to
			}

			state.DoUpdate(pWnd, false);
			if (recursiveCall)
			{
				// FIXME: slow recursive calls to fix enabling/disabling of accel keys
				InitMenuKludge(state.m_pMenu, state.m_pSubMenu, pWnd);
			}
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(pWnd, state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
			       pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

void MainWnd::OnInitMenuPopup(CMenu *pMenu, UINT nIndex, BOOL bSysMenu)
{
	ASSERT(pMenu != NULL);

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu; // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd*pParent = GetTopLevelParent();
		// children windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
		    (hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	// FIXME: magic to workaround the accel key bug without slowing down too much
	if (translatingAccelerator && !fullUpdated && state.m_pParentMenu == &theApp.m_menu)
	{
		state.m_pMenu = state.m_pParentMenu;
		recursiveCall = true;
		fullUpdated = true;
	}
	else if (!translatingAccelerator && fullUpdated)
	{
		fullUpdated = false;
	}

	InitMenuKludge(state.m_pParentMenu, state.m_pMenu, this);

	recursiveCall = false;
}

void MainWnd::OnInitMenu(CMenu *pMenu)
{
//	CWnd::OnInitMenu(pMenu);

	if (translatingAccelerator)
	{
	}
	else
	{
		// HACK: we only want to call this if the user is pulling down the menu,
		// but TranslateAccelerator also causes OnInitMenu to be called, so ignore that

		soundPause();
	}
}

void MainWnd::OnActivate(UINT nState, CWnd*pWndOther, BOOL bMinimized)
{
	CWnd::OnActivate(nState, pWndOther, bMinimized);

	bool a = (nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE)
/*
		// FIXME: this is a logical error, which will the emulator fail to pause when the focus is lost
		//   see what theApp.pauseDuringCheatSearch is supposed to be used for: MainWndCheats.cpp
		//   it would be problematic to use, as long as the old cheat search is still using it
		|| (RamSearchHWnd && pWndOther->GetSafeHwnd() == RamSearchHWnd && !theApp.pauseDuringCheatSearch)
		|| (RamWatchHWnd && pWndOther->GetSafeHwnd() == RamWatchHWnd && !theApp.pauseDuringCheatSearch)
*/
	;

	extern bool inputActive;
	inputActive = a;

	if (a && theApp.input)
	{
		theApp.active = a;
		theApp.input->activate();
		if (!theApp.paused)
		{
			if (emulating)
			{
				soundResume();
			}
		}
	}
	else
	{
		wasPaused        = theApp.paused;
		theApp.wasPaused = true;
		if (theApp.pauseWhenInactive)
		{
			if (emulating)
			{
				soundPause();
			}
			theApp.active = a;
		}

		memset(theApp.delta, 255, sizeof(theApp.delta));
	}

	if (theApp.paused && emulating)
	{
		theApp.painting = true;
		systemDrawScreen();
		theApp.painting = false;
	}
}

#if _MSC_VER <= 1200
void MainWnd::OnActivateApp(BOOL bActive, HTASK hTask)
#else
void MainWnd::OnActivateApp(BOOL bActive, DWORD hTask)
#endif
{
	CWnd::OnActivateApp(bActive, hTask);

	if (theApp.tripleBuffering && theApp.videoOption > VIDEO_4X)
	{
		if (bActive)
		{
			if (theApp.display)
				theApp.display->clear();
		}
	}
}

void MainWnd::OnDropFiles(HDROP hDropInfo)
{
	// FIXME: required for the accel key fix
	fullUpdated = false;

	if(theApp.sound) theApp.sound->clearAudioBuffer();

	char szFile[1024];
	char ext[1024];

	if (DragQueryFile(hDropInfo, 0, szFile, 1024))
	{
		DragFinish(hDropInfo);

		_splitpath(szFile, NULL, NULL, NULL, ext);
		if (strcasecmp(ext, ".lua") == 0)
		{
			if (VBALoadLuaCode(szFile))
			{
				// success, there is nothing to do
			}
			else
			{
				// Errors are displayed by the Lua code.
			}
		}
		else if (strcasecmp(ext, ".vbm") == 0)
		{
			SMovie movieInfo;
			char * movieName = szFile;
			char   romTitle [12];
			uint32 romGameCode;
			uint16 checksum;
			uint8  crc;

			if (VBAMovieGetInfo(movieName, &movieInfo) != SUCCESS)
			{
				return;
			}

			extern void fillRomInfo(const SMovie &movieInfo, char romTitle [12],
			                        uint32 & romGameCode, uint16 & checksum, uint8 & crc); // from MovieOpen.cpp

			int cartType = movieInfo.header.typeFlags & 1 ? 0 : 1;

			if (!emulating)
			{
				theApp.winCheckFullscreen();
				if (fileOpenSelect(cartType))
				{
					if (VBAMovieActive())
						VBAMovieStop(false); // will only get here on user selecting to play a ROM, canceling movie
					FileRun();
				}
				else
					return;
			}
			fillRomInfo(movieInfo, romTitle, romGameCode, checksum, crc);

			while (movieInfo.header.romCRC != crc
			       || strncmp(movieInfo.header.romTitle, romTitle, 12) != 0
			       || movieInfo.header.romOrBiosChecksum != checksum
			       && !((movieInfo.header.optionFlags & MOVIE_SETTING_USEBIOSFILE) == 0 && checksum == 0))
			{
				char msg [1024], warning1 [1024], warning2 [1024], buffer [1024];

				strcpy(warning1, "");
				strcpy(warning2, "");
				{
					char str [13];
					strncpy(str, movieInfo.header.romTitle, 12);
					str[12] = '\0';
					sprintf(buffer, "title=%s  ", str);
					strcat(warning1, buffer);

					strncpy(str, romTitle, 12);
					str[12] = '\0';
					sprintf(buffer, "title=%s  ", str);
					strcat(warning2, buffer);
				}
				{
					sprintf(buffer, "type=%s  ",
					        (movieInfo.header.typeFlags&MOVIE_TYPE_GBA) ? "GBA" : (movieInfo.header.typeFlags&
					                                                               MOVIE_TYPE_GBC) ? "GBC" : (movieInfo.header.
					                                                                                          typeFlags&
					                                                                                          MOVIE_TYPE_SGB) ? "SGB" : "GB");
					strcat(warning1, buffer);

					sprintf(buffer, "type=%s  ", theApp.cartridgeType ==
					        0 ? "GBA" : (gbRom[0x143] & 0x80 ? "GBC" : (gbRom[0x146] == 0x03 ? "SGB" : "GB")));
					strcat(warning2, buffer);
				}
				{
					sprintf(buffer, "crc=%02d  ", movieInfo.header.romCRC);
					strcat(warning1, buffer);

					sprintf(buffer, "crc=%02d  ", crc);
					strcat(warning2, buffer);
				}
				{
					char code [5];
					if (movieInfo.header.typeFlags&MOVIE_TYPE_GBA)
					{
						memcpy(code, &movieInfo.header.romGameCode, 4);
						code[4] = '\0';
						sprintf(buffer, "code=%s  ", code);
						strcat(warning1, buffer);
					}

					if (theApp.cartridgeType == 0)
					{
						memcpy(code, &romGameCode, 4);
						code[4] = '\0';
						sprintf(buffer, "code=%s  ", code);
						strcat(warning2, buffer);
					}
				}
				{
					sprintf(buffer,
					        movieInfo.header.typeFlags&
					        MOVIE_TYPE_GBA ? ((movieInfo.header.optionFlags & MOVIE_SETTING_USEBIOSFILE) ==
					                          0 ? "(bios=none)  " :  "(bios=%d)  ") : "check=%d  ",
					        movieInfo.header.romOrBiosChecksum);
					strcat(warning1, buffer);

					sprintf(buffer,
					        checksum == 0 ? "(bios=none)  " : theApp.cartridgeType == 0 ? "(bios=%d)  " : "check=%d  ",
					        checksum);
					strcat(warning2, buffer);
				}

				strcpy(msg, "");
				sprintf(buffer, "Movie ROM: %s\n", warning1);
				strcat(msg, buffer);
				sprintf(buffer, "Your ROM: %s\n", warning2);
				strcat(msg, buffer);
				strcat(msg, "still want to play the movie?");

				int sel = MessageBox(msg, TEXT("ROM Mismatch"), MB_ABORTRETRYIGNORE|MB_ICONQUESTION);
				switch (sel)
				{
				case IDABORT:
					return;
				case IDRETRY:
					theApp.winCheckFullscreen();
					if (fileOpenSelect(cartType))
					{
						if (VBAMovieActive())
							VBAMovieStop(false); // will only get here on user selecting to play a ROM, canceling movie
						FileRun();
						fillRomInfo(movieInfo, romTitle, romGameCode, checksum, crc);
					}
					else
						return;
					break;
				default:
					goto romcheck_exit;
				}
			}
romcheck_exit:

			theApp.useBiosFile = (movieInfo.header.optionFlags & MOVIE_SETTING_USEBIOSFILE) != 0;
			if (theApp.useBiosFile)
			{
				extern bool checkBIOS(CString & biosFileName); // from MovieCreate.cpp
				if (!checkBIOS(theApp.biosFileName))
				{
					systemMessage(0, "This movie requires a valid GBA BIOS file to play.\nPlease locate a BIOS file.");
					((MainWnd *)theApp.m_pMainWnd)->OnOptionsEmulatorSelectbiosfile();
					if (!checkBIOS(theApp.biosFileName))
					{
						systemMessage(0, "\"%s\" is not a valid BIOS file; cannot play movie without one.", theApp.biosFileName);
						return;
					}
				}
			}
			extern void loadBIOS();
			loadBIOS();

			int code = VBAMovieOpen(movieName, theApp.movieReadOnly);

			if (code != SUCCESS)
			{
				if (code == FILE_NOT_FOUND)
					systemMessage(0, "Could not find movie file \"%s\".", (const char *)movieName);
				else if (code == WRONG_FORMAT)
					systemMessage(0, "Movie file \"%s\" is not in proper VBM format.", (const char *)movieName);
				else if (code == WRONG_VERSION)
					systemMessage(0, "Movie file \"%s\" is not a supported version.", (const char *)movieName);
				else
					systemMessage(0, "Failed to open movie \"%s\".", (const char *)movieName);
				return;
			}
		}
		else if (strcasecmp(ext, ".wch") == 0)
		{
			if (emulating) {
				MainWnd::OnFileRamWatch();
				Load_Watches(true, szFile);
			}
		}
		else
		{
			theApp.szFile = szFile;
			if (FileRun())
			{
				SetForegroundWindow();
				emulating = TRUE;
			}
			else
			{
				emulating = FALSE;
				soundPause();
			}
		}
	}
	else
		DragFinish(hDropInfo);
}

LRESULT MainWnd::OnMySysCommand(WPARAM wParam, LPARAM lParam)
{
	if (emulating && !theApp.paused)
	{
		if ((wParam&0xFFF0) == SC_SCREENSAVE || (wParam&0xFFF0) == SC_MONITORPOWER)
			return 0;
	}
	return Default();
}

void OnFrameBoundary()
{
	Update_RAM_Search(); // updates RAM search and RAM watch
}

