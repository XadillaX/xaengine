/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Scene Manager
*/
#pragma once
#ifndef XAESCENEMGR_H
#define XAESCENEMGR_H
#include <map>
#include <string>
using namespace std;

#include "utils.h"
#include "ixaeobject.h"
#include "xaeSceneObject.h"

typedef map<string, xaeSceneObject*> scene_obj_map;
typedef ObjectFactory<xaeSceneObject> xaeSceneFactory;
extern xaeSceneFactory* g_pSceneFactory;

class XAEEXP xaeSceneMgr : public iXAEObject, public Singleton<xaeSceneMgr>
{
public:
    xaeSceneMgr();
    virtual ~xaeSceneMgr();

public:
    /** 获取场景 */
    xaeSceneObject*             get_scene(string name);
    scene_obj_map::iterator     get_scene_iterator(string name);
    /** 新增场景 */
    bool                        add_scene(string name, xaeSceneObject* obj);
    /** 删除场景 */
    bool                        del_scene(string name);
    /** 获取当前场景 */
    xaeSceneObject*             get_current_scene();
    xaeSceneObject*             get_prev_scene();
    xaeSceneObject*             get_next_scene();
    /** 设置当前场景 */
    bool                        set_current_scene(string name, bool bAutoDelPrev = true);
    /** 根据原先订好的顺序改变场景 */
    void                        turn_scene_by_order();
    /** 释放场景 */
    void                        release();

    /** 新建场景 */
    xaeSceneObject*             create_scene(string tn, string scenename = "default");
    /** 注册场景类型 */
    bool                        register_scene(char* tn, xaeSceneFactory::tCreator fn);
    /** 设置默认鼠标 */
    void                        set_default_mouse(char* szMouse, bool bWinDefault = false);
    void                        set_default_mouse(string szMouse);
    /** 获取设置的Windows默认鼠标 */
    LPSTR                       get_default_mouse_win();
    /** 获取设置的精灵型默认鼠标 */
    string                      get_default_mouse_path();
    /** 获取默认鼠标类型: true为Windows鼠标，否则为精灵型鼠标 */
    bool                        get_default_mouse_type();

private:
    scene_obj_map               m_pSceneArray;
    xaeSceneObject*             m_pCurrentScene;                ///< 当前场景
    xaeSceneObject*             m_pPreviousScene;               ///< 上一个场景
    xaeSceneObject*             m_pNextScene;                   ///< 下一个场景

    string                      m_szMouse;
    LPSTR                       m_lpszMouse;
    bool                        m_bWinMouse;
};

#endif
