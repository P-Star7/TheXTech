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
#ifndef RENDERSDL_T_H
#define RENDERSDL_T_H

#include <set>

#include "../base/render_base.h"
#include "cmd_line_setup.h"


struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Window;

class RenderSDL final : public AbstractRender_t
{
    SDL_Window   *m_window = nullptr;

    SDL_Renderer *m_gRenderer = nullptr;
    SDL_Texture  *m_tBuffer = nullptr;
    SDL_Texture  *m_recentTarget = nullptr;
    std::set<SDL_Texture *> m_textureBank;

    // Scale of virtual and window resolutuins
    float m_scale_x = 1.f;
    float m_scale_y = 1.f;
    // Side offsets to keep ratio
    float m_offset_x = 0.f;
    float m_offset_y = 0.f;

    //Need to calculate relative viewport position when screen was scaled
    float m_viewport_scale_x = 1.0f;
    float m_viewport_scale_y = 1.0f;

public:
    RenderSDL();
    ~RenderSDL() override;


    unsigned int SDL_InitFlags() override;

    bool isWorking() override;

    bool initRender(const CmdLineSetup_t &setup, SDL_Window *window);

    /*!
     * \brief Close the renderer
     */
    void close() override;

    /*!
     * \brief Update viewport (mainly after screen resize)
     */
    void updateViewport() override;

    /*!
     * \brief Updates the internal viewport to reflect the updated viewport
     */
    void updateViewportInternal();

    void mapToScreen(int x, int y, int *dx, int *dy) override;

    void mapFromScreen(int x, int y, int *dx, int *dy) override;

    /*!
     * \brief Set render target into the virtual in-game screen (use to render in-game world)
     */
    void setTargetTexture() override;

    /*!
     * \brief Set render target into the real window or screen (use to render on-screen buttons and other meta-info)
     */
    void setTargetScreen() override;


    void loadTexture(StdPicture &target,
                     uint32_t width,
                     uint32_t height,
                     uint8_t *RGBApixels,
                     uint32_t pitch) override;

    void deleteTexture(StdPicture &tx, bool lazyUnload = false) override;
    void clearAllTextures() override;

    void clearBuffer() override;


    // Actual draw functions
    void executeRender(const RenderCall_t &render, int16_t depth) override;
    void textureToScreen() override;
    void finalizeRender() override;


    // Retrieve raw pixel data

    void getScreenPixels(int x, int y, int w, int h, unsigned char *pixels) override;

    void getScreenPixelsRGBA(int x, int y, int w, int h, unsigned char *pixels) override;

    int  getPixelDataSize(const StdPicture &tx) override;

    void getPixelData(const StdPicture &tx, unsigned char *pixelData) override;

};


#endif // RENDERSDL_T_H
