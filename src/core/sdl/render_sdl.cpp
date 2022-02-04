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

#include <SDL2/SDL_version.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_opengl.h>

#include <FreeImageLite.h>
#include <Logger/logger.h>
#include <Utils/maths.h>

#include "render_sdl.h"
#include "video.h"
#include "../window.h"

#include <SDL2/SDL_assert.h>

#include "controls.h"

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

// Workaround for older SDL versions that lacks the floating-point based rects and points
#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 10)
#define XTECH_SDL_NO_RECTF_SUPPORT
#define SDL_RenderCopyF SDL_RenderCopy
#define SDL_RenderCopyExF SDL_RenderCopyEx
#endif



RenderSDL::RenderSDL() :
    AbstractRender_t()
{}

RenderSDL::~RenderSDL()
{
    if(m_window)
        RenderSDL::close();
}

unsigned int RenderSDL::SDL_InitFlags()
{
    return 0;
}

bool RenderSDL::isWorking()
{
    return m_gRenderer && m_tBuffer;
}

bool RenderSDL::initRender(const CmdLineSetup_t &setup, SDL_Window *window)
{
    pLogDebug("Init renderer settings...");

    if(!AbstractRender_t::init())
        return false;

    m_window = window;

    Uint32 renderFlags = 0;

    switch(setup.renderType)
    {
    case RENDER_ACCELERATED_VSYNC:
        renderFlags = SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC;
        g_videoSettings.renderModeObtained = RENDER_ACCELERATED_VSYNC;
        pLogDebug("Using accelerated rendering with a vertical synchronization");
        m_gRenderer = SDL_CreateRenderer(window, -1, renderFlags | SDL_RENDERER_TARGETTEXTURE); // Try to make renderer
        if(m_gRenderer)
            break; // All okay
        pLogWarning("Failed to initialize V-Synced renderer, trying to create accelerated renderer...");

        // fallthrough
    case RENDER_ACCELERATED:
        renderFlags = SDL_RENDERER_ACCELERATED;
        g_videoSettings.renderModeObtained = RENDER_ACCELERATED;
        pLogDebug("Using accelerated rendering");
        m_gRenderer = SDL_CreateRenderer(window, -1, renderFlags | SDL_RENDERER_TARGETTEXTURE); // Try to make renderer
        if(m_gRenderer)
            break; // All okay
        pLogWarning("Failed to initialize accelerated renderer, trying to create a software renderer...");

        // fallthrough
    case RENDER_SOFTWARE:
        renderFlags = SDL_RENDERER_SOFTWARE;
        g_videoSettings.renderModeObtained = RENDER_SOFTWARE;
        pLogDebug("Using software rendering");
        m_gRenderer = SDL_CreateRenderer(window, -1, renderFlags | SDL_RENDERER_TARGETTEXTURE); // Try to make renderer
        if(m_gRenderer)
            break; // All okay

        pLogCritical("Unable to create renderer!");
        return false;
    }

    SDL_RendererInfo ri;
    SDL_GetRendererInfo(m_gRenderer, &ri);
    m_maxTextureWidth = ri.max_texture_width;
    m_maxTextureHeight = ri.max_texture_height;

    m_tBuffer = SDL_CreateTexture(m_gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, ScaleWidth, ScaleHeight);
    if(!m_tBuffer)
    {
        pLogCritical("Unable to create texture render buffer!");
        return false;
    }

    // Clean-up from a possible start-up junk
    clearBuffer();

    setTargetTexture();
    SDL_SetRenderDrawBlendMode(m_gRenderer, SDL_BLENDMODE_BLEND);

    updateViewport();

    // Clean-up the texture buffer from the same start-up junk
    clearBuffer();

    setTargetScreen();

    repaint();

    return true;
}

void RenderSDL::close()
{
    RenderSDL::clearAllTextures();
    AbstractRender_t::close();

    if(m_tBuffer)
        SDL_DestroyTexture(m_tBuffer);
    m_tBuffer = nullptr;

    if(m_gRenderer)
        SDL_DestroyRenderer(m_gRenderer);
    m_gRenderer = nullptr;
}

