#include "global.h"
#include "resource.h"
#include "xaeLoadingScene.h"

void __initIniFile()
{
    xae::Instance().get_core()->System_SetState(HGE_INIFILE, "config.ini");

    g_szDefaultFont = xae::Instance().get_core()->Ini_GetString("system", "font", "΢���ź�");
    g_nReboundAlgorithm = xae::Instance().get_core()->Ini_GetInt("system", "rebound", 1);
    g_nVolume = xae::Instance().get_core()->Ini_GetInt("system", "volume", 100);
}

#ifdef __DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR szCmd, int)
#endif
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    /** �������� */
    xae::Instance().set_core("- �������� -", GAME_WIDTH, GAME_HEIGHT, 100, 32, true, "SecretGarden.log");
    //xae::Instance().get_core()->Resource_AttachPack("System.sgpak", "XadillaX");
    xae::Instance().get_core()->System_SetState(HGE_DONTSUSPEND, true);
    xae::Instance().get_core()->System_SetState(HGE_ICON, MAKEINTRESOURCE(IDI_ICON1));
    xae::Instance().set_resource_mgr(new xaePackage());

    /** ��ȡIni�ļ� */
    __initIniFile();

    /** ��ʼ���������� */
    init_scene_factory();

    /** ��ȡ��Ϸ���� */
    if(!getGameSetting())
    {
        xae::Instance().alert("Error in reading config file.", "Error", MB_ICONERROR);
        return 0;
    }
    /** ����������Ϸ���� */
    xae::Instance().get_core()->System_SetState(HGE_TITLE, g_Setting.System.m_szGameTitle.c_str());

    if(xae::Instance().init_core())
    {
        g_hWnd = xae::Instance().get_core()->System_GetState(HGE_HWND);

        /** ���ð��������� */
        xae::Instance().set_resource_mgr(new xaePackage());
        xaeSpriteLoader::Instance().set_xml("media/loader/common_resource.xml");

        /** ���ó��� */
        xaeSceneMgr::Instance().add_scene("Logo", xaeSceneMgr::Instance().create_scene("Logo", "Logo"));
        xaeSceneMgr::Instance().set_default_mouse("media/images/ui/cursor_arrow.png");
        xaeSceneMgr::Instance().set_current_scene("Logo");

        /** ��ʼ */
        xae::Instance().start_core();

        /** ���� */
        xaeSceneMgr::Instance().release();
        g_BGMPlayer.release();
        xae::Instance().shutdown_core();
        xae::Instance().release_core();
    }
    else xae::Instance().alert("Core Initialing error.", "Error", MB_ICONERROR);

    return 0;
}
