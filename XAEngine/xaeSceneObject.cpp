/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Scene Object
*/
#include <xaeSceneObject.h>

xaeSceneObject::xaeSceneObject(string name /* = "default" */ ) :
    m_szSceneName(name),
    m_emLeaveState(SLS_NO_LEAVE)
{
    m_pHGE = hgeCreate(HGE_VERSION);
}

xaeSceneObject::~xaeSceneObject()
{
    m_pHGE->Release();
    release();
}

void xaeSceneObject::release()
{
    release_sprite_pool();
}

bool xaeSceneObject::update(float fDT)
{
    return false;
}

bool xaeSceneObject::render()
{
    return false;
}

XAE_SCENE_LEAVE_STATE xaeSceneObject::get_leave_state()
{
    return m_emLeaveState;
}

void xaeSceneObject::set_leave_state(XAE_SCENE_LEAVE_STATE state)
{
    m_emLeaveState = state;
}

void xaeSceneObject::leave(bool bAutoDel /* = true */)
{
    if(bAutoDel)
    {
        set_leave_state(SLS_LEAVING_WITH_DELETE);
    }
    else
    {
        set_leave_state(SLS_LEAVING_WITHOUT_DELETE);
    }
}

bool xaeSceneObject::add_sprite_to_pool(string key, xaeSprite* spr, float x, float y, float zbuf /* = 1.0f */, bool autodel /* = true */, DWORD color /* = 0xffffffff */, DWORD blendmode /* = BLEND_NOZWRITE */)
{
    /** 看看有没有重名 */
    if(!query_unque_sprite_name(key)) return false;

    /** 创建精灵渲染池元素 */
    xaeSceneSpriteElement tagSSE = create_pool_sprite(key, spr, x, y, zbuf, autodel, color, blendmode);

    /** 将其放到最后 */
    m_lstSpritePool.push_back(tagSSE);
    return true;
}

xaeSceneSpriteElement xaeSceneObject::create_pool_sprite(string key, xaeSprite* spr, float x, float y, float zbuf /* = 1.0f */, bool autodel /* = true */, DWORD color /* = 0xffffffff */, DWORD blendmode /* = BLEND_NOZWRITE */)
{
    xaeSceneSpriteElement tagSSE;
    tagSSE.autodel = autodel;
    tagSSE.blendmode = blendmode;
    tagSSE.color = color;
    tagSSE.pSpr = spr;
    tagSSE.type = XAE_SERT_NORMAL;
    tagSSE.x = x;
    tagSSE.y = y;
    tagSSE.zbuf = zbuf;

    return tagSSE;
}

bool xaeSceneObject::add_sprite_to_pool_stretch(string key, xaeSprite* spr, float x, float y, float x2, float y2, float zbuf /* = 1.0f */, bool autodel /* = true */, DWORD color /* = 0xffffffff */, DWORD blendmode /* = BLEND_NOZWRITE */)
{
    /** 看看有没有重名 */
    if(!query_unque_sprite_name(key)) return false;

    /** 创建精灵渲染池元素 */
    xaeSceneSpriteElement tagSSE = create_pool_sprite_stretch(key, spr, x, y, x2, y2, zbuf, autodel, color, blendmode);

    /** 将其放到最后 */
    m_lstSpritePool.push_back(tagSSE);
    return true;
}

xaeSceneSpriteElement xaeSceneObject::create_pool_sprite_stretch(string key, xaeSprite* spr, float x, float y, float x2, float y2, float zbuf /* = 1.0f */, bool autodel /* = true */, DWORD color /* = 0xffffffff */, DWORD blendmode /* = BLEND_NOZWRITE */)
{
    xaeSceneSpriteElement tagSSE;
    tagSSE.autodel = autodel;
    tagSSE.blendmode = blendmode;
    tagSSE.color = color;
    tagSSE.pSpr = spr;
    tagSSE.type = XAE_SERT_STRETCH;
    tagSSE.x = x;
    tagSSE.y = y;
    tagSSE.x2 = x2;
    tagSSE.y2 = y2;
    tagSSE.zbuf = zbuf;

    return tagSSE;
}

void xaeSceneObject::release_sprite_pool()
{
    map<xaeSprite*, bool> bDeled;
    for(size_t i = 0; i < m_lstSpritePool.size(); i++)
    {
        /** 如果要自动删除 */
        if(m_lstSpritePool[i].autodel && bDeled.find(m_lstSpritePool[i].pSpr) == bDeled.end())
        {
            m_lstSpritePool[i].pSpr->release();
            delete m_lstSpritePool[i].pSpr;
        }
    }

    bDeled.clear();
    m_lstSpritePool.clear();
}

void xaeSceneObject::render_sprite_pool()
{
    for(size_t i = 0; i < m_lstSpritePool.size(); i++)
    {
        xaeSceneSpriteElement tagSSE = m_lstSpritePool[i];
        tagSSE.pSpr->get_sprite()->SetBlendMode(tagSSE.blendmode);
        tagSSE.pSpr->get_sprite()->SetColor(tagSSE.color);
        tagSSE.pSpr->get_sprite()->SetZ(tagSSE.zbuf);

        switch(tagSSE.type)
        {
        case XAE_SERT_NORMAL: tagSSE.pSpr->render(tagSSE.x, tagSSE.y); break;
        case XAE_SERT_STRETCH: tagSSE.pSpr->render_stretch(tagSSE.x, tagSSE.y, tagSSE.x2, tagSSE.y2); break;
        case XAE_SERT_EX: tagSSE.pSpr->render_ex(tagSSE.x, tagSSE.y, tagSSE.rot, tagSSE.hscale, tagSSE.vscale); break;
        default: break;
        }
    }
}

bool xaeSceneObject::query_unque_sprite_name(string key)
{
    /** 遍历数组 */
    for(xae_scene_spr_ele_pool_iterator it = m_lstSpritePool.begin();
        it != m_lstSpritePool.end(); 
        it++
        )
    {
        if(key == (*it).name)
        {
            return false;
        }
    }

    return true;
}

xaeSceneSpriteElement* xaeSceneObject::get_sprite_pool_element(string key)
{
    /** 遍历数组 */
    for(xae_scene_spr_ele_pool_iterator it = m_lstSpritePool.begin();
        it != m_lstSpritePool.end(); 
        it++
        )
    {
        if(key == (*it).name)
        {
            return &(*it);
        }
    }

    return NULL;
}