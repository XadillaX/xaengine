#include "xaeGUIAlphaButton.h"

xaeGUIAlphaButton::xaeGUIAlphaButton(int id, string key, string path_format, int x, int y, string sound_hover /* =  */, string sound_click /* =  */, string enter_mouse /* = */ ) :
    xaeGUIButton(id, key, path_format, x, y, sound_hover, sound_click, enter_mouse)
{
}

xaeGUIAlphaButton::~xaeGUIAlphaButton(void)
{
}

void xaeGUIAlphaButton::on_render(int x, int y)
{
    xaeSprite* spr = m_bMouseEnter ? (m_bMouseLButtonDown ? m_pSprite[2] : m_pSprite[1]) : m_pSprite[0];

    if(spr && spr->get_sprite())
    {
        if(m_bMouseEnter)
        {
            spr->get_sprite()->SetColor(0xffffffff);
        }
        else spr->get_sprite()->SetColor(0xaaffffff);
    }

    xaeGUIButton::on_render(x, y);
}