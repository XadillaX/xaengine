#include "xaeLoginScene.h"

#define LOGIN_STAGE_GUI_START 10
const int stage_gui_x[] = { 220, 310, 400, 490, 140, 225, 310, 395, 480, 565, 175, 420, 330 };
const int stage_gui_y[] = { 95, 95, 95, 95, 250, 250, 250, 250, 250, 250, 310, 310, 455 };

xaeLoginScene::xaeLoginScene(void) :
    m_pGUI(NULL)
{
    _init();
}

xaeLoginScene::~xaeLoginScene(void)
{
    release_sprite_pool();

    SAFEDEL(m_pGUI);
}

void xaeLoginScene::_init()
{
    /** 设置Loader */
    xaeSpriteLoader::Instance().set_xml("media/loader/login.xml");

    /** GUI */
    m_pGUI = new xaeGUIObject();
    m_pGUI->add_control(new xaeGUIButton(0, "Back", "Button*Back%d", (GAME_WIDTH - 103) >> 1, 520, "", "", "Cursor*Pointer"));

    /** 衬底 */
    add_sprite_to_pool(
        "Underpainting",
        xaeSpriteLoader::Instance().get_sprite("BG", "Underpainting"),
        0,
        0
        );
    /** 标题 */
    add_sprite_to_pool(
        "Title1",
        xaeSpriteLoader::Instance().get_sprite("Title", "1"),
        (GAME_WIDTH - 100) >> 1,
        30
        );
    if(g_tagSave.stage > 3)
    {
        add_sprite_to_pool(
            "Title2",
            xaeSpriteLoader::Instance().get_sprite("Title", "2"),
            (GAME_WIDTH - 100) >> 1,
            190
            );
    }
    if(g_tagSave.stage > 11)
    {
        add_sprite_to_pool(
            "Title3",
            xaeSpriteLoader::Instance().get_sprite("Title", "3"),
            (GAME_WIDTH - 100) >> 1,
            405
            );
    }

    /** 分割线 */
    if(g_tagSave.stage > 3)
    {
        add_sprite_to_pool(
            "Line1",
            xaeSpriteLoader::Instance().get_sprite("Title", "Line"),
            (GAME_WIDTH - 575) >> 1,
            145
            );
    }
    if(g_tagSave.stage > 11)
    {
        add_sprite_to_pool(
            "Line2",
            xaeSpriteLoader::Instance().get_sprite("Title", "Line"),
            (GAME_WIDTH - 575) >> 1,
            350
            );
    }
    add_sprite_to_pool(
        "Line3",
        xaeSpriteLoader::Instance().get_sprite("Title", "Line"),
        (GAME_WIDTH - 575) >> 1,
        500
        );

    _create_stage_gui();
}

#include "xaePlayScene.h"
bool xaeLoginScene::update(float fDT)
{
    /** 更新GUI */
    float mousex, mousey;
    m_pHGE->Input_GetMousePos(&mousex, &mousey);
    m_pGUI->update(fDT, mousex, mousey);

    /** 相应GUI */
    if(m_pGUI->get_control("Back")->get_mouse_lbutton_clicked())
    {
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Welcome", "Welcome");
        if(NULL != scene) xaeSceneMgr::Instance().add_scene("Welcome", scene);
        xaeSceneMgr::Instance().set_current_scene("Welcome");

        return false;
    }

    /** 是否选择了Stage */
    for(int i = 0; i < g_tagSave.stage + 1; i++)
    {
        xaeGUIButton* pBtn = (xaeGUIButton*)m_pGUI->get_control(LOGIN_STAGE_GUI_START + i);
        if(pBtn->get_mouse_lbutton_clicked())
        {
            xaePlayScene* scene = (xaePlayScene*)(xaeSceneMgr::Instance().create_scene("Play", pBtn->get_control_key()));
            if(NULL != scene)
            {
                xaeSceneMgr::Instance().add_scene(pBtn->get_control_key(), scene);
                scene->load_stage(i);
            }
            xaeSceneMgr::Instance().set_current_scene(pBtn->get_control_key());

            return false;
        }
    }

    return false;
}

bool xaeLoginScene::render()
{
    render_sprite_pool();

    /** 绘制GUI */
    m_pGUI->render(0, 0);

    /** 绘制鼠标 */
    xaeGUIObject::render_mouse();

    return false;
}

void xaeLoginScene::_create_stage_gui()
{
    for(int i = 0; i < g_tagSave.stage + 1; i++)
    {
        char key[10];
        sprintf(key, "stage%d", i + 1);
        xaeGUIButton* pBtn = new xaeGUIButton(
            LOGIN_STAGE_GUI_START + i,
            key,
            "StageSelect*" + string(key) + "_%d",
            stage_gui_x[i],
            stage_gui_y[i],
            "",
            "",
            "Cursor*Pointer"
            );

        m_pGUI->add_control(pBtn);
    }
}
