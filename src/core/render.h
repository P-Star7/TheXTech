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
#ifndef RENDER_HHHHHH
#define RENDER_HHHHHH

#include "xcolor.h"
#include <SDL2/SDL_stdinc.h>
#include "base/render_base.h"

#ifndef RENDER_CUSTOM
#   define E_INLINE SDL_FORCE_INLINE
#   define TAIL
#else
#   define E_INLINE    extern
#   define TAIL ;
#endif

namespace XDepth
{
    constexpr int16_t Layer(int8_t layer)
    {
        return (int16_t)layer * 256;
    }
    constexpr int16_t Offset(double offset)
    {
        return (int16_t)(offset * 256);
    }

    // for arbitrary draws, safe to use this:
    constexpr int16_t Default = 0;

    // in 3D effect, this is 2 units back
    constexpr int16_t Background2 = Layer(-100);
    // in 3D effect, this is 1 unit back
    constexpr int16_t BGO_Back = Layer(-95);
    constexpr int16_t SBlock = Layer(-90);
    constexpr int16_t BGO = Layer(-85);
    constexpr int16_t BGO_Lock = Layer(-80);
    // in 3D effect, this is screen plane
    constexpr int16_t NPC_Back = Layer(-75);
    constexpr int16_t Block = Layer(-65);
    constexpr int16_t Effect_Back = Layer(-60);
    constexpr int16_t NPC_Coin = Layer(-55);
    constexpr int16_t NPC_Iced = Layer(-50);
    constexpr int16_t NPC_Normal = Layer(-45);
    constexpr int16_t ChatIcon = Layer(-40);
    constexpr int16_t ClownCar = Layer(-35);
    constexpr int16_t NPC_Held = Layer(-32); // was incorrectly listed as -70
    constexpr int16_t Mount = Layer(-30);
    constexpr int16_t Player = Layer(-25);

    constexpr int16_t BGO_Front = Layer(-20);
    constexpr int16_t NPC_Front = Layer(-15);
    constexpr int16_t Block_Front = Layer(-10);
    constexpr int16_t Effect = Layer(-5);
    constexpr int16_t Phys = Layer(-4);

    // in 3D effect, this is 1 unit forewards
    constexpr int16_t HUD = Layer(5);
    constexpr int16_t NPC_Dropped = Layer(10);
    constexpr int16_t ScreenEffect = Layer(15);
    constexpr int16_t UI = Layer(20);
    constexpr int16_t Meta = Layer(25);
    constexpr int16_t EditorItem = Layer(30);
    constexpr int16_t Cursor = Layer(35);
    constexpr int16_t TouchscreenController = Layer(40);

    // in 3D effect, this is 2 units back
    constexpr int16_t WorldTile = Layer(-120);
    constexpr int16_t WorldScene = Layer(-115);
    constexpr int16_t WorldPath = Layer(-110);
    constexpr int16_t WorldLevel = Layer(-105);
    constexpr int16_t WorldMusic = Layer(-100);
    constexpr int16_t WorldPlayer = Layer(-100);
    // in 3D effect, this is 1 unit back
    constexpr int16_t WorldEffect = Layer(-90);
    // in 3D effect, this is screen plane
    constexpr int16_t WorldFrame = Layer(-75);
}

namespace XRender
{

/*!
 * \brief Identify does render engine works or not
 * \return true if render initialized and works
 */
E_INLINE bool isWorking() TAIL
#ifndef RENDER_CUSTOM
{
    return g_render->isWorking();
}
#endif

/*!
 * \brief Call the repaint
 */
E_INLINE void repaint() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->repaint();
}
#endif

/*!
 * \brief Update viewport (mainly after screen resize)
 */
E_INLINE void updateViewport() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->updateViewport();
}
#endif

/*!
 * \brief Reset viewport into default state
 */
E_INLINE void resetViewport() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->resetViewport();
}
#endif

/*!
 * \brief Set the viewport area
 * \param x X position
 * \param y Y position
 * \param w Viewport Width
 * \param h Viewport Height
 */
E_INLINE void setViewport(int x, int y, int w, int h) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->setViewport(x, y, w, h);
}
#endif

