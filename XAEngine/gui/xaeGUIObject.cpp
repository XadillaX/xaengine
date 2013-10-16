#include <gui/xaeGUIObject.h>
#include <xae.h>
#include <xaeSpriteLoader.h>

bool _gui_first_init = true;

xaeGUIObject* xaeGUIObject::m_pFocusedControl = NULL;
xaeGUIObject* xaeGUIObject::m_pMouseCaptureControl = NULL;
xaeMouseType xaeGUIObject::m_tagMouse;

xaeGUIObject::xaeGUIObject(int id, string key) :
    m_bShow(true),
    m_bEnabled(true),
    m_dwControlID(0),
    m_szControlKey(""),
    m_tagRect(0.0f, 0.0f, 0.0f, 0.0f),

    m_bMouseEnter(false),
    m_bMouseLeave(true),
    m_bMouseLButtonDown(false),
    m_bMouseLButtonClicked(false),
    m_bMouseRButtonDown(false),
    m_bMouseRButtonClicked(false),
    m_bMouseMButtonDown(false),
    m_bMouseMButtonClicked(false),
    m_nMouseWheel(0),
    m_bMouseDragging(false),
    m_tagDrag(0, 0, 0, 0),

    m_pParent(NULL)
{
    m_pHGE = hgeCreate(HGE_VERSION);

    m_dwControlID = id;
    m_szControlKey = key;
}

xaeGUIObject::~xaeGUIObject()
{
    xaeGUIObject::m_pFocusedControl = NULL;
    xaeGUIObject::m_pMouseCaptureControl = NULL;
    while(!m_pChildren.empty())
    {
        del_control(m_pChildren.front().m_pGUI);
    }

    m_pHGE->Release();
}

bool xaeGUIObject::add_control(xaeGUIObject* pCtrl, bool bAutoDel)
{
    if(!pCtrl) return false;

    /** 寻找冲突 */
    int size = m_pChildren.size();
    for(int i = 0; i < size; i++)
    {
        /** ID或者KEY冲突了 */
        if(pCtrl->get_control_id() == m_pChildren[i].m_pGUI->get_control_id() ||
            pCtrl->get_control_key() == m_pChildren[i].m_pGUI->get_control_key()
            )
        {
            m_pHGE->System_Log("[!] GUI [%d][%s] is conflicting.", pCtrl->get_control_id(), pCtrl->get_control_key().c_str());
            return false;
        }
    }

    /** 未发生任何冲突 */
    pCtrl->set_parent(this);
    XAE_GUI_LIST_ELEMENT tagGLE(pCtrl, bAutoDel);
    m_pChildren.push_back(tagGLE);

    return true;
}

bool xaeGUIObject::del_control(int dwControlID)
{
    /** 寻找控件 */
    xaeGUIObjectList::iterator it;
    for(it = m_pChildren.begin(); it != m_pChildren.end(); it++)
    {
        /** 找到了 */
        if((*it).m_pGUI->get_control_id() == dwControlID)
        {
            XAE_GUI_LIST_ELEMENT tagGLE = *it;
            m_pChildren.erase(it);

            tagGLE.m_pGUI->set_parent(NULL);

            /** 若是焦点 */
            if(xaeGUIObject::m_pFocusedControl == tagGLE.m_pGUI)
            {
                xaeGUIObject::m_pFocusedControl = NULL;
            }

            /** 若要自动删除 */
            if(tagGLE.m_bAutoDel == true)
            {
                delete tagGLE.m_pGUI;
            }

            return true;
        }
    }

    /** 未找到 */
    return false;
}

bool xaeGUIObject::del_control(xaeGUIObject* pCtrl)
{
    /** 寻找控件 */
    xaeGUIObjectList::iterator it;
    for(it = m_pChildren.begin(); it != m_pChildren.end(); it++)
    {
        /** 找到了 */
        if((*it).m_pGUI == pCtrl)
        {
            XAE_GUI_LIST_ELEMENT tagGLE = *it;
            m_pChildren.erase(it);

            tagGLE.m_pGUI->set_parent(NULL);

            /** 若是焦点 */
            if(xaeGUIObject::m_pFocusedControl == tagGLE.m_pGUI)
            {
                xaeGUIObject::m_pFocusedControl = NULL;
            }

            /** 若要自动删除 */
            if(tagGLE.m_bAutoDel == true)
            {
                delete tagGLE.m_pGUI;
            }

            return true;
        }
    }

    /** 未找到 */
    return false;
}

