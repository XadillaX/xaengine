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
    SLS_ENTERING,                               ///< ���ڽ���
    SLS_NO_LEAVE,                               ///< ���뿪
    SLS_LEAVING_WITH_DELETE,                    ///< �뿪�У������뿪��ɾ���Լ�
    SLS_LEAVING_WITHOUT_DELETE,                 ///< �뿪�У������뿪��ɾ���Լ�
    SLS_LEAVED_WITH_DELETE,                     ///< ���뿪������Ҫɾ���Լ�
    SLS_LEAVED_WITHOUT_DELETE                   ///< ���뿪�����Ҳ�ɾ���Լ�
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
    /** �ͷų��� */
    virtual void                release();
    /** �߼����� */
    virtual bool                update(float fDT);
    /** ��Ⱦ���� */
    virtual bool                render();
    /** ����ʱ�ĺ��� */
    virtual bool                on_enter_update(float fDT)  { set_leave_state(SLS_NO_LEAVE); return false; }
    virtual bool                on_enter_render()           { return false; }
    /** �뿪ʱ�ĺ��� */
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
    /** ��ȡ�������� */
    string                      get_scene_name()            { return m_szSceneName; }
    void                        set_scene_name(string name) { m_szSceneName = name; }

    /** ��Ӵ���Ⱦ�ľ��� */
    bool                        add_sprite_to_pool(string key, xaeSprite* spr, float x, float y, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);
    xaeSceneSpriteElement       create_pool_sprite(string key, xaeSprite* spr, float x, float y, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);
    
    bool                        add_sprite_to_pool_stretch(string key, xaeSprite* spr, float x, float y, float x2, float y2, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);
    xaeSceneSpriteElement       create_pool_sprite_stretch(string key, xaeSprite* spr, float x, float y, float x2, float y2, float zbuf = 1.0f, bool autodel = true, DWORD color = 0xffffffff, DWORD blendmode = BLEND_DEFAULT);

    /** �ͷ���Ⱦ����� */
    void                        release_sprite_pool();
    /** ��Ⱦ��Ⱦ����� */
    void                        render_sprite_pool();

    /** �����Ⱦ�������key��Ψһ�� */
    bool                        query_unque_sprite_name(string key);
    /** ��ȡ��Ⱦ������еľ��� */
    xaeSceneSpriteElement*      get_sprite_pool_element(string key);

protected:
    string                      m_szSceneName;
    XAE_SCENE_LEAVE_STATE       m_emLeaveState;
    vector<xaeSceneSpriteElement> m_lstSpritePool;
};

#endif
