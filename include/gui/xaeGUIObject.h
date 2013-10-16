/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: GUI Object
 */
#ifndef XAEGUIOBJECT_H
#define XAEGUIOBJECT_H
#pragma once
#include "ixaeobject.h"
#include "xaeSprite.h"
#include <vector>
#include <string>
using namespace std;

class xaeGUIObject;

/** 鼠标拖动结构体 */
struct XAEEXP XAE_MOUSE_DRAG_STRUCT
{
    xaePoint                    m_tagStart;
    xaePoint                    m_tagEnd;

    XAE_MOUSE_DRAG_STRUCT(int startx, int starty, int endx, int endy)
    {
        m_tagStart.x = startx, m_tagStart.y = starty;
        m_tagEnd.x = endx, m_tagEnd.y = endy;
    }
};

struct XAEEXP XAE_GUI_LIST_ELEMENT
{
    xaeGUIObject*               m_pGUI;
    bool                        m_bAutoDel;

    XAE_GUI_LIST_ELEMENT() { m_pGUI = NULL, m_bAutoDel = true; }
    XAE_GUI_LIST_ELEMENT(xaeGUIObject* gui, bool del) { m_pGUI = gui, m_bAutoDel = del; }
};

enum XAEEXP XAE_GUI_MOUSE_STATE
{
    GUIMS_LBUTTON_DOWN,
    GUIMS_LBUTTON_UP,
    GUIMS_LBUTTON_CLICK,
    GUIMS_RBUTTON_DOWN,
    GUIMS_RBUTTON_UP,
    GUIMS_RBUTTON_CLICK,
    GUIMS_MBUTTON_DOWN,
    GUIMS_MBUTTON_UP,
    GUIMS_MBUTTON_CLICK
};

typedef vector<XAE_GUI_LIST_ELEMENT> xaeGUIObjectList;

struct xaeMouseType
{
    union{
        xaeSprite*              pMouse;
        LPSTR                   nMouse;
    }                           uMouse;
    bool                        bSprite;
    bool                        bAutoDel;
    string                      szPath;

    xaeMouseType()              { bSprite = false; uMouse.nMouse = IDC_ARROW; bAutoDel = true; }
};

class XAEEXP xaeGUIObject : public iXAEObject
{
public:
    xaeGUIObject(int id = 0, string key = "noname");
    virtual ~xaeGUIObject();

    /** 虚函数 */
public:
    virtual void                on_render(int x, int y)                 {}
    //virtual void                on_render_children(xaeGUIObject* pObj, int x, int y) {}
    virtual void                on_update(float fDT, int nMX, int nMY)  {}

    virtual void                on_set_pos(int x, int y)                {}
    virtual void                on_set_size(int w, int h)               {}
    virtual void                on_set_show_mode(bool bShow)            {}
    virtual void                on_set_enable_mode(bool bEnabled)       {}
    virtual void                on_set_focus_control(xaeGUIObject* pObj){}

    virtual void                on_mouse_enter()                        {}
    virtual void                on_mouse_leave()                        {}
    virtual void                on_mouse_lbutton(bool bDown)            {}
    virtual void                on_mouse_lbutton_clicked()              {}
    virtual void                on_mouse_rbutton(bool bDown)            {}
    virtual void                on_mouse_rbutton_clicked()              {}
    virtual void                on_mouse_mbutton(bool bDown)            {}
    virtual void                on_mouse_mbutton_clicked()              {}
    virtual void                on_mouse_wheel(int time)                {}

    virtual LRESULT             on_wnd_proc(HWND hWnd, DWORD dwMessage, WPARAM& wParam, LPARAM& pParam) { return (LRESULT)0; }

public:
    static void                 set_mouse_capture_control(xaeGUIObject* obj) { xaeGUIObject::m_pMouseCaptureControl = obj; }
    static xaeGUIObject*        get_mouse_capture_control() { return xaeGUIObject::m_pMouseCaptureControl; }

    void                        set_parent(xaeGUIObject* pParent)       { m_pParent = pParent; }
    bool                        add_control(xaeGUIObject* pCtrl, bool bAutoDel = true);
    bool                        del_control(int dwControlID);
    bool                        del_control(xaeGUIObject* pCtrl);
    bool                        del_control(string szControlKey);
    xaeGUIObject*               get_control(int dwControlID);
    xaeGUIObject*               get_control(string szControlKey);

    void                        update(float fDT, int nMX, int nMY);
    void                        render(int x, int y);

    /** 获取类成员数据 */
    xaeGUIObject*               get_parent()                            { return m_pParent; }
    DWORD                       get_control_id()                        { return m_dwControlID; }
    string                      get_control_key()                       { return m_szControlKey; }
    xaeRect                     get_control_rect()                      { return m_tagRect; }

    bool                        get_show_mode()                         { return m_bShow; }
    bool                        get_enable_mode()                       { return m_bEnabled; }

    void                        set_pos(int x, int y);

    void                        set_size(int w, int h)
    {
        m_tagRect.Set(m_tagRect.x1, m_tagRect.y1, m_tagRect.x1 + w, m_tagRect.y1 + h);
        on_set_size(w, h);
    }

