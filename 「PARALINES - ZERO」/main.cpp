/** ��֮�顤Zero ���̰� */
#include "global.h"
#include "xaeWelcomeScene.h"
#include "resource.h"

void read_ini_file()
{
    HGE* pHGE = xae::Instance().get_core();
    pHGE->System_SetState(HGE_INIFILE, "para.ini");

    g_szDefaultFont = pHGE->Ini_GetString("system", "font", "����");
}

#ifdef __DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    /** �������� */
    xae::Instance().set_core(GAME_TITLE, GAME_WIDTH, GAME_HEIGHT, 100, 32, true, "para.log");
    xae::Instance().get_core()->System_SetState(HGE_DONTSUSPEND, false);
    xae::Instance().set_resource_mgr(new xaePackage());
    xae::Instance().get_core()->System_SetState(HGE_ICON, MAKEINTRESOURCE(IDI_ICON1));
    xaeSceneMgr::Instance().set_default_mouse(IDC_ARROW, true);

    read_ini_file();

    /** ��ʼ���������󹤳� */
    init_scene_factory();

    if(xae::Instance().init_core())
    {
        g_hWnd = xae::Instance().get_core()->System_GetState(HGE_HWND);
        xae::Instance().create_temp_folder(GAME_STATE_PATH);
        read_save();

        /** ���ð��������� */
        xae::Instance().set_resource_mgr(new xaePackage());

        /** ��ʼ������ */
        xaeSceneMgr::Instance().add_scene(
            "Welcome",
            xaeSceneMgr::Instance().create_scene("Welcome", "Welcome"));
        xaeSceneMgr::Instance().set_default_mouse("media/cursor_arrow.png");
        xaeSceneMgr::Instance().set_current_scene("Welcome");

        xae::Instance().start_core();
        xae::Instance().shutdown_core();
        xae::Instance().release_core();
    }
    else xae::Instance().alert("�ں˳�ʼ������", "����", MB_ICONERROR);

    return 0;
}
