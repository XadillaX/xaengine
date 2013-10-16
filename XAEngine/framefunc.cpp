/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Frame Function & Render Function & so on...
*/
#include <xae.h>
#include <xaeSceneMgr.h>

bool __framefunc(float fDT);

/** 无更新时间的帧函数 */
bool __framefunc()
{
    static HGE* hge = xae::Instance().get_core();
    /** 获取间隔时间 */
    static float fDT;
    /** 修复fDT BUG，本来这一句直接接在static之后，那么就是只获取第一次fDT 2012/5/1 -XadillaX */
    fDT = xae::Instance().get_core()->Timer_GetDelta();

    return __framefunc(fDT);
}

/** 有更新时间的帧函数 */
bool __framefunc(float fDT)
{
    static HGE* hge = xae::Instance().get_core();

    /** 当前场景更新 */
    static xaeSceneObject* scene;
    scene = xaeSceneMgr::Instance().get_current_scene();

    /** 没有当前场景 */
    if(NULL == scene)
    {
        hge->System_Log("[!] No current scene.");
        return false;
    }

    /** 若当前场景正在进入 */
    bool result = true;
    if(SLS_ENTERING == scene->get_leave_state())
    {
        result = scene->on_enter_update(fDT);
    }
    /** 若场景已经进入了 */
    if(SLS_NO_LEAVE == scene->get_leave_state())
    {
        result = scene->update(fDT);
    }
    /** 若场景正在离开 */
    if(SLS_LEAVING_WITH_DELETE == scene->get_leave_state() ||
        SLS_LEAVING_WITHOUT_DELETE == scene->get_leave_state()
        )
    {
        result = scene->on_leave_update(fDT);
    }
    /** 若场景已离开 */
    if(SLS_LEAVED_WITH_DELETE == scene->get_leave_state() ||
        SLS_LEAVED_WITHOUT_DELETE == scene->get_leave_state()
        )
    {
        xaeSceneMgr::Instance().turn_scene_by_order();

        /** 更新一遍现有场景 */
        result = __framefunc(fDT);
    }

    return result;
}

bool __renderfunc()
{
    if(xae::Instance().is_no_more_render()) return false;
    static HGE* hge = xae::Instance().get_core();

    /** 当前场景更新 */
    static xaeSceneObject* scene;
    scene = xaeSceneMgr::Instance().get_current_scene();

    /** 没有当前场景 */
    if(NULL == scene)
    {
        hge->System_Log("[!] No current scene.");
        return false;
    }

    /** 开始渲染 */
    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0xff000000);

    /** 渲染场景 */
    bool res = false;
    XAE_SCENE_LEAVE_STATE state = scene->get_leave_state();
    switch(state)
    {
    case SLS_ENTERING: res = scene->on_enter_render(); break;
    case SLS_NO_LEAVE: res = scene->render(); break;
    default: res = scene->on_leave_render(); break;
    }

    /** 结束渲染 */
    hge->Gfx_EndScene();

    return res;
}
