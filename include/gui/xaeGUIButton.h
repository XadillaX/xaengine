/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: GUI Button
 */
#ifndef XAEGUIBUTTON_H
#define XAEGUIBUTTON_H
#pragma once
#include "xaeGUIObject.h"
#include "../xaeSprite.h"
#include "../xaeSound.h"

class XAEEXP xaeGUIButton : public xaeGUIObject
{
public:
    /** 若path_format中有 '*' 间隔，则说明纹理由xaeSpriteLoader载入，并且 '*' 前后为纹理类型和纹理名 */
    xaeGUIButton(int id, string key, string path_format, int x, int y, string sound_hover = "", string sound_click = "", string enter_mouse = "");
    virtual ~xaeGUIButton();

    virtual void                on_render(int x, int y);
    virtual void                on_update(float fDT, int nMX, int nMY);

    virtual void                on_mouse_enter();
    virtual void                on_mouse_leave();
    virtual void                on_mouse_lbutton_clicked();

    xaeSound*                   get_hover_snd() { return m_pSoundHover; }
    xaeSound*                   get_click_snd() { return m_pSoundClick; }

private:
    xaeGUIButton() {};

protected:
    xaeSprite*                  m_pSprite[3];

    xaeSound*                   m_pSoundHover;
    xaeSound*                   m_pSoundClick;

    string                      m_szEnterMouse;
    xaeMouseType                m_tagPreMouse;
};

#endif