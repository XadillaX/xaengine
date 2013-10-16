#include <gui/xaeGUITextButton.h>

xaeGUITextButton::xaeGUITextButton(int id, string key, string path_format, int x, int y, string text, string font_name /* =  */, int font_size /* = 12 */, DWORD font_color /* = 0xff000000 */, DWORD font_hover_color /* = 0xff000000 */, bool bold /* = false */, bool italian /* = false */, bool anti /* = true */, string sound_hover /* =  */, string sound_click /* =  */, string enter_mouse /* = */ ) :
    xaeGUIButton(id, key, path_format, x, y, sound_hover, sound_click, enter_mouse),
    m_szText(text),
    m_dwColor(font_color),
    m_dwHoverColor(font_color)
{
    m_pFont = new xaeFont(font_name.c_str(), font_size, bold, italian, anti);

    SIZE sz = m_pFont->GetTextSize(C2W(text.c_str()));
    m_szTextPos.cx = (m_tagRect.x2 - m_tagRect.x1 - sz.cx) / 2;
    m_szTextPos.cy = (m_tagRect.y2 - m_tagRect.y1 - sz.cy) / 2;
}

xaeGUITextButton::~xaeGUITextButton()
{
    SAFEDEL(m_pFont);
}

void xaeGUITextButton::on_render(int x, int y)
{
    xaeGUIButton::on_render(x, y);

    if(this->get_mouse_capture_control() == this)
    {
        m_pFont->SetColor(m_dwHoverColor);
    }
    else m_pFont->SetColor(m_dwColor);

    m_pFont->Printf(x + m_tagRect.x1 + m_szTextPos.cx, y + m_tagRect.y1 + m_szTextPos.cy, C2W(m_szText.c_str()));
}
