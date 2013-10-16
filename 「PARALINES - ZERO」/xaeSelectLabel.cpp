#include "xaeSelectLabel.h"
#include <xaeSpriteLoader.h>
#include "global.h"

xaeSelectLabel::xaeSelectLabel(int id, string szText, int next_p, int idx, int x, int y, string fontname, int fontsize, DWORD color, DWORD hcolor) :
    m_szEnterMouse(""),
    m_dwColor(color),
    m_dwHoverColor(hcolor)
{
    m_dwControlID = id;
    m_szControlKey = szText;
    m_szText = szText;
    m_nNextPara = next_p;

    /** 字体 */
    m_pFont = new xaeFont(fontname == "_default" ? g_szDefaultFont.c_str() : fontname.c_str(), fontsize);

    /** 文字大小 */
    SIZE sz = m_pFont->GetTextSize(C2W(szText.c_str()));

    set_pos(x, y);
    set_size(sz.cx, sz.cy);
}

xaeSelectLabel::~xaeSelectLabel(void)
{
    SAFEDEL(m_pFont);
}

void xaeSelectLabel::on_update(float fDT, int nMX, int nMY)
{
}

void xaeSelectLabel::on_render(int x, int y)
{
    if(!m_bMouseEnter)
    {
        m_pFont->SetColor(m_dwColor);
    }
    else
    {
        m_pFont->SetColor(m_dwHoverColor);
    }

    m_pFont->Print(x + m_tagRect.x1, y + m_tagRect.y1, m_szText.c_str());
}

void xaeSelectLabel::on_mouse_enter()
{
    /** 设置鼠标 */
    m_tagPreMouse = xaeGUIObject::get_mouse();
    set_mouse(string("media/cursor_point.png"));
}

void xaeSelectLabel::on_mouse_leave()
{
    if(m_tagPreMouse.bSprite) set_mouse(m_tagPreMouse.szPath);
    else set_mouse(m_tagPreMouse.uMouse.nMouse);
}

int xaeSelectLabel::get_next_para()
{
    return m_nNextPara;
}

void xaeSelectLabel::set_color(DWORD color, DWORD hcolor)
{
    m_dwColor = color, m_dwHoverColor = hcolor;
}