void RenderSDL::updateViewport()
{
    float w, w1, h, h1;
    int   wi, hi;

#ifndef __EMSCRIPTEN__
    XWindow::getWindowSize(&wi, &hi);
#else
    if(XWindow::isFullScreen())
    {
        XWindow::getWindowSize(&wi, &hi);
    }
    else
    {
        wi = ScaleWidth;
        hi = ScaleHeight;
    }
#endif

    w = wi;
    h = hi;
    w1 = w;
    h1 = h;

    m_scale_x = w / ScaleWidth;
    m_scale_y = h / ScaleHeight;
    m_viewport_scale_x = m_scale_x;
    m_viewport_scale_y = m_scale_y;

    m_viewport_offset_x = 0;
    m_viewport_offset_y = 0;
    m_viewport_offset_x_cur = 0;
    m_viewport_offset_y_cur = 0;
    m_viewport_offset_ignore = false;

    if(m_scale_x > m_scale_y)
    {
        w1 = m_scale_y * ScaleWidth;
        m_viewport_scale_x = w1 / ScaleWidth;
    }
    else if(m_scale_x < m_scale_y)
    {
        h1 = m_scale_x * ScaleHeight;
        m_viewport_scale_y = h1 / ScaleHeight;
    }

    m_offset_x = (w - w1) / 2;
    m_offset_y = (h - h1) / 2;
}

void RenderSDL::updateViewportInternal()
{
    if(!m_viewport_enabled)
        SDL_RenderSetViewport(m_gRenderer, nullptr);
    else
    {
        SDL_Rect topLeftViewport = {m_viewport_x, m_viewport_y, m_viewport_w, m_viewport_h};
        SDL_RenderSetViewport(m_gRenderer, &topLeftViewport);
    }
}

void RenderSDL::mapToScreen(int x, int y, int *dx, int *dy)
{
    *dx = static_cast<int>((static_cast<float>(x) - m_offset_x) / m_viewport_scale_x);
    *dy = static_cast<int>((static_cast<float>(y) - m_offset_y) / m_viewport_scale_y);
}

void RenderSDL::mapFromScreen(int scr_x, int scr_y, int *window_x, int *window_y)
{
    *window_x = (float)scr_x * m_viewport_scale_x + m_offset_x;
    *window_y = (float)scr_y * m_viewport_scale_y + m_offset_y;
}

void RenderSDL::setTargetTexture()
{
    if(m_recentTarget == m_tBuffer)
        return;
    flushRenderQueue();
    SDL_SetRenderTarget(m_gRenderer, m_tBuffer);
    m_recentTarget = m_tBuffer;
}

void RenderSDL::setTargetScreen()
{
    if(m_recentTarget == nullptr)
        return;
    flushRenderQueue();
    SDL_SetRenderTarget(m_gRenderer, nullptr);
    m_recentTarget = nullptr;
}

void RenderSDL::loadTexture(StdPicture &target, uint32_t width, uint32_t height, uint8_t *RGBApixels, uint32_t pitch)
{
    SDL_Surface *surface;
    SDL_Texture *texture = nullptr;

    target.d.nOfColors = GL_RGBA;
    target.d.format = GL_BGRA;

    surface = SDL_CreateRGBSurfaceFrom(RGBApixels,
                                       static_cast<int>(width),
                                       static_cast<int>(height),
                                       32,
                                       static_cast<int>(pitch),
                                       FI_RGBA_RED_MASK,
                                       FI_RGBA_GREEN_MASK,
                                       FI_RGBA_BLUE_MASK,
                                       FI_RGBA_ALPHA_MASK);
    if(surface)
        texture = SDL_CreateTextureFromSurface(m_gRenderer, surface);

    SDL_FreeSurface(surface);

    if(!texture)
    {
        pLogWarning("Render SDL: Failed to load texture! (%s)", SDL_GetError());
        target.d.clear();
        target.inited = false;
        return;
    }

    target.d.texture = texture;
    m_textureBank.insert(texture);

    target.inited = true;
}

void RenderSDL::deleteTexture(StdPicture &tx, bool lazyUnload)
{
    if(!tx.inited || !tx.d.texture)
    {
        if(!lazyUnload)
            tx.inited = false;
        return;
    }

    auto corpseIt = m_textureBank.find(tx.d.texture);
    if(corpseIt == m_textureBank.end())
    {
        SDL_DestroyTexture(tx.d.texture);
        tx.d.texture = nullptr;
        if(!lazyUnload)
            tx.inited = false;
        return;
    }

    SDL_Texture *corpse = *corpseIt;
    if(corpse)
        SDL_DestroyTexture(corpse);
    m_textureBank.erase(corpse);

    tx.d.texture = nullptr;

    if(!lazyUnload)
        tx.resetAll();

    tx.d.format = 0;
    tx.d.nOfColors = 0;

    tx.resetColors();
}

void RenderSDL::clearAllTextures()
{
    for(SDL_Texture *tx : m_textureBank)
        SDL_DestroyTexture(tx);
    m_textureBank.clear();
}

void RenderSDL::clearBuffer()
{
#ifdef USE_RENDER_BLOCKING
    SDL_assert(!m_blockRender);
#endif
    SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_gRenderer);
}


