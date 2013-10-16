/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Scene Object
*/
#pragma once
#ifndef XAESCENEOBJECT_H
#define XAESCENEOBJECT_H
#include "utils.h"
#include "ixaeobject.h"
#include <string>
#include <vector>
#include "xaeSprite.h"
using namespace std;

enum XAE_SCENE_LEAVE_STATE
{
    SLS_ENTERING,                               ///< 正在进入
    SLS_NO_LEAVE,                               ///< 不离开
    SLS_LEAVING_WITH_DELETE,                    ///< 离开中，并且离开后删除自己
    SLS_LEAVING_WITHOUT_DELETE,                 ///< 离开中，并且离开后不删除自己
    SLS_LEAVED_WITH_DELETE,                     ///< 已离开，并且要删除自己
    SLS_LEAVED_WITHOUT_DELETE                   ///< 已离开，并且不删除自己
};

enum XAE_SCENE_SPRITE_ELEMENT_RENDER_TYPE
{
    XAE_SERT_NORMAL,
    XAE_SERT_STRETCH,
    XAE_SERT_EX
};

struct xaeSceneSpriteElement
{
    bool                        autodel;
    DWORD                       color;
    DWORD                       blendmode;
    float                       zbuf;

    string                      name;

    xaeSprite*                  pSpr;
    float                       x;
    float                       y;
    XAE_SCENE_SPRITE_ELEMENT_RENDER_TYPE type;
    float                       x2;
    float                       y2;
    float                       rot;
    float                       hscale;
    float                       vscale;
};

typedef vector<xaeSceneSpriteElement>::iterator xae_scene_spr_ele_pool_iterator;

class XAEEXP xaeSceneObject : public iXAEObject
{
public:
    xaeSceneObject(string name = "default");
    virtual ~xaeSceneObject();

public:
    /** 释放场景 */
    virtual void                release();
    /** 逻辑函数 */
    virtual bool                update(float fDT);
    /** 渲染函数 */
    virtual bool                render();
    /** 进入时的函数 */
    virtual bool                on_enter_update(float fDT)  { set_leave_state(SLS_NO_LEAVE); return false; }
    virtual bool                on_enter_render()           { return false; }
    /** 离开时的函数 */
    virtual bool                on_leave_update(float fDT)
    {
        if(m_emLeaveState == SLS_LEAVING_WITH_DELETE) set_leave_state(SLS_LEAVED_WITH_DELETE);
        else
        if(m_emLeaveState == SLS_LEAVING_WITHOUT_DELETE) set_leave_state(SLS_LEAVED_WITHOUT_DELETE);
        return false;
    }
    virtual bool                on_leave_render()           { return false; }

public:
    XAE_SCENE_LEAVE_STATE       get_leave_state();
    void                        set_leave_state(XAE_SCENE_LEAVE_STATE state);

    void                        leave(bool bAutoDel = true);
    /** 获取场景名字 */
    string                      get_scene_name()            { return m_szSceneName; }
    void                        set_scene_name(string name) { m_szSceneName = name; }

    /** 添加待渲染的精灵 */
    bool                        add_sprite_to_pool(string key, xaeSprite* spr, float x, float y, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);
    xaeSceneSpriteElement       create_pool_sprite(string key, xaeSprite* spr, float x, float y, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);
    
    bool                        add_sprite_to_pool_stretch(string key, xaeSprite* spr, float x, float y, float x2, float y2, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);
    xaeSceneSpriteElement       create_pool_sprite_stretch(string key, xaeSprite* spr, float x, float y, float x2, float y2, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);

    /** 释放渲染精灵池 */
    void                        release_sprite_pool();
    /** 渲染渲染精灵池 */
    void                        render_sprite_pool();

    /** 检查渲染精灵池中key的唯一性 */
    bool                        query_unque_sprite_name(string key);
    /** 获取渲染精灵池中的精灵 */
    xaeSceneSpriteElement*      get_sprite_pool_element(string key);

protected:
    string                      m_szSceneName;
    XAE_SCENE_LEAVE_STATE       m_emLeaveState;
    vector<xaeSceneSpriteElement> m_lstSpritePool;
};

#endif
