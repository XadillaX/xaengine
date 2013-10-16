/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Scene Manager
*/
#include <xaeSceneMgr.h>
xaeSceneFactory* g_pSceneFactory = NULL;

xaeSceneMgr::xaeSceneMgr() :
    m_pCurrentScene(NULL),
    m_pPreviousScene(NULL),
    m_pNextScene(NULL),
    m_bWinMouse(true),
    m_szMouse(""),
    m_lpszMouse(IDC_ARROW)
{
    m_pHGE = hgeCreate(HGE_VERSION);
    g_pSceneFactory = &(xaeSceneFactory::Instance());
}

xaeSceneMgr::~xaeSceneMgr() {
    m_pHGE->Release();
}

xaeSceneObject* xaeSceneMgr::get_scene(string name)
{
    scene_obj_map::iterator it;
    for(it = m_pSceneArray.begin(); it != m_pSceneArray.end(); it++)
    {
        if(name == it->first) return it->second;
    }
    return NULL;
}

scene_obj_map::iterator xaeSceneMgr::get_scene_iterator(string name)
{
    scene_obj_map::iterator it;
    for(it = m_pSceneArray.begin(); it != m_pSceneArray.end(); it++)
    {
        if(name == it->first) return it;
    }
    return m_pSceneArray.end();
}

bool xaeSceneMgr::add_scene(std::string name, xaeSceneObject *obj)
{
    /** 场景映射中是否已存在此名字 */
    if(NULL != get_scene(name))
    {
        m_pHGE->System_Log("[x] Can't add scene [%s] to ScnMgr. This scene already exists.", name.c_str());
        return false;
    }

    /** 当前场景加入映射 */
    m_pSceneArray[name] = obj;
    obj->set_scene_name(name);
    m_pHGE->System_Log("[o] Successfully add scene [%s] to ScnMgr.", name.c_str());
    return true;
}

bool xaeSceneMgr::del_scene(string name)
{
    /** 寻找对应场景 */
    xaeSceneObject* obj;
    if(NULL != (obj = get_scene(name)))
    {
        /** 场景正在使用中 */
        if(m_pCurrentScene == obj)
        {
            m_pHGE->System_Log("[x] Can't delete scene [%s]. Because it's on using now.", name.c_str());
            return false;
        }
        else
        {
            /** 从映射中移除 */
            scene_obj_map::iterator it = get_scene_iterator(name);
            m_pSceneArray.erase(it);

            /** 释放资源 */
            obj->release();

            /** 删除指针 */
            delete obj;

            m_pHGE->System_Log("[o] Successfully delete scene [%s].", name.c_str());
            return true;
        }
    }

    /** 没有找到场景 */
    m_pHGE->System_Log("[x] Can't delete scene [%s]. Because it's not existing.", name.c_str());
    return false;
}

xaeSceneObject* xaeSceneMgr::get_current_scene()
{
    if(NULL == m_pCurrentScene)
    {
        m_pHGE->System_Log("[!] Current scene is NULL.");
    }
    return m_pCurrentScene;
}

xaeSceneObject* xaeSceneMgr::get_prev_scene()
{
    if(NULL == m_pPreviousScene)
    {
        m_pHGE->System_Log("[!] Previous scene is NULL.");
    }
    return m_pCurrentScene;
}

xaeSceneObject* xaeSceneMgr::get_next_scene()
{
    if(NULL == m_pNextScene)
    {
        m_pHGE->System_Log("[!] Next scene is NULL.");
    }
    return m_pNextScene;
}

bool xaeSceneMgr::set_current_scene(string name, bool bAutoDelPrev)
{
    /** 获取场景 */
    xaeSceneObject* obj = get_scene(name);
    if(NULL == obj)
    {
        m_pHGE->System_Log("[x] Can't set [%s] to current scene. Because it's not existing.", name.c_str());
        return false;
    }

    /** 若原先无场景，则直接进入场景 */
    if(NULL == m_pCurrentScene)
    {
        m_pCurrentScene = obj;
        m_pCurrentScene->set_leave_state(SLS_ENTERING);
    }
    else
    {
        m_pNextScene = obj;
        m_pNextScene->set_leave_state(SLS_ENTERING);
        m_pCurrentScene->leave(bAutoDelPrev);
    }

    return true;
}

void xaeSceneMgr::release()
{
    /** 当前场景设置为NULL */
    m_pCurrentScene = NULL;

    /** 逐一释放并且删除 */
    scene_obj_map::iterator it;
    for(it = m_pSceneArray.begin(); it != m_pSceneArray.end(); it++)
    {
        it->second->release();
        delete (it->second);
    }

    /** 清空映射 */
    m_pSceneArray.clear();
}

void xaeSceneMgr::turn_scene_by_order()
{
    m_pPreviousScene = m_pCurrentScene;
    m_pCurrentScene = m_pNextScene;
    m_pNextScene = NULL;

    if(NULL != m_pPreviousScene &&
        (
        m_pPreviousScene->get_leave_state() == SLS_LEAVED_WITH_DELETE ||
        m_pPreviousScene->get_leave_state() == SLS_LEAVING_WITH_DELETE
        )
      )
    {
        del_scene(m_pPreviousScene->get_scene_name());
    }
}

#include <gui/xaeGUIObject.h>
xaeSceneObject* xaeSceneMgr::create_scene(string tn, string scenename)
{
    /** 设置鼠标为WAIT状态 */
    xaeGUIObject::set_mouse(IDC_WAIT);
    xaeGUIObject::render_mouse();

    /** 新建场景 */
    xaeSceneObject* obj = g_pSceneFactory->Create(tn);
    if(obj != NULL) obj->set_scene_name(scenename);
    
    /** 还原鼠标 */
    if(get_default_mouse_type())
    {
        xaeGUIObject::set_mouse(get_default_mouse_win());
    }
    else xaeGUIObject::set_mouse(get_default_mouse_path());
    xaeGUIObject::render_mouse();

    return obj;
}

bool xaeSceneMgr::register_scene(char* tn, xaeSceneFactory::tCreator fn)
{
    return g_pSceneFactory->Register(tn, fn);
}

void xaeSceneMgr::set_default_mouse(char* szMouse, bool bWinDefault /* = false */)
{
    m_bWinMouse = bWinDefault;
    if(m_bWinMouse)
    {
        m_lpszMouse = szMouse;
        xaeGUIObject::set_mouse((LPSTR)szMouse);
    }
    else
    {
        m_szMouse = szMouse;
        xaeGUIObject::set_mouse(string(szMouse));
    }
}

void xaeSceneMgr::set_default_mouse(string szMouse)
{
    m_szMouse = szMouse;
    xaeGUIObject::set_mouse(szMouse);
}

LPSTR xaeSceneMgr::get_default_mouse_win()
{
    return m_lpszMouse;
}

string xaeSceneMgr::get_default_mouse_path()
{
    return m_szMouse;
}

bool xaeSceneMgr::get_default_mouse_type()
{
    return m_bWinMouse;
}