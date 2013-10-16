#ifndef XAESELECTLABEL_H
#define XAESELECTLABEL_H
#pragma once
#include <xaeFont.h>
#include <string>
#include <gui/xaeGUIObject.h>
using namespace std;

class xaeSelectLabel : public xaeGUIObject
{
public:
    xaeSelectLabel(int id, string szText, int next_p, int idx, int x, int y, string fontname = "_default", int fontsize = 16, DWORD color = 0xffff7800, DWORD hcolor = 0xffff0099);
    virtual ~xaeSelectLabel(void);

    virtual void                on_render(int x, int y);
    virtual void                on_update(float fDT, int nMX, int nMY);

    virtual void                on_mouse_enter();
    virtual void                on_mouse_leave();

    int                         get_next_para();

    void                        set_color(DWORD color, DWORD hcolor);

private:
    xaeFont*                    m_pFont;
    string                      m_szText;
    int                         m_nNextPara;
    int                         m_nIdx;
    xaeMouseType                m_tagPreMouse;
    string                      m_szEnterMouse;

    DWORD                       m_dwColor;
    DWORD                       m_dwHoverColor;
};
#endif
