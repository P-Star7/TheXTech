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
#ifndef XCOLOR_H
#define XCOLOR_H

struct XColor
{
    uint32_t _color;
    constexpr XColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        : _color((uint32_t)a << 24 | (uint32_t)b << 16 | (uint32_t)g << 8 | (uint32_t)r)
    {}
    constexpr XColor(int r, int g, int b, int a = 255)
        : _color((uint32_t)uint8_t(a) << 24 | (uint32_t)uint8_t(b) << 16 | (uint32_t)uint8_t(g) << 8 | (uint32_t)uint8_t(r))
    {}
    constexpr XColor(float r, float g, float b, float a = 1.f)
        : _color((uint32_t)uint8_t(a*255) << 24 | (uint32_t)uint8_t(b*255) << 16 | (uint32_t)uint8_t(g*255) << 8 | (uint32_t)uint8_t(r*255))
    {}
    constexpr XColor(double r, double g, double b, double a = 1.)
        : _color((uint32_t)uint8_t(a*255) << 24 | (uint32_t)uint8_t(b*255) << 16 | (uint32_t)uint8_t(g*255) << 8 | (uint32_t)uint8_t(r*255))
    {}
    constexpr bool operator==(const XColor& lhs)
    {
        return lhs._color == _color;
    }
    constexpr bool operator!=(const XColor& lhs)
    {
        return lhs._color != _color;
    }
    constexpr uint8_t r() const
    {
        return _color && 0xff;
    }
    constexpr uint8_t g() const
    {
        return _color >> 8 && 0xff;
    }
    constexpr uint8_t b() const
    {
        return _color >> 16 && 0xff;
    }
    constexpr uint8_t a() const
    {
        return _color >> 24 && 0xff;
    }
};

constexpr XColor XColor_None = XColor(255, 255, 255, 255);

#endif