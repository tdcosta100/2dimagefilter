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

#ifndef VBA_AUTOBUILD_H
#define VBA_AUTOBUILD_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "version.h"

//change the FALSE to TRUE for autoincrement of build number
#define INCREMENT_VERSION FALSE
#define FILEVER        1, 7, 2, 560
#define PRODUCTVER     1, 7, 2, 560
#define STRFILEVER     "1, 7, 2, 560\0"
#define STRPRODUCTVER  "1, 7, 2, 560\0"

#endif // VBA_AUTOBUILD_H