/*!
 * \brief Set the render offset
 * \param x X offset
 * \param y Y offset
 *
 * All drawing objects will be drawn with a small offset
 */
E_INLINE void offsetViewport(int x, int y) TAIL // for screen-shaking
#ifndef RENDER_CUSTOM
{
    g_render->offsetViewport(x, y);
}
#endif

/*!
 * \brief Set temporary ignore of render offset
 * \param en Enable viewport offset ignore
 *
 * Use this to draw certain objects with ignorign of the GFX offset
 */
E_INLINE void offsetViewportIgnore(bool en) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->offsetViewportIgnore(en);
}
#endif

/*!
 * \brief Map absolute point coordinate into screen relative
 * \param x Window X position
 * \param y Window Y position
 * \param dx Destinition on-screen X position
 * \param dy Destinition on-screen Y position
 */
E_INLINE void mapToScreen(int x, int y, int *dx, int *dy) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->mapToScreen(x, y, dx, dy);
}
#endif


/*!
 * \brief Map screen relative coordinate into physical canvas
 * \param x On-screen X position
 * \param y On-screen Y position
 * \param dx Destinition window X position
 * \param dy Destinition window Y position
 */
E_INLINE void mapFromScreen(int x, int y, int *dx, int *dy) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->mapFromScreen(x, y, dx, dy);
}
#endif

/*!
 * \brief Set render target into the E_INLINE in-game screen (use to render in-game world)
 */
E_INLINE void setTargetTexture() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->setTargetTexture();
}
#endif

/*!
 * \brief Set render target into the real window or screen (use to render on-screen buttons and other meta-info)
 */
E_INLINE void setTargetScreen() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->setTargetScreen();
}
#endif



SDL_FORCE_INLINE StdPicture LoadPicture(const std::string &path,
                                        const std::string &maskPath = std::string(),
                                        const std::string &maskFallbackPath = std::string())
{
    return AbstractRender_t::LoadPicture(path, maskPath, maskFallbackPath);
}

SDL_FORCE_INLINE StdPicture lazyLoadPicture(const std::string &path,
                                            const std::string &maskPath = std::string(),
                                            const std::string &maskFallbackPath = std::string())
{
    return AbstractRender_t::lazyLoadPicture(path, maskPath, maskFallbackPath);
}

SDL_FORCE_INLINE void setTransparentColor(StdPicture &target, uint32_t rgb)
{
    AbstractRender_t::setTransparentColor(target, rgb);
}



E_INLINE void loadTexture(StdPicture &target,
                          uint32_t width,
                          uint32_t height,
                          uint8_t *RGBApixels,
                          uint32_t pitch) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->loadTexture(target, width, height, RGBApixels, pitch);
}
#endif

SDL_FORCE_INLINE void lazyLoad(StdPicture &target)
{
    AbstractRender_t::lazyLoad(target);
}

SDL_FORCE_INLINE void lazyUnLoad(StdPicture &target)
{
    AbstractRender_t::lazyUnLoad(target);
}

SDL_FORCE_INLINE void lazyPreLoad(StdPicture &target)
{
    AbstractRender_t::lazyPreLoad(target);
}

SDL_FORCE_INLINE size_t lazyLoadedBytes()
{
    return AbstractRender_t::lazyLoadedBytes();
}

SDL_FORCE_INLINE void lazyLoadedBytesReset()
{
    AbstractRender_t::lazyLoadedBytesReset();
}



E_INLINE void deleteTexture(StdPicture &tx, bool lazyUnload = false) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->deleteTexture(tx, lazyUnload);
}
#endif

E_INLINE void clearAllTextures() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->clearAllTextures();
}
#endif

E_INLINE void clearBuffer() TAIL
#ifndef RENDER_CUSTOM
{
    g_render->clearBuffer();
}
#endif



// Draw primitives

E_INLINE void renderRect(int x, int y, int w, int h, int16_t depth,
                        XColor color,
                        bool filled = true) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderRect(x, y, w, h, depth,
                         color,
                         filled);
}
#endif

