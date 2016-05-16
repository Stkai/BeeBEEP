/****************************************************************************
 **
 ** Copyright (C) Qxt Foundation. Some rights reserved.
 **
 ** This file is part of the QxtGui module of the Qxt library.
 **
 ** This library is free software; you can redistribute it and/or modify it
 ** under the terms of the Common Public License, version 1.0, as published
 ** by IBM, and/or under the terms of the GNU Lesser General Public License,
 ** version 2.1, as published by the Free Software Foundation.
 **
 ** This file is provided "AS IS", without WARRANTIES OR CONDITIONS OF ANY
 ** KIND, EITHER EXPRESS OR IMPLIED INCLUDING, WITHOUT LIMITATION, ANY
 ** WARRANTIES OR CONDITIONS OF TITLE, NON-INFRINGEMENT, MERCHANTABILITY OR
 ** FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** You should have received a copy of the CPL and the LGPL along with this
 ** file. See the LICENSE file and the cpl1.0.txt/lgpl-2.1.txt files
 ** included with the source distribution for more information.
 ** If you did not receive a copy of the licenses, contact the Qxt Foundation.
 **
 ** <http://libqxt.org>  <foundation@libqxt.org>
 **
 ****************************************************************************/
#include "qxtglobalshortcut_p.h"
#include <qt_os2.h>


#define MOD_ALT         0x0001
#define MOD_CONTROL     0x0002
#define MOD_SHIFT       0x0004
#define MOD_WIN         0x0008

#define VK_LBUTTON 	0x01 	
#define VK_RBUTTON 	0x02 	
#define VK_CANCEL 	0x03 	
#define VK_MBUTTON 	0x04 	
#define VK_BACK 	0x08 	
#define VK_TAB 		0x09 
#define VK_CLEAR 	0x0C 	
#define VK_RETURN 	0x0D 	
#define VK_SHIFT 	0x10 	
#define VK_CONTROL 	0x11 	
#define VK_MENU 	0x12 	
#define VK_PAUSE 	0x13 	
#define VK_CAPITAL 	0x14 	
#define VK_ESCAPE 	0x1B 	
#define VK_SPACE 	0x20 	
#define VK_PRIOR 	0x21 	
#define VK_NEXT 	0x22 	
#define VK_END 		0x23 
#define VK_HOME 	0x24 	
#define VK_LEFT 	0x25 	
#define VK_UP 		0x26 	
#define VK_RIGHT 	0x27 	
#define VK_DOWN 	0x28 	
#define VK_SELECT 	0x29 	
#define VK_PRINT 	0x2A 	
#define VK_EXECUTE 	0x2B 	
#define VK_SNAPSHOT 	0x2C	
#define VK_INSERT 	0x2D 	
#define VK_DELETE 	0x2E 	
#define VK_HELP 	0x2F
#define VK_NUMPAD0 	0x60 	
#define VK_NUMPAD1 	0x61 	
#define VK_NUMPAD2 	0x62 	
#define VK_NUMPAD3 	0x63 	
#define VK_NUMPAD4 	0x64 	
#define VK_NUMPAD5 	0x65 	
#define VK_NUMPAD6 	0x66 	
#define VK_NUMPAD7 	0x67 	
#define VK_NUMPAD8 	0x68 	
#define VK_NUMPAD9 	0x69 	
#define VK_SEPARATOR 	0x6C 	
#define VK_SUBTRACT 	0x6D 	
#define VK_DECIMAL 	0x6E 
#define VK_DIVIDE 	0x6F 
#define VK_F1 		0x70 	
#define VK_F2 	0x71 	
#define VK_F3 	0x72 	
#define VK_F4 	0x73 	
#define VK_F5 	0x74 	
#define VK_F6 	0x75 	
#define VK_F7 	0x76 	
#define VK_F8 	0x77 	
#define VK_F9 	0x78 	
#define VK_F10 	0x79 	
#define VK_F11 	0x7A 	
#define VK_F12 	0x7B 	
#define VK_F13 	0x7C 	
#define VK_F14 	0x7D 	
#define VK_F15 	0x7E 	
#define VK_F16 	0x7F 	
#define VK_F17 	0x80
#define VK_F18 	0x81 	
#define VK_F19 	0x82 	
#define VK_F20 	0x83 	
#define VK_F21 	0x84 	
#define VK_F22 	0x85 	
#define VK_F23 	0x86 	
#define VK_F24 	0x87 	
#define VK_NUMLOCK 	0x90 	
#define VK_SCROLL 	0x91 	
#define VK_LSHIFT 	0xA0 	
#define VK_RSHIFT 	0xA1 	
#define VK_LCONTROL 	0xA2
#define VK_RCONTROL 	0xA3
#define VK_LMENU 	0xA4 	
#define VK_RMENU 	0xA5 	
#define VK_PLAY 	0xFA 	
#define VK_ZOOM 	0xFB 	

typedef unsigned long DWORD;
typedef unsigned short WORD;
#define LOWORD(a) ((WORD)(a))
#define HIWORD(a) ((WORD)(((DWORD)(a) >> 16) & 0xFFFF))

