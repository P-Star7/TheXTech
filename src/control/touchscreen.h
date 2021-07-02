/*
 * TheXTech - A platform game engine ported from old source code for VB6
 *
 * Copyright (c) 2009-2011 Andrew Spinks, original VB6 code
 * Copyright (c) 2020-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <SDL2/SDL_touch.h>
#include <map>
#include "../controls.h"

#ifdef __ANDROID__
#define HAS_VIBRATOR
typedef struct _SDL_Haptic SDL_Haptic;
#endif

/*!
 * \brief A mobile touch-screen controller which reads state of the keyboard device
 */
class TouchScreenController
{
    //! Count of touch devices
    int m_touchDevicesCount = 0;
    //! Physical screen width
    int m_screenWidth = 0;
    //! Physical screen height
    int m_screenHeight = 0;
    //! Actual touch device to use
    int m_actualDevice = -1;

#ifdef HAS_VIBRATOR
    SDL_Haptic *m_vibrator = nullptr;
#endif

    /*!
     * \brief Is touch-screen supported?
     */
    bool touchSupported();

public:

    enum commands
    {
        key_BEGIN = 0,
        key_start = 0,
        key_left,
        key_right,
        key_up,
        key_down,
        key_upleft,
        key_upright,
        key_downleft,
        key_downright,
        key_run,
        key_jump,
        key_altrun,
        key_altjump,
        key_drop,
        key_holdRun,
        key_toggleKeysView,
        key_END
    };

    //! In-game controls pressed
    Controls_t m_current_keys;
    bool m_keysHeld[key_END] = {false};

    struct ExtraKeys
    {
        bool keyToggleView = false;
        bool keyToggleViewOnce = false;

        bool keyHoldRun = false;
        bool keyHoldRunOnce = false;

        bool keyRunOnce = false;
        bool keyAltRunOnce = false;
    } m_current_extra_keys;

    //! Touch hidden by default, re-enablable by left-top corder
    bool       m_touchHidden = true;
    bool       m_runHeld = false;

    struct FingerState
    {
        bool alive = false;
        bool heldKey[key_END] = {};
        bool heldKeyPrev[key_END] = {};

        FingerState();
        FingerState(const FingerState &fs);
        FingerState &operator=(const FingerState &fs);
    };

private:
    //! Held finger states
    std::map<SDL_FingerID, FingerState> m_fingers;

    void doVibration();

public:
    /*!
     * \brief Constructor
     */
    TouchScreenController();

    /*!
     * \brief Destructor
     */
    ~TouchScreenController();

    void init();
    void quit();

    void updateScreenSize();

    /*!
     * \brief Read current state of keyboard controller
     */
    void update();

    void render();

    void processTouchDevice(int dev_i);
};

#endif // TOUCHSCREEN_H
