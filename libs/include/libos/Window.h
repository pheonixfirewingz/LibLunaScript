/*!
 * \file Window.h
 * \author Luke Shore <lukethemodded@gmail.com>
 * \version 1.0
 * \date 05/12/2022
 * \brief WINDOW is a simple cross-platform API for accessing the native platform window, keyboard and mouse io
 */
#pragma once
// LIBOS LICENCE
//
// GNU Lesser General Public License Version 3.0
//
// Copyright Luke Shore (c) 2020, 2022
/*! Importation of libraries*/
#include <libos/Defines.h>
/*!
 *  \brief this is the Library object that is used to Control the Native  as the
 * user only holds a pointer to the library object that is controlled by the library.
 */
typedef struct losWindow_T *losWindow;

typedef enum losMouseButton
{
    LOS_RIGHT_BUTTON = 0,
    LOS_MIDDLE_BUTTON = 1,
    LOS_LEFT_BUTTON = 2,
} losMouseButton;

typedef enum losKeyboardButton
{
    LOS_KEY_0,
    LOS_KEY_1,
    LOS_KEY_2,
    LOS_KEY_3,
    LOS_KEY_4,
    LOS_KEY_5,
    LOS_KEY_6,
    LOS_KEY_7,
    LOS_KEY_8,
    LOS_KEY_9,
    LOS_KEY_A,
    LOS_KEY_B,
    LOS_KEY_C,
    LOS_KEY_D,
    LOS_KEY_E,
    LOS_KEY_F,
    LOS_KEY_G,
    LOS_KEY_H,
    LOS_KEY_I,
    LOS_KEY_J,
    LOS_KEY_K,
    LOS_KEY_L,
    LOS_KEY_M,
    LOS_KEY_N,
    LOS_KEY_O,
    LOS_KEY_P,
    LOS_KEY_Q,
    LOS_KEY_R,
    LOS_KEY_S,
    LOS_KEY_T,
    LOS_KEY_U,
    LOS_KEY_V,
    LOS_KEY_W,
    LOS_KEY_X,
    LOS_KEY_Y,
    LOS_KEY_Z,
    LOS_KEY_APOSTROPHE,
    LOS_KEY_BACKSLASH,
    LOS_KEY_COMMA,
    LOS_KEY_LEFT_BRACKET,
    LOS_KEY_MINUS,
    LOS_KEY_PERIOD,
    LOS_KEY_RIGHT_BRACKET,
    LOS_KEY_SEMICOLON,
    LOS_KEY_SLASH,
    LOS_KEY_BACKSPACE,
    LOS_KEY_DELETE,
    LOS_KEY_END,
    LOS_KEY_ENTER,
    LOS_KEY_ESCAPE,
    LOS_KEY_HOME,
    LOS_KEY_INSERT,
    LOS_KEY_PAGE_DOWN,
    LOS_KEY_PAGE_UP,
    LOS_KEY_PAUSE,
    LOS_KEY_SPACE,
    LOS_KEY_TAB,
    LOS_KEY_CAPS_LOCK,
    LOS_KEY_NUM_LOCK,
    LOS_KEY_SCROLL_LOCK,
    LOS_KEY_F1,
    LOS_KEY_F2,
    LOS_KEY_F3,
    LOS_KEY_F4,
    LOS_KEY_F5,
    LOS_KEY_F6,
    LOS_KEY_F7,
    LOS_KEY_F8,
    LOS_KEY_F9,
    LOS_KEY_F10,
    LOS_KEY_F11,
    LOS_KEY_F12,
    LOS_KEY_ALT,
    LOS_KEY_LEFT_ALT,
    LOS_KEY_RIGHT_ALT,
    LOS_KEY_CONTROL,
    LOS_KEY_LEFT_CONTROL,
    LOS_KEY_RIGHT_CONTROL,
    LOS_KEY_SHIFT,
    LOS_KEY_LEFT_SHIFT,
    LOS_KEY_RIGHT_SHIFT,
    LOS_KEY_LEFT_SUPER,
    LOS_KEY_RIGHT_SUPER,
    LOS_KEY_PRINT_SCREEN,
    LOS_KEY_DOWN,
    LOS_KEY_LEFT,
    LOS_KEY_RIGHT,
    LOS_KEY_UP,
    LOS_KEY_NUM_PAD_0,
    LOS_KEY_NUM_PAD_1,
    LOS_KEY_NUM_PAD_2,
    LOS_KEY_NUM_PAD_3,
    LOS_KEY_NUM_PAD_4,
    LOS_KEY_NUM_PAD_5,
    LOS_KEY_NUM_PAD_6,
    LOS_KEY_NUM_PAD_7,
    LOS_KEY_NUM_PAD_8,
    LOS_KEY_NUM_PAD_9,
    LOS_KEY_ADD,
    LOS_KEY_DECIMAL,
    LOS_KEY_DIVIDE,
    LOS_KEY_EQUAL,
    LOS_KEY_MULTIPLY,
    LOS_KEY_SUBTRACT,
} losKeyboardButton;

typedef struct losWindowInfo
{
    /*!< 
     * this is used for supported platforms will talk to the kernel to draw to the display if the
     * backend dose not support it will revert to the default behavior
     * ALERT: this is not to be used in applications to act as full-screen as it takes over any
     * window manager the OS may have this could break your OS.
     *
     * !!!USE "is_compositeServer" AT YOUR OWN RISK!!
     * (NOT_IMPLEMENTED)
     */
    uint8_t is_compositeServer;
    losSize window_size;
    const char *title;
    data_size_t title_size;
} losWindowInfo;

EXPORT_DLL enum losResult losCreateWindow(losWindow *, losWindowInfo);
EXPORT_DLL enum losResult losCreateKeyboard(losWindow);
EXPORT_DLL enum losResult losCreateMouse(losWindow);
EXPORT_DLL enum losResult losCreateTouch(losWindow);

// the way to check if the window should close check to see if function returns "LOS_WINDOW_CLOSE"
EXPORT_DLL enum losResult losUpdateWindow(losWindow);
//--------------------------------------------------------------------------------------------------------------------------
EXPORT_DLL uint8_t losIsKeyDown(losWindow, losKeyboardButton);
EXPORT_DLL uint8_t losIsMouseDown(losWindow, losMouseButton);
EXPORT_DLL enum losResult losRequestClose(losWindow);
EXPORT_DLL losSize losRequestMousePosition(losWindow);
EXPORT_DLL losSize losRequestMouseWheelDelta(losWindow);
EXPORT_DLL losSize losIsBeingPressed(losWindow);

EXPORT_DLL void losDestroyKeyboard(losWindow);
EXPORT_DLL void losDestroyMouse(losWindow);
EXPORT_DLL void losDestroyTouch(losWindow);
EXPORT_DLL void losDestroyWindow(losWindow);