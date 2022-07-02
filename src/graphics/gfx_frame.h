/*
 * TheXTech - A platform game engine ported from old source code for VB6
 *
 * Copyright (c) 2009-2011 Andrew Spinks, original VB6 code
 * Copyright (c) 2020-2022 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef GFX_FRAME_H
#define GFX_FRAME_H

#include "location.h"
#include "std_picture.h"

struct FrameBorderInfo
{
	// top, left, bottom, and right, external and internal
	int te = 2;
	int ti = 18;
	int le = 2;
	int li = 18;
	int be = 18;
	int bi = 0;
	int re = 18;
	int ri = 0;
};

// renders a new-style frame with the included external and internal locations
// border and borderinfo are nullable
void RenderFrame(const Location_t& external, const Location_t& internal,
	StdPicture& tile, StdPicture* border, const FrameBorderInfo* borderinfo);

#endif // GFX_FRAME_H