bool xaeGUIObject::del_control(string szControlKey)
{
    /** 寻找控件 */
    xaeGUIObjectList::iterator it;
    for(it = m_pChildren.begin(); it != m_pChildren.end(); it++)
    {
        /** 找到了 */
        if((*it).m_pGUI->get_control_key() == szControlKey)
        {
            XAE_GUI_LIST_ELEMENT tagGLE = *it;
            m_pChildren.erase(it);

            tagGLE.m_pGUI->set_parent(NULL);

            /** 若是焦点 */
            if(xaeGUIObject::m_pFocusedControl == tagGLE.m_pGUI)
            {
                xaeGUIObject::m_pFocusedControl = NULL;
            }

            /** 若要自动删除 */
            if(tagGLE.m_bAutoDel == true)
            {
                delete tagGLE.m_pGUI;
            }

            return true;
        }
    }

    /** 未找到 */
    return false;
}

xaeGUIObject* xaeGUIObject::get_control(int dwControlID)
{
    /** 寻找控件 */
    xaeGUIObjectList::iterator it;
    for(it = m_pChildren.begin(); it != m_pChildren.end(); it++)
    {
        /** 找到了 */
        if((*it).m_pGUI->get_control_id() == dwControlID)
        {
            return (*it).m_pGUI;
        }
    }

    /** 未找到 */
    return NULL;
}

xaeGUIObject* xaeGUIObject::get_control(string szControlKey)
{
    /** 寻找控件 */
    xaeGUIObjectList::iterator it;
    for(it = m_pChildren.begin(); it != m_pChildren.end(); it++)
    {
        /** 找到了 */
        if((*it).m_pGUI->get_control_key() == szControlKey)
        {
            return (*it).m_pGUI;
        }
    }

    /** 未找到 */
    return NULL;
}

void xaeGUIObject::update(float fDT, int nMX, int nMY)
{
    if(!m_bShow || !m_bEnabled) return;

    bool bLButton = xae::Instance().keystate(HGEK_LBUTTON);
    bool bRButton = xae::Instance().keystate(HGEK_RBUTTON);
    bool bMButton = xae::Instance().keystate(HGEK_MBUTTON);
    int nMouseWheel = m_pHGE->Input_GetMouseWheel();

    int nX = nMX - m_tagRect.x1;
    int nY = nMY - m_tagRect.y1;

    /** 更新子控件 */
    int size = m_pChildren.size();
    for(int i = 0; i < size; i++)
    {
        m_pChildren[i].m_pGUI->update(fDT, nX, nY);
    }

    /** 夺得鼠标所有权 */
    if(m_tagRect.TestPoint((float)nMX, (float)nMY))
    {
        /** 原鼠标所有权控件 */
        xaeGUIObject* obj = xaeGUIObject::get_mouse_capture_control();

        /** 若不是子控件 */
        if(!is_children(obj))
        {
            if(NULL != obj && this != obj)
            {
                obj->on_mouse_leave();
            }
            
            xaeGUIObject::set_mouse_capture_control(this);
        }
    }
    else
    /** 失去鼠标所有权 */
    {
        xaeGUIObject* obj = xaeGUIObject::get_mouse_capture_control();
        if(this == obj)
        {
            this->on_mouse_leave();
            xaeGUIObject::set_mouse_capture_control(NULL);
        }
    }

    /** 更新本身 */
    update_control(fDT, nX, nY, bLButton, bRButton, bMButton, nMouseWheel);
}

void xaeGUIObject::render(int x, int y)
{
    if(!m_bShow) return;

    /** 渲染本身 */
    on_render(x, y);

    /** 渲染子控件 */
    for(size_t i = 0; i < m_pChildren.size(); i++)
    {
        m_pChildren[i].m_pGUI->render(m_tagRect.x1 + x, m_tagRect.y1 + y);
    }
}