static SDL_INLINE void txColorMod(StdPictureData &tx, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if(tx.modColor[0] != r || tx.modColor[1] != g || tx.modColor[2] != b)
    {
        SDL_SetTextureColorMod(tx.texture, r, g, b);
        tx.modColor[0] = r;
        tx.modColor[1] = g;
        tx.modColor[2] = b;
    }

    if(tx.modColor[3] != a)
    {
        SDL_SetTextureAlphaMod(tx.texture, a);
        tx.modColor[3] = a;
    }
}

void RenderSDL::executeRender(const RenderCall_t &render, int16_t depth)
{
    switch(render.type)
    {
    case RenderCallType::rect:
    {
        SDL_Rect aRect = {render.xDst, render.yDst,
                          render.wDst, render.hDst};
        SDL_SetRenderDrawColor(m_gRenderer,
                               render.r,
                               render.g,
                               render.b,
                               render.a
                              );

        if(render.features & RenderFeatures::filled)
            SDL_RenderFillRect(m_gRenderer, &aRect);
        else
            SDL_RenderDrawRect(m_gRenderer, &aRect);

        break;
    }
    case RenderCallType::circle:
    {
        SDL_SetRenderDrawColor(m_gRenderer,
                               render.r,
                               render.g,
                               render.b,
                               render.a
                              );

        int dy = 1;
        do //for(double dy = 1; dy <= radius; dy += 1.0)
        {
            // the 2.0 promotes everything to doubles
            int dx = std::sqrt((2.0 * render.radius * dy) - (dy * dy));

            SDL_RenderDrawLine(m_gRenderer,
                               int(render.xDst - dx),
                               int(render.yDst + dy - render.radius),
                               int(render.xDst + dx),
                               int(render.yDst + dy - render.radius));

            if(dy < render.radius) // Don't cross lines
            {
                SDL_RenderDrawLine(m_gRenderer,
                                   int(render.xDst - dx),
                                   int(render.yDst - dy + render.radius),
                                   int(render.xDst + dx),
                                   int(render.yDst - dy + render.radius));
            }

            dy += 1;
        } while(dy <= render.radius);
        break;
    }
    case RenderCallType::circle_hole:
    {
        SDL_SetRenderDrawColor(m_gRenderer,
                               render.r,
                               render.g,
                               render.b,
                               render.a
                              );

        int dy = 1;
        do //for(double dy = 1; dy <= radius; dy += 1.0)
        {
            // the 2.0 promotes everything to doubles
            int dx = std::sqrt((2.0 * render.radius * dy) - (dy * dy));

            SDL_RenderDrawLine(m_gRenderer,
                               int(render.xDst - render.radius),
                               int(render.yDst + dy - render.radius),
                               int(render.xDst - dx),
                               int(render.yDst + dy - render.radius));

            SDL_RenderDrawLine(m_gRenderer,
                               int(render.xDst + dx),
                               int(render.yDst + dy - render.radius),
                               int(render.xDst + render.radius),
                               int(render.yDst + dy - render.radius));


            if(dy < render.radius) // Don't cross lines
            {
                SDL_RenderDrawLine(m_gRenderer,
                                   int(render.xDst - render.radius),
                                   int(render.yDst - dy + render.radius),
                                   int(render.xDst - dx),
                                   int(render.yDst - dy + render.radius));

                SDL_RenderDrawLine(m_gRenderer,
                                   int(render.xDst + dx),
                                   int(render.yDst - dy + render.radius),
                                   int(render.xDst + render.radius),
                                   int(render.yDst - dy + render.radius));
            }

            dy += 1;
        } while(dy <= render.radius);
        break;
    }
    case RenderCallType::texture:
    {
        const SDL_RendererFlip flip = (SDL_RendererFlip)(render.features & 3);
        if(!render.texture)
            break;

        StdPicture &tx = *render.texture;

        if(!tx.d.texture && tx.l.lazyLoaded)
            lazyLoad(tx);

        if(!tx.d.texture)
        {
            D_pLogWarningNA("Attempt to render an empty texture!");
            break;
        }

        SDL_assert_release(tx.d.texture);

        if(render.features & RenderFeatures::color)
            txColorMod(tx.d, render.r, render.g, render.b, render.a);
        else
            txColorMod(tx.d, 255, 255, 255, 255);

        // set up the source rect
        SDL_Rect sourceRect;
        SDL_Rect* sourceRectPtr = nullptr;

        if(render.features & RenderFeatures::src_rect)
        {
            sourceRect.x = render.xSrc;
            sourceRect.y = render.ySrc;
            sourceRect.w = render.wSrc;
            sourceRect.h = render.hSrc;

            if(sourceRect.x + sourceRect.w > tx.w)
            {
                if(sourceRect.x > tx.w)
                    break;
                sourceRect.w = tx.w - sourceRect.x;
            }

            if(sourceRect.y + sourceRect.h > tx.h)
            {
                if(sourceRect.y > tx.h)
                    break;
                sourceRect.h = tx.h - sourceRect.y;
            }

            sourceRectPtr = &sourceRect;
        }

        // set up the dest rect
        SDL_Rect destRect;
        destRect.x = render.xDst;
        destRect.y = render.yDst;

        if(render.features & RenderFeatures::scaling)
        {
            destRect.w = render.wDst;
            destRect.h = render.hDst;
        }
        else if(render.features & RenderFeatures::src_rect)
        {
            destRect.w = sourceRect.w;
            destRect.h = sourceRect.h;
        }
        else
        {
            destRect.w = tx.w;
            destRect.h = tx.h;
        }

        // if using source rect, correct it for scaling factor
        if(sourceRectPtr && (tx.l.w_orig != 0 || tx.l.h_orig != 0))
        {
            sourceRect.x *= tx.l.w_scale;
            sourceRect.y *= tx.l.h_scale;
            sourceRect.w *= tx.l.w_scale;
            sourceRect.h *= tx.l.h_scale;
        }

        // need to select an SDL render signature based on features used
        if(flip || render.features & RenderFeatures::rotation)
        {
            double angle;
            if(render.features & RenderFeatures::rotation)
                angle = double(render.angle) * (360. / 65536.);
            else
                angle = 0.;

            SDL_RenderCopyEx(m_gRenderer, tx.d.texture, sourceRectPtr, &destRect,
                      angle, nullptr, flip);
        }
        else
        {
            SDL_RenderCopy(m_gRenderer, tx.d.texture, sourceRectPtr, &destRect);
        }
    }
    }
}

