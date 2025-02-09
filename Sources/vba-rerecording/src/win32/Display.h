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

#ifndef VBA_WIN32_DISASSEMBLE_H
#define VBA_WIN32_DISASSEMBLE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum DISPLAY_TYPE
{
	GDI         = 0,
	DIRECT_DRAW = 1,
	DIRECT_3D   = 2,
	OPENGL      = 3
};

class IDisplay
{
public:
	IDisplay() {};
	virtual ~IDisplay() {};

	virtual bool initialize() = 0;
	virtual void cleanup()    = 0;
	virtual void render()     = 0;
	virtual void checkFullScreen() { };
	virtual void renderMenu() { };
	virtual void clear() = 0;
	virtual bool changeRenderSize(int w, int h) { return true; };
	virtual void resize(int w, int h) {};
	virtual void setOption(const char *option, int value) = 0;
	virtual DISPLAY_TYPE getType() = 0;
	virtual int selectFullScreenMode(GUID * *) = 0;
};

#endif // VBA_WIN32_DISASSEMBLE_H