void xaeGUIObject::set_mouse(LPSTR nMouse)
{
    /** 删除原鼠标 */
    if(xaeGUIObject::m_tagMouse.bSprite && xaeGUIObject::m_tagMouse.bAutoDel)
    {
        xaeGUIObject::m_tagMouse.uMouse.pMouse->release();
        delete xaeGUIObject::m_tagMouse.uMouse.pMouse;
    }

    /** 显示鼠标 */
    xae::Instance().get_core()->System_SetState(HGE_HIDEMOUSE, false);

    xaeGUIObject::m_tagMouse.bSprite = false;
    xaeGUIObject::m_tagMouse.uMouse.nMouse = nMouse;
    xaeGUIObject::m_tagMouse.szPath = "";
}

void xaeGUIObject::set_mouse(xaeSprite* pMouse, bool bAutoDel)
{
    /** 隐藏鼠标 */
    xae::Instance().get_core()->System_SetState(HGE_HIDEMOUSE, true);

    /** 删除原鼠标 */
    if(xaeGUIObject::m_tagMouse.bSprite && xaeGUIObject::m_tagMouse.bAutoDel)
    {
        xaeGUIObject::m_tagMouse.uMouse.pMouse->release();
        SAFEDEL(xaeGUIObject::m_tagMouse.uMouse.pMouse);
        xaeGUIObject::m_tagMouse.bSprite = false;
    }

    xaeGUIObject::m_tagMouse.bSprite = true;
    xaeGUIObject::m_tagMouse.uMouse.pMouse = pMouse;
    xaeGUIObject::m_tagMouse.bAutoDel = bAutoDel;
    pMouse->get_sprite()->SetZ(1.0f);
    pMouse->get_sprite()->SetBlendMode(BLEND_DEFAULT_Z);
}

void xaeGUIObject::set_mouse(string szMousePath)
{
    /** 查找'*'并且生成鼠标 */
    xaeSprite* pMouse;
    int pos = szMousePath.find('*');
    if(string::npos != pos)
    {
        string type = szMousePath.substr(0, pos);
        string name = szMousePath.substr(pos + 1, szMousePath.length());
        pMouse = xaeSpriteLoader::Instance().get_sprite(type, name);
    }
    else pMouse = new xaeSprite(szMousePath.c_str());

    xaeGUIObject::set_mouse(pMouse, true);
    xaeGUIObject::m_tagMouse.szPath = szMousePath;
}

void xaeGUIObject::release_mouse()
{
    /** 若是精灵型鼠标且需要自动删除 */
    if(xaeGUIObject::m_tagMouse.bSprite && xaeGUIObject::m_tagMouse.bAutoDel)
    {
        xaeGUIObject::m_tagMouse.uMouse.pMouse->release();
        SAFEDEL(xaeGUIObject::m_tagMouse.uMouse.pMouse);
        xaeGUIObject::m_tagMouse.bSprite = false;
    }

    /** 复原鼠标 */
    xae::Instance().get_core()->System_SetState(HGE_HIDEMOUSE, false);
    xaeGUIObject::set_mouse(IDC_ARROW);
}

void xaeGUIObject::render_mouse()
{
    if(xaeGUIObject::m_tagMouse.bSprite)
    {
        float mx, my;
        xae::Instance().get_core()->Input_GetMousePos(&mx, &my);
        xaeGUIObject::m_tagMouse.uMouse.pMouse->render(mx, my);
    }
    else
    {
        HCURSOR hCur = LoadCursor(NULL, xaeGUIObject::m_tagMouse.uMouse.nMouse);
        SetCursor(hCur);
    }
}

