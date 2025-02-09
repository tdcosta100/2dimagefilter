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

#ifndef VBA_WIN32_SOUND_H
#define VBA_WIN32_SOUND_H

class ISound
{
public:
	virtual ~ISound() {};

	virtual bool init()   = 0;
	virtual void pause()  = 0;
	virtual void reset()  = 0;
	virtual void resume() = 0;
	virtual void write()  = 0;
	virtual void setSpeed(float rate) = 0;
	virtual bool isPlaying() = 0;
	virtual void clearAudioBuffer() {}
};

#endif
