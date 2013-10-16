#include "xaeWelcomeScene.h"
#include "xaeAVGScene.h"
#include "xaeLoadingScene.h"

DWORD WINAPI createWelcomeThread(LPVOID lpParam)
{
    loadingThreadPara* para = (loadingThreadPara*)lpParam;
    para->m_szSceneName = para->m_szSceneType = "Welcome";

    xaeWelcomeScene* scene;
    scene = (xaeWelcomeScene*)(xaeSceneMgr::Instance().create_scene(para->m_szSceneName, para->m_szSceneType));

    LockScope oScope(para->m_CriticalSection);
    para->m_pScene = scene;
    para->m_bCreated = true;

    return 0;
}

xaeWelcomeScene::xaeWelcomeScene(void) :
    m_pGUI(NULL)
{
    __init();
}

xaeWelcomeScene::~xaeWelcomeScene(void)
{
    release_sprite_pool();

    if(NULL != m_pGUI)
    {
        delete m_pGUI;
    }
}

void xaeWelcomeScene::__init()
{
    /** 背景 */
    add_sprite_to_pool(
        "BG",
        xaeSpriteLoader::Instance().get_sprite("Welcome", "BG"),
        0,
        0
        );

    /** UI */
    m_pGUI = new xaeGUIObject();
    m_pGUI->add_control(new xaeGUIButton(0, "Start", "Welcome*Start%d", 50, 220, "", "", "UI*Cursor_Pointer"));
    m_pGUI->add_control(new xaeGUIButton(1, "Quit", "Welcome*Quit%d", 50, 300, "", "", "UI*Cursor_Pointer"));

    /** 播放BGM */
    g_BGMPlayer.setBGM("media/music/cover.mp3");
}

bool xaeWelcomeScene::update(float fDT)
{
    /** 更新GUI */
    float mousex, mousey;
    m_pHGE->Input_GetMousePos(&mousex, &mousey);
    m_pGUI->update(fDT, mousex, mousey);

    if(m_pGUI->get_control("Start")->get_mouse_lbutton_clicked())
    {
        //xaeAVGScene* scene = (xaeAVGScene*)(xaeSceneMgr::Instance().create_scene("AVG", "AVG"));
        //if(NULL != scene)
        //{
        //    xaeSceneMgr::Instance().add_scene("AVG", scene);
        //}
        //xaeSceneMgr::Instance().set_current_scene("AVG");

        /** 载入AVG场景并显示 */
        //xaeLoadingScene* loadingScene = (xaeLoadingScene*)(xaeSceneMgr::Instance().get_scene("Loading"));
        //loadingScene->loadingSceneAndShow(createAVGThread);
        //xaeSceneMgr::Instance().set_current_scene("Loading");

        /** Loading界面 */
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Loading", "Loading");
        xaeSceneMgr::Instance().add_scene("Loading", scene);
        xaeSceneMgr::Instance().set_current_scene("Loading");

        return false;
    }
    else
    if(m_pGUI->get_control("Quit")->get_mouse_lbutton_clicked())
    {
        return true;
    }

    g_BGMPlayer.update(fDT);

    return false;
}

bool xaeWelcomeScene::render()
{
    render_sprite_pool();

    m_pGUI->render(0, 0);

    xaeGUIObject::render_mouse();

    return false;
}