void xaeGUIObject::update_control(float fDT, int nMX, int nMY, bool LButton, bool RButton, bool MButton, int MouseWheel)
{
    if(xaeGUIObject::get_mouse_capture_control() == this)
    {
        /** 本控件鼠标迁出 */
        if(!(nMX >= 0 && nMY >= 0 && nMX < (m_tagRect.x2 - m_tagRect.x1) && nMY < (m_tagRect.y2 - m_tagRect.y1)))
        {
            m_bMouseEnter = false;
            if(!m_bMouseLeave)
            {
                on_mouse_leave();
                m_bMouseLeave = true;
            }

            m_bMouseLButtonDown = false;
            m_bMouseMButtonDown = false;
            m_bMouseRButtonDown = false;
            m_bMouseLButtonClicked = false;
            m_bMouseRButtonClicked = false;
            m_bMouseMButtonClicked = false;
            m_nMouseWheel = 0;

            return;
        }

        /** 左键单击 */
        if(!LButton && m_bMouseLButtonDown) m_bMouseLButtonClicked = true;
        else m_bMouseLButtonClicked = false;
        /** 右键单击 */
        if(!RButton && m_bMouseRButtonDown) m_bMouseRButtonClicked = true;
        else m_bMouseRButtonClicked = false;
        /** 中键单击 */
        if(!MButton && m_bMouseMButtonDown) m_bMouseMButtonClicked = true;
        else m_bMouseMButtonClicked = false;

        m_bMouseLButtonDown = LButton;
        m_bMouseMButtonDown = MButton;
        m_bMouseRButtonDown = RButton;
        m_nMouseWheel = m_nMouseWheel;

        /** 鼠标进入 */
        m_bMouseLeave = false;
        if(!m_bMouseEnter)
        {
            on_mouse_enter();
            m_bMouseEnter = true;
        }

        on_mouse_lbutton(LButton);
        on_mouse_rbutton(RButton);
        on_mouse_mbutton(MButton);
        if(m_bMouseLButtonClicked) on_mouse_lbutton_clicked();
        if(m_bMouseMButtonClicked) on_mouse_mbutton_clicked();
        if(m_bMouseRButtonClicked) on_mouse_rbutton_clicked();
    }
    else
    {
        /** 鼠标迁出 */
        m_bMouseEnter = false;
        if(!m_bMouseLeave)
        {
            on_mouse_leave();
            m_bMouseLeave = true;
        }
    }

    /** 更新 */
    on_update(fDT, nMX, nMY);
}

xaeGUIObject* xaeGUIObject::find_children_mouse_state(XAE_GUI_MOUSE_STATE state)
{
    /** 看看是不是自己 */
    switch(state)
    {
    case GUIMS_LBUTTON_CLICK: if(m_bMouseLButtonClicked) return this; break;
    case GUIMS_LBUTTON_DOWN:  if(m_bMouseLButtonDown) return this; break;
    case GUIMS_LBUTTON_UP:    if(!m_bMouseLButtonDown) return this; break;

    case GUIMS_RBUTTON_CLICK: if(m_bMouseRButtonClicked) return this; break;
    case GUIMS_RBUTTON_DOWN:  if(m_bMouseRButtonDown) return this; break;
    case GUIMS_RBUTTON_UP:    if(!m_bMouseRButtonDown) return this; break;

    case GUIMS_MBUTTON_CLICK: if(m_bMouseMButtonClicked) return this; break;
    case GUIMS_MBUTTON_DOWN:  if(m_bMouseMButtonDown) return this; break;
    case GUIMS_MBUTTON_UP:    if(!m_bMouseMButtonDown) return this; break;

    default: return NULL; break;
    }

    /** 从子控件中找 */
    xaeGUIObject* obj;
    for(size_t i = 0; i < m_pChildren.size(); i++)
    {
        obj = m_pChildren[i].m_pGUI->find_children_mouse_state(state);
        if(NULL != obj) return obj;
    }

    /** 找不到 */
    return NULL;
}

bool xaeGUIObject::is_children(xaeGUIObject* obj)
{
    for(size_t i = 0; i < m_pChildren.size(); i++)
    {
        if(m_pChildren[i].m_pGUI == obj) return true;
        if(m_pChildren[i].m_pGUI->is_children(obj)) return true;
    }

    return false;
}

void xaeGUIObject::set_pos( int x, int y )
{
    int w = m_tagRect.x2 - m_tagRect.x1, h = m_tagRect.y2 - m_tagRect.y1;
    m_tagRect.Set(x, y, x + w, y + h);
    on_set_pos(x, y);
}