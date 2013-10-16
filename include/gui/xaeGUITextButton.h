/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: GUI Text Button
 */
#ifndef XAEGUITEXTBUTTON_H
#define XAEGUITEXTBUTTON_H
#pragma once
#include "../xaeSprite.h"
#include "../xaeSound.h"
#include "xaeGUIButton.h"
#include "../xaeFont.h"

class XAEEXP xaeGUITextButton : public xaeGUIButton
{
public:
    xaeGUITextButton(int id, string key, string path_format, int x, int y, string text, string font_name = "ºÚÌå", int font_size = 12, DWORD font_color = 0xff000000, DWORD font_hover_color = 0xff000000, bool bold = false, bool italian = false, bool anti = true, string sound_hover = "", string sound_click = "", string enter_mouse = "");
    virtual ~xaeGUITextButton();

    virtual void                on_render(int x, int y);

protected:
    string                      m_szText;
    DWORD                       m_dwColor;
    DWORD                       m_dwHoverColor;
    xaeFont*                    m_pFont;

    SIZE                        m_szTextPos;
};

#endif
