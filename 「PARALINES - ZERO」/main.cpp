/** 夏之扉·Zero 复刻版 */
#include "global.h"
#include "xaeWelcomeScene.h"
#include "resource.h"

void read_ini_file()
{
    HGE* pHGE = xae::Instance().get_core();
    pHGE->System_SetState(HGE_INIFILE, "para.ini");

    g_szDefaultFont = pHGE->Ini_GetString("system", "font", "黑体");
}

#ifdef __DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    /** 设置引擎 */
    xae::Instance().set_core(GAME_TITLE, GAME_WIDTH, GAME_HEIGHT, 100, 32, true, "para.log");
    xae::Instance().get_core()->System_SetState(HGE_DONTSUSPEND, false);
    xae::Instance().set_resource_mgr(new xaePackage());
    xae::Instance().get_core()->System_SetState(HGE_ICON, MAKEINTRESOURCE(IDI_ICON1));
    xaeSceneMgr::Instance().set_default_mouse(IDC_ARROW, true);

    read_ini_file();

    /** 初始化场景对象工厂 */
    init_scene_factory();

    if(xae::Instance().init_core())
    {
        g_hWnd = xae::Instance().get_core()->System_GetState(HGE_HWND);
        xae::Instance().create_temp_folder(GAME_STATE_PATH);
        read_save();

        /** 设置包裹管理器 */
        xae::Instance().set_resource_mgr(new xaePackage());

        /** 初始化场景 */
        xaeSceneMgr::Instance().add_scene(
            "Welcome",
            xaeSceneMgr::Instance().create_scene("Welcome", "Welcome"));
        xaeSceneMgr::Instance().set_default_mouse("media/cursor_arrow.png");
        xaeSceneMgr::Instance().set_current_scene("Welcome");

        xae::Instance().start_core();
        xae::Instance().shutdown_core();
        xae::Instance().release_core();
    }
    else xae::Instance().alert("内核初始化错误。", "错误", MB_ICONERROR);

    return 0;
}