// TODO: fixme

bool QxtGlobalShortcutPrivate::eventFilter(void* message)
{
/*
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        const quint32 keycode = HIWORD(msg->lParam);
        const quint32 modifiers = LOWORD(msg->lParam);
        activateShortcut(keycode, modifiers);
    }
*/
    return false;
}

quint32 QxtGlobalShortcutPrivate::nativeModifiers(Qt::KeyboardModifiers modifiers)
{
    // MOD_ALT, MOD_CONTROL, (MOD_KEYUP), MOD_SHIFT, MOD_WIN
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier)
        native |= MOD_SHIFT;
    if (modifiers & Qt::ControlModifier)
        native |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier)
        native |= MOD_ALT;
    if (modifiers & Qt::MetaModifier)
        native |= MOD_WIN;
    // TODO: resolve these?
    //if (modifiers & Qt::KeypadModifier)
    //if (modifiers & Qt::GroupSwitchModifier)
    return native;
}

quint32 QxtGlobalShortcutPrivate::nativeKeycode(Qt::Key key)
{
    switch (key)
    {
    case Qt::Key_Escape:
        return VK_ESCAPE;
    case Qt::Key_Tab:
    case Qt::Key_Backtab:
        return VK_TAB;
    case Qt::Key_Backspace:
        return VK_BACK;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        return VK_RETURN;
    case Qt::Key_Insert:
        return VK_INSERT;
    case Qt::Key_Delete:
        return VK_DELETE;
    case Qt::Key_Pause:
        return VK_PAUSE;
    case Qt::Key_Print:
        return VK_PRINT;
    case Qt::Key_Clear:
        return VK_CLEAR;
    case Qt::Key_Home:
        return VK_HOME;
    case Qt::Key_End:
        return VK_END;
    case Qt::Key_Left:
        return VK_LEFT;
    case Qt::Key_Up:
        return VK_UP;
    case Qt::Key_Right:
        return VK_RIGHT;
    case Qt::Key_Down:
        return VK_DOWN;
    case Qt::Key_PageUp:
        return VK_PRIOR;
    case Qt::Key_PageDown:
        return VK_NEXT;
    case Qt::Key_F1:
        return VK_F1;
    case Qt::Key_F2:
        return VK_F2;
    case Qt::Key_F3:
        return VK_F3;
    case Qt::Key_F4:
        return VK_F4;
    case Qt::Key_F5:
        return VK_F5;
    case Qt::Key_F6:
        return VK_F6;
    case Qt::Key_F7:
        return VK_F7;
    case Qt::Key_F8:
        return VK_F8;
    case Qt::Key_F9:
        return VK_F9;
    case Qt::Key_F10:
        return VK_F10;
    case Qt::Key_F11:
        return VK_F11;
    case Qt::Key_F12:
        return VK_F12;
    case Qt::Key_F13:
        return VK_F13;
    case Qt::Key_F14:
        return VK_F14;
    case Qt::Key_F15:
        return VK_F15;
    case Qt::Key_F16:
        return VK_F16;
    case Qt::Key_F17:
        return VK_F17;
    case Qt::Key_F18:
        return VK_F18;
    case Qt::Key_F19:
        return VK_F19;
    case Qt::Key_F20:
        return VK_F20;
    case Qt::Key_F21:
        return VK_F21;
    case Qt::Key_F22:
        return VK_F22;
    case Qt::Key_F23:
        return VK_F23;
    case Qt::Key_F24:
        return VK_F24;
    case Qt::Key_Space:
        return VK_SPACE;
    case Qt::Key_Comma:
        return VK_SEPARATOR;
    case Qt::Key_Minus:
        return VK_SUBTRACT;
    case Qt::Key_Slash:
        return VK_DIVIDE;

        // numbers
    case Qt::Key_0:
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        return key;

        // letters
    case Qt::Key_A:
    case Qt::Key_B:
    case Qt::Key_C:
    case Qt::Key_D:
    case Qt::Key_E:
    case Qt::Key_F:
    case Qt::Key_G:
    case Qt::Key_H:
    case Qt::Key_I:
    case Qt::Key_J:
    case Qt::Key_K:
    case Qt::Key_L:
    case Qt::Key_M:
    case Qt::Key_N:
    case Qt::Key_O:
    case Qt::Key_P:
    case Qt::Key_Q:
    case Qt::Key_R:
    case Qt::Key_S:
    case Qt::Key_T:
    case Qt::Key_U:
    case Qt::Key_V:
    case Qt::Key_W:
    case Qt::Key_X:
    case Qt::Key_Y:
    case Qt::Key_Z:
        return key;

    default:
        return 0;
    }
}

bool QxtGlobalShortcutPrivate::registerShortcut(quint32 nativeKey, quint32 nativeMods)
{
return true;
    //return RegisterHotKey(0, nativeMods ^ nativeKey, nativeMods, nativeKey);
}

bool QxtGlobalShortcutPrivate::unregisterShortcut(quint32 nativeKey, quint32 nativeMods)
{
return true;
    //return UnregisterHotKey(0, nativeMods ^ nativeKey);
}