    void                        set_show_mode(bool bShow)
    {
        m_bShow = bShow;
        on_set_show_mode(bShow);

        for(size_t i = 0; i < m_pChildren.size(); i++)
        {
            m_pChildren[i].m_pGUI->set_show_mode(bShow);
        }

        if(!bShow)
        {
            if(xaeGUIObject::get_mouse_capture_control() == this)
            {
                xaeGUIObject::set_mouse_capture_control(NULL);
                on_mouse_leave();
            }

            m_bMouseLeave = true;
            m_bMouseEnter = false;
            m_bMouseDragging = false;
            m_bMouseLButtonClicked = false;
            m_bMouseLButtonDown = false;
            m_bMouseMButtonClicked = false;
            m_bMouseMButtonDown = false;
            m_bMouseRButtonClicked = false;
            m_bMouseRButtonDown = false;
            m_nMouseWheel = 0;
        }
    }
    void                        set_enable_mode(bool bEnabled)          
    {
        m_bEnabled = bEnabled;
        on_set_enable_mode(bEnabled);

        for(size_t i = 0; i < m_pChildren.size(); i++)
        {
            m_pChildren[i].m_pGUI->set_enable_mode(bEnabled);
        }

        if(!bEnabled)
        {
            if(xaeGUIObject::get_mouse_capture_control() == this)
            {
                xaeGUIObject::set_mouse_capture_control(NULL);
                on_mouse_leave();
            }

            m_bMouseLeave = true;
            m_bMouseEnter = false;
            m_bMouseDragging = false;
            m_bMouseLButtonClicked = false;
            m_bMouseLButtonDown = false;
            m_bMouseMButtonClicked = false;
            m_bMouseMButtonDown = false;
            m_bMouseRButtonClicked = false;
            m_bMouseRButtonDown = false;
            m_nMouseWheel = 0;
        }
    }
    void                        set_focused(bool focused)
    {
        if(focused)
        {
            xaeGUIObject::m_pFocusedControl = this;
            on_set_focus_control(this);
        }
        else
        {
            xaeGUIObject::m_pFocusedControl = NULL;
            on_set_focus_control(NULL);
        }
    }

    bool                        get_mouse_lbutton_state()               { return m_bMouseLButtonDown; }
    bool                        get_mouse_mbutton_state()               { return m_bMouseMButtonDown; }
    bool                        get_mouse_rbutton_state()               { return m_bMouseRButtonDown; }
    bool                        get_mouse_lbutton_clicked()             { return m_bMouseLButtonClicked; }
    bool                        get_mouse_mbutton_clicked()             { return m_bMouseMButtonClicked; }
    bool                        get_mouse_rbutton_clicked()             { return m_bMouseRButtonClicked; }
    bool                        get_mouse_inside()                      { return m_bMouseEnter; }
    bool                        is_children(xaeGUIObject* obj);

    /** 设置鼠标以及获取鼠标 */
    static void                 set_mouse(LPSTR nMouse);
    static void                 set_mouse(xaeSprite* pMouse, bool bAutoDel = false);
    /** 若szMousePath中有 '*' 间隔，则说明纹理由xaeSpriteLoader载入，并且 '*' 前后为纹理类型和纹理名 */
    static void                 set_mouse(string szMousePath);
    static void                 release_mouse();
    static void                 render_mouse();
    static xaeMouseType         get_mouse()                             { return xaeGUIObject::m_tagMouse; }

protected:
    void                        update_control(float fDT, int nMX, int nMY, bool LButton, bool RButton, bool MButton, int MouseWheel);

    xaeGUIObject*               find_children_mouse_state(XAE_GUI_MOUSE_STATE state);

protected:
    bool                        m_bShow;                                ///< 是否显示
    bool                        m_bEnabled;                             ///< 是否可用

    DWORD                       m_dwControlID;                          ///< 控件编号
    string                      m_szControlKey;                         ///< 控件名字

    xaeRect                     m_tagRect;                              ///< 位置矩形

    bool                        m_bMouseEnter;                          ///< 鼠标是否进入
    bool                        m_bMouseLeave;                          ///< 鼠标是否离开
    bool                        m_bMouseLButtonDown;                    ///< 鼠标左键是否按下
    bool                        m_bMouseLButtonClicked;                 ///< 鼠标左键是否被单击
    bool                        m_bMouseRButtonDown;                    ///< 鼠标右键是否按下
    bool                        m_bMouseRButtonClicked;                 ///< 鼠标右键是否被单击
    bool                        m_bMouseMButtonDown;                    ///< 鼠标中键是否按下
    bool                        m_bMouseMButtonClicked;                 ///< 鼠标中键是否被单击
    int                         m_nMouseWheel;                          ///< 鼠标滚轮圈数
    bool                        m_bMouseDragging;                       ///< 鼠标是否在拖动
    XAE_MOUSE_DRAG_STRUCT       m_tagDrag;                              ///< 鼠标拖动数据

    xaeGUIObject*               m_pParent;                              ///< 父GUI
    xaeGUIObjectList            m_pChildren;                            ///< 子GUI

    static xaeGUIObject*        m_pFocusedControl;                      ///< 在焦点之中的控件
    static xaeGUIObject*        m_pMouseCaptureControl;                 ///< 鼠标夺得的最上层控件
    static xaeMouseType         m_tagMouse;                             ///< 鼠标指针
};

#endif