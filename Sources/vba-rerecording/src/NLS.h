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

#ifndef VBS_NLS_H
#define VBA_NLS_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define N_(String) (String)

#define MSG_UNSUPPORTED_VBA_SGM             1
#define MSG_CANNOT_LOAD_SGM                 2
#define MSG_SAVE_GAME_NOT_USING_BIOS        3
#define MSG_SAVE_GAME_USING_BIOS            4
#define MSG_UNSUPPORTED_SAVE_TYPE           5
#define MSG_CANNOT_OPEN_FILE                6
#define MSG_BAD_ZIP_FILE                    7
#define MSG_NO_IMAGE_ON_ZIP                 8
#define MSG_ERROR_OPENING_IMAGE             9
#define MSG_ERROR_READING_IMAGE            10
#define MSG_UNSUPPORTED_BIOS_FUNCTION      11
#define MSG_INVALID_BIOS_FILE_SIZE         12
#define MSG_INVALID_CHEAT_CODE             13
#define MSG_UNKNOWN_ARM_OPCODE             14
#define MSG_UNKNOWN_THUMB_OPCODE           15
#define MSG_ERROR_CREATING_FILE            16
#define MSG_FAILED_TO_READ_SGM             17
#define MSG_FAILED_TO_READ_RTC             18
#define MSG_UNSUPPORTED_VB_SGM             19
#define MSG_CANNOT_LOAD_SGM_FOR            20
#define MSG_ERROR_OPENING_IMAGE_FROM       21
#define MSG_ERROR_READING_IMAGE_FROM       22
#define MSG_UNSUPPORTED_ROM_SIZE           23
#define MSG_UNSUPPORTED_RAM_SIZE           24
#define MSG_UNKNOWN_CARTRIDGE_TYPE         25
#define MSG_MAXIMUM_NUMBER_OF_CHEATS       26
#define MSG_INVALID_GAMESHARK_CODE         27
#define MSG_INVALID_GAMEGENIE_CODE         28
#define MSG_INVALID_CHEAT_TO_REMOVE        29
#define MSG_INVALID_CHEAT_CODE_ADDRESS     30
#define MSG_UNSUPPORTED_CHEAT_LIST_VERSION 31
#define MSG_UNSUPPORTED_CHEAT_LIST_TYPE    32
#define MSG_INVALID_GSA_CODE               33
#define MSG_CANNOT_IMPORT_SNAPSHOT_FOR     34
#define MSG_UNSUPPORTED_SNAPSHOT_FILE      35
#define MSG_UNSUPPORTED_ARM_MODE           36
#define MSG_UNSUPPORTED_CODE_FILE          37
#define MSG_GBA_CODE_WARNING               38
#define MSG_INVALID_CBA_CODE               39
#define MSG_CBA_CODE_WARNING               40
#define MSG_OUT_OF_MEMORY                  41

#endif // VBA_NLS_H
