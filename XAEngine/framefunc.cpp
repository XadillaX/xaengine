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

/** �޸���ʱ���֡���� */
bool __framefunc()
{
    static HGE* hge = xae::Instance().get_core();
    /** ��ȡ���ʱ�� */
    static float fDT;
    /** �޸�fDT BUG��������һ��ֱ�ӽ���static֮����ô����ֻ��ȡ��һ��fDT 2012/5/1 -XadillaX */
    fDT = xae::Instance().get_core()->Timer_GetDelta();

    return __framefunc(fDT);
}

/** �и���ʱ���֡���� */
bool __framefunc(float fDT)
{
    static HGE* hge = xae::Instance().get_core();

    /** ��ǰ�������� */
    static xaeSceneObject* scene;
    scene = xaeSceneMgr::Instance().get_current_scene();

    /** û�е�ǰ���� */
    if(NULL == scene)
    {
        hge->System_Log("[!] No current scene.");
        return false;
    }

    /** ����ǰ�������ڽ��� */
    bool result = true;
    if(SLS_ENTERING == scene->get_leave_state())
    {
        result = scene->on_enter_update(fDT);
    }
    /** �������Ѿ������� */
    if(SLS_NO_LEAVE == scene->get_leave_state())
    {
        result = scene->update(fDT);
    }
    /** �����������뿪 */
    if(SLS_LEAVING_WITH_DELETE == scene->get_leave_state() ||
        SLS_LEAVING_WITHOUT_DELETE == scene->get_leave_state()
        )
    {
        result = scene->on_leave_update(fDT);
    }
    /** ���������뿪 */
    if(SLS_LEAVED_WITH_DELETE == scene->get_leave_state() ||
        SLS_LEAVED_WITHOUT_DELETE == scene->get_leave_state()
        )
    {
        xaeSceneMgr::Instance().turn_scene_by_order();

        /** ����һ�����г��� */
        result = __framefunc(fDT);
    }

    return result;
}

bool __renderfunc()
{
    if(xae::Instance().is_no_more_render()) return false;
    static HGE* hge = xae::Instance().get_core();

    /** ��ǰ�������� */
    static xaeSceneObject* scene;
    scene = xaeSceneMgr::Instance().get_current_scene();

    /** û�е�ǰ���� */
    if(NULL == scene)
    {
        hge->System_Log("[!] No current scene.");
        return false;
    }

    /** ��ʼ��Ⱦ */
    hge->Gfx_BeginScene();
    hge->Gfx_Clear(0xff000000);

    /** ��Ⱦ���� */
    bool res = false;
    XAE_SCENE_LEAVE_STATE state = scene->get_leave_state();
    switch(state)
    {
    case SLS_ENTERING: res = scene->on_enter_render(); break;
    case SLS_NO_LEAVE: res = scene->render(); break;
    default: res = scene->on_leave_render(); break;
    }

    /** ������Ⱦ */
    hge->Gfx_EndScene();

    return res;
}
