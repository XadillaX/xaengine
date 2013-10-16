#include "xaeReviveScene.h"

xaeReviveScene::xaeReviveScene(void) :
    m_pGUI(NULL)
{
    __init();
}

xaeReviveScene::~xaeReviveScene(void)
{
    SAFEDEL(m_pGUI);
    release_sprite_pool();
}

void xaeReviveScene::__init()
{
    add_sprite_to_pool("Background", xaeSpriteLoader::Instance().get_sprite("Revive", "Background"), 0, 0);

    /** GUI */
    m_pGUI = new xaeGUIObject();
    m_pGUI->add_control(new xaeGUIButton(0, "Yes", "Revive*yes%d", 75, 365, "", "", "UI*Cursor_Pointer"));
    m_pGUI->add_control(new xaeGUIButton(1, "No", "Revive*no%d", 75 + 220, 365, "", "", "UI*Cursor_Pointer"));

    /** 储存原先BGM */
    m_szBGMFilename = g_BGMPlayer.getFilename();

    /** 播放BGM */
    g_BGMPlayer.setBGM("media/music/revive.ogg");
}

#include "xaeBlockScene.h"
bool xaeReviveScene::update(float fDT)
{
    float mx, my;
    m_pHGE->Input_GetMousePos(&mx, &my);
    m_pGUI->update(fDT, mx, my);

    g_BGMPlayer.update(fDT);

    /** 复活 */
    if(xae::Instance().keyup(HGEK_Y) || m_pGUI->get_control(0)->get_mouse_lbutton_clicked())
    {
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Block", "Block");
        xaeSceneMgr::Instance().add_scene("Block", scene);
        xaeSceneMgr::Instance().set_current_scene("Block");

        /** 载入关卡 */
        ((xaeBlockScene*)scene)->loadLevel(m_szLevel);

        /** 播放BGM */
        g_BGMPlayer.setBGM(m_szBGMFilename);

        return false;
    }

    /** 不复活 */
    if(xae::Instance().keyup(HGEK_N) || m_pGUI->get_control(1)->get_mouse_lbutton_clicked())
    {
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Welcome", "Welcome");
        xaeSceneMgr::Instance().add_scene("Welcome", scene);
        xaeSceneMgr::Instance().set_current_scene("Welcome");

        xaeSceneMgr::Instance().del_scene("AVG");

        return false;
    }

    return false;
}

bool xaeReviveScene::render()
{
    render_sprite_pool();

    m_pGUI->render(0, 0);
    m_pGUI->render_mouse();

    return false;
}