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
#ifndef ABTRACTRENDER_T_H
#define ABTRACTRENDER_T_H

#include <string>
#include <vector>
#include <algorithm>
#include <SDL2/SDL_assert.h>

#include "std_picture.h"

#ifndef __EMSCRIPTEN__
#   define USE_SCREENSHOTS_AND_RECS
#   define USE_DRAW_BATTERY_STATUS
struct GifRecorder;
#endif

#ifdef __ANDROID__
#   define USE_RENDER_BLOCKING
#endif

#ifdef __ANDROID__
#   define RENDER_FULLSCREEN_ALWAYS
#endif


typedef struct SDL_Thread SDL_Thread;
typedef struct SDL_mutex SDL_mutex;


enum RendererFlip_t
{
    X_FLIP_NONE       = 0x00000000,    /**< Do not flip */
    X_FLIP_HORIZONTAL = 0x00000001,    /**< flip horizontally */
    X_FLIP_VERTICAL   = 0x00000002     /**< flip vertically */
};

struct FPoint_t
{
    float x;
    float y;
};

enum class RenderCallType : uint8_t
{
    texture,
    rect,
    circle,
    circle_hole
};

namespace RenderFeatures
{
    constexpr uint8_t flip_X = 1;
    constexpr uint8_t flip_Y = 2;
    constexpr uint8_t color = 4;
    constexpr uint8_t src_rect = 8;
    constexpr uint8_t scaling = 16;
    constexpr uint8_t rotation = 32;
    constexpr uint8_t filled = 64;
};

// carefully designed to consume max of 32 bytes
struct RenderCall_t
{
    RenderCallType type;
    uint8_t features;

    int16_t xDst, yDst, wDst, hDst;
    int16_t xSrc, ySrc, wSrc, hSrc;
    uint8_t r, g, b, a;
    union
    {
        uint16_t angle;
        uint16_t radius;
    };
    StdPicture* texture;
};

struct RenderQueue_t
{
    std::vector<RenderCall_t> m_calls;
    std::vector<int32_t> m_indices;
    uint16_t m_size = 0;

    inline void clear()
    {
        m_calls.clear();
        m_indices.clear();
        m_size = 0;
    }

    inline RenderCall_t& push(int16_t depth)
    {
        if(m_size == UINT16_MAX)
        {
            SDL_assert_release(false); // render queue overflow
            return m_calls[m_size - 1];
        }

        m_calls.emplace_back();
        m_indices.push_back((int32_t)(((uint32_t)depth << 16) | m_size));
        return m_calls[m_size++];
    }

    inline void sort()
    {
        std::sort(m_indices.begin(), m_indices.end());
    }
};

class AbstractRender_t
{
    friend class FrmMain;

    static size_t m_lazyLoadedBytes;

protected:
    //! Maximum texture width
    static int    m_maxTextureWidth;
    //! Maximum texture height
    static int    m_maxTextureHeight;

    static int    ScaleWidth;
    static int    ScaleHeight;

#ifdef USE_RENDER_BLOCKING
    static bool m_blockRender;
#endif

    RenderQueue_t m_renderQueue;
    bool m_directRender = false;

    // Offset to shake screen
    int m_viewport_offset_x = 0;
    int m_viewport_offset_y = 0;
    // Keep zero viewport offset while this flag is on
    bool m_viewport_offset_ignore = false;
    // Carried set value for viewport offset (used to preserve values while ignore option is on)
    int m_viewport_offset_x_cur = 0;
    int m_viewport_offset_y_cur = 0;

    bool m_viewport_enabled = false;
    int m_viewport_x = 0;
    int m_viewport_y = 0;
    int m_viewport_w = 0;
    int m_viewport_h = 0;

public:
    AbstractRender_t();
    virtual ~AbstractRender_t();


    /*!
     * \brief Flags needed to initialize SDL-based window
     * \return Bitwise flags of SDL Window or 0 if no special flags set
     */
    virtual unsigned int SDL_InitFlags() = 0;


    /*!
     * \brief Identify does render engine works or not
     * \return true if render initialized and works
     */
    virtual bool isWorking() = 0;

    /*!
     * \brief Initialize defaults of the renderer
     * \return false on error, true on success
     */
    virtual bool init();

    /*!
     * \brief Close the renderer
     */
    virtual void close();

    /*!
     * \brief Flushes the render queue (if it exists)
     */
    void flushRenderQueue();

    /*!
     * \brief Call the repaint
     */
    void repaint();

    /*!
     * \brief Update viewport (mainly after screen resize)
     */
    virtual void updateViewport() = 0;

    /*!
     * \brief Reset viewport into default state
     */
    void resetViewport();

    /*!
     * \brief Set the viewport area
     * \param x X position
     * \param y Y position
     * \param w Viewport Width
     * \param h Viewport Height
     */
    void setViewport(int x, int y, int w, int h);

    /*!
     * \brief Set the render offset
     * \param x X offset
     * \param y Y offset
     *
     * All drawing objects will be drawn with a small offset
     */
    void offsetViewport(int x, int y); // for screen-shaking