E_INLINE void renderRectBR(int _left, int _top, int _right, int _bottom, int16_t depth,
                           XColor color) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderRectBR(_left, _top, _right, _bottom, depth,
                           color);
}
#endif

E_INLINE void renderCircle(int cx, int cy,
                          int radius, int16_t depth,
                          XColor color,
                          bool filled = true) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderCircle(cx, cy,
                           radius, depth,
                           color,
                           filled);
}
#endif

E_INLINE void renderCircleHole(int cx, int cy,
                              int radius, int16_t depth,
                              XColor color) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderCircleHole(cx, cy,
                               radius, depth,
                               color);
}
#endif


// Draw texture

E_INLINE void renderTextureScaleEx(double xDst, double yDst, double wDst, double hDst, int16_t depth,
                          StdPicture &tx,
                          int xSrc, int ySrc,
                          int wSrc, int hSrc,
                          double rotateAngle = 0.0, FPoint_t *center = nullptr, unsigned int flip = X_FLIP_NONE,
                          XColor color = XColor_None) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderTextureScaleEx(xDst, yDst, wDst, hDst, depth,
                                   tx,
                                   xSrc, ySrc,
                                   wSrc, hSrc,
                                   rotateAngle, center, flip,
                                   color);
}
#endif

E_INLINE void renderTextureScale(double xDst, double yDst, double wDst, double hDst, int16_t depth,
                        StdPicture &tx,
                        XColor color = XColor_None) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderTextureScale(xDst, yDst, wDst, hDst, depth,
                                 tx,
                                 color);
}
#endif

E_INLINE void renderTexture(double xDst, double yDst, double wDst, double hDst, int16_t depth,
                           StdPicture &tx,
                           int xSrc, int ySrc,
                           XColor color = XColor_None) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderTexture(xDst, yDst, wDst, hDst, depth,
                            tx,
                            xSrc, ySrc,
                            color);
}
#endif

E_INLINE void renderTextureFL(double xDst, double yDst, double wDst, double hDst, int16_t depth,
                             StdPicture &tx,
                             int xSrc, int ySrc,
                             double rotateAngle = 0.0, FPoint_t *center = nullptr, unsigned int flip = X_FLIP_NONE,
                             XColor color = XColor_None) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderTextureFL(xDst, yDst, wDst, hDst, depth,
                              tx,
                              xSrc, ySrc,
                              rotateAngle, center, flip,
                              color);
}
#endif

E_INLINE void renderTexture(float xDst, float yDst, int16_t depth, StdPicture &tx,
                           XColor color = XColor_None) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->renderTexture(xDst, yDst, depth, tx, color);
}
#endif



// Retrieve raw pixel data

E_INLINE void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->getScreenPixels(x, y, w, h, pixels);
}
#endif

E_INLINE void getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->getScreenPixelsRGBA(x, y, w, h, pixels);
}
#endif

E_INLINE int  getPixelDataSize(const StdPicture &tx) TAIL
#ifndef RENDER_CUSTOM
{
    return g_render->getPixelDataSize(tx);
}
#endif

E_INLINE void getPixelData(const StdPicture &tx, unsigned char *pixelData) TAIL
#ifndef RENDER_CUSTOM
{
    g_render->getPixelData(tx, pixelData);
}
#endif

#ifdef USE_RENDER_BLOCKING
SDL_FORCE_INLINE bool renderBlocked()
{
    return AbstractRender_t::renderBlocked();
}

SDL_FORCE_INLINE void setBlockRender(bool b)
{
    AbstractRender_t::setBlockRender(b);
}
#endif // USE_RENDER_BLOCKING


#ifdef USE_SCREENSHOTS_AND_RECS

SDL_FORCE_INLINE void makeShot()
{
    AbstractRender_t::makeShot();
}

SDL_FORCE_INLINE void toggleGifRecorder()
{
    AbstractRender_t::toggleGifRecorder();
}

SDL_FORCE_INLINE void processRecorder()
{
    AbstractRender_t::processRecorder();
}

#endif // USE_SCREENSHOTS_AND_RECS


} // XRender

#ifndef RENDER_CUSTOM
#   undef E_INLINE
#   undef TAIL
#endif


#endif // RENDER_HHHHHH