void RenderSDL::textureToScreen()
{
    int w, h, off_x, off_y, wDst, hDst;
    float scale_x, scale_y;

    // Get the size of surface where to draw the scene
    SDL_GetRendererOutputSize(m_gRenderer, &w, &h);

    // Calculate the size difference factor
    scale_x = float(w) / ScaleWidth;
    scale_y = float(h) / ScaleHeight;

    wDst = w;
    hDst = h;

    // Keep aspect ratio
    if(scale_x > scale_y) // Width more than height
    {
        wDst = int(scale_y * ScaleWidth);
        hDst = int(scale_y * ScaleHeight);
    }
    else if(scale_x < scale_y) // Height more than width
    {
        hDst = int(scale_x * ScaleHeight);
        wDst = int(scale_x * ScaleWidth);
    }

    // Align the rendering scene to the center of screen
    off_x = (w - wDst) / 2;
    off_y = (h - hDst) / 2;

    SDL_SetRenderDrawColor(m_gRenderer, 0, 0, 0, 255);
    SDL_RenderClear(m_gRenderer);

    SDL_Rect destRect = {off_x, off_y, wDst, hDst};
    SDL_Rect sourceRect = {0, 0, ScaleWidth, ScaleHeight};

    SDL_SetTextureColorMod(m_tBuffer, 255, 255, 255);
    SDL_SetTextureAlphaMod(m_tBuffer, 255);
    SDL_RenderCopy(m_gRenderer, m_tBuffer, &sourceRect, &destRect);
}

void RenderSDL::finalizeRender()
{
    SDL_RenderPresent(m_gRenderer);
}

void RenderSDL::getScreenPixels(int x, int y, int w, int h, unsigned char *pixels)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderReadPixels(m_gRenderer,
                         &rect,
                         SDL_PIXELFORMAT_BGR24,
                         pixels,
                         w * 3 + (w % 4));
}

void RenderSDL::getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderReadPixels(m_gRenderer,
                         &rect,
                         SDL_PIXELFORMAT_ABGR8888,
                         pixels,
                         w * 4);
}

int RenderSDL::getPixelDataSize(const StdPicture &tx)
{
    if(!tx.d.texture)
        return 0;
    return (tx.w * tx.h * 4);
}

void RenderSDL::getPixelData(const StdPicture &tx, unsigned char *pixelData)
{
    int pitch, w, h, a;
    void *pixels;

    if(!tx.d.texture)
        return;

    SDL_SetTextureBlendMode(tx.d.texture, SDL_BLENDMODE_BLEND);
    SDL_QueryTexture(tx.d.texture, nullptr, &a, &w, &h);
    SDL_LockTexture(tx.d.texture, nullptr, &pixels, &pitch);
    std::memcpy(pixelData, pixels, static_cast<size_t>(pitch) * h);
    SDL_UnlockTexture(tx.d.texture);
}