    /*!
     * \brief Set temporary ignore of render offset
     * \param en Enable viewport offset ignore
     *
     * Use this to draw certain objects with ignorign of the GFX offset
     */
    void offsetViewportIgnore(bool en);

    /*!
     * \brief Updates the internal viewport to reflect the updated viewport
     */
    virtual void updateViewportInternal() = 0;

    /*!
     * \brief Map absolute point coordinate into screen relative
     * \param x Window X position
     * \param y Window Y position
     * \param dx Destinition on-screen X position
     * \param dy Destinition on-screen Y position
     */
    virtual void mapToScreen(int x, int y, int *dx, int *dy) = 0;

    /*!
     * \brief Map screen relative coordinate into physical canvas
     * \param x On-screen X position
     * \param y On-screen Y position
     * \param dx Destinition window X position
     * \param dy Destinition window Y position
     */
    virtual void mapFromScreen(int x, int y, int *dx, int *dy) = 0;

    /*!
     * \brief Set render target into the virtual in-game screen (use to render in-game world)
     */
    virtual void setTargetTexture() = 0;

    /*!
     * \brief Set render target into the real window or screen (use to render on-screen buttons and other meta-info)
     */
    virtual void setTargetScreen() = 0;




    // Load and unload textures

    static StdPicture LoadPicture(const std::string &path,
                                  const std::string &maskPath = std::string(),
                                  const std::string &maskFallbackPath = std::string());

    static StdPicture lazyLoadPicture(const std::string &path,
                                      const std::string &maskPath = std::string(),
                                      const std::string &maskFallbackPath = std::string());

    static void setTransparentColor(StdPicture &target, uint32_t rgb);

    virtual void loadTexture(StdPicture &target,
                             uint32_t width,
                             uint32_t height,
                             uint8_t *RGBApixels,
                             uint32_t pitch) = 0;

    static void lazyLoad(StdPicture &target);
    static void lazyUnLoad(StdPicture &target);
    static void lazyPreLoad(StdPicture &target);

    static size_t lazyLoadedBytes();
    static void lazyLoadedBytesReset();

    virtual void deleteTexture(StdPicture &tx, bool lazyUnload = false) = 0;
    virtual void clearAllTextures() = 0;

    virtual void clearBuffer() = 0;



    // Actual draw function
    virtual void executeRender(const RenderCall_t &render, int16_t depth) = 0;
    virtual void textureToScreen() = 0;
    virtual void finalizeRender() = 0;


    // Draw primitives

    void renderRect(int x, int y, int w, int h,
                    float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f,
                    bool filled = true);

    void renderRectBR(int _left, int _top, int _right, int _bottom,
                      float red, float green, float blue, float alpha);

    void renderCircle(int cx, int cy,
                      int radius,
                      float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f,
                      bool filled = true);

    void renderCircleHole(int cx, int cy,
                          int radius,
                          float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);




    // Draw texture

    void renderTextureScaleEx(double xDst, double yDst, double wDst, double hDst,
                              StdPicture &tx,
                              int xSrc, int ySrc,
                              int wSrc, int hSrc,
                              double rotateAngle = 0.0, FPoint_t *center = nullptr, unsigned int flip = X_FLIP_NONE,
                              float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);

    void renderTextureScale(double xDst, double yDst, double wDst, double hDst,
                            StdPicture &tx,
                            float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);

    void renderTexture(double xDst, double yDst, double wDst, double hDst,
                               StdPicture &tx,
                               int xSrc, int ySrc,
                               float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);

    void renderTextureFL(double xDst, double yDst, double wDst, double hDst,
                                 StdPicture &tx,
                                 int xSrc, int ySrc,
                                 double rotateAngle = 0.0, FPoint_t *center = nullptr, unsigned int flip = X_FLIP_NONE,
                                 float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);

    void renderTexture(float xDst, float yDst, StdPicture &tx,
                               float red = 1.f, float green = 1.f, float blue = 1.f, float alpha = 1.f);



    // Retrieve raw pixel data

    virtual void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels) = 0;

    virtual void getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels) = 0;

    virtual int  getPixelDataSize(const StdPicture &tx) = 0;

    virtual void getPixelData(const StdPicture &tx, unsigned char *pixelData) = 0;



#ifdef USE_RENDER_BLOCKING
    static bool renderBlocked();
    static void setBlockRender(bool b);
#endif

    // Screenshots, GIF recordings, etc., etc.
#ifdef USE_DRAW_BATTERY_STATUS
    static void drawBatteryStatus();
#endif

#ifdef USE_SCREENSHOTS_AND_RECS
    static void makeShot();

    static void toggleGifRecorder();
    static void processRecorder();

private:
    static GifRecorder *m_gif;
    static bool recordInProcess();
#endif // USE_SCREENSHOTS_AND_RECS

};


//! Globally available renderer instance
extern AbstractRender_t* g_render;


#endif // ABTRACTRENDER_T_H
