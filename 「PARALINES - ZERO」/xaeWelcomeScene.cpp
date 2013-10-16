#include "xaeWelcomeScene.h"
#include <gui/xaeGUITextButton.h>
#include "xaeGUIAlphaButton.h"

#define CG_PAGE_GUI_START_ID 100

xaeWelcomeScene::xaeWelcomeScene(void) :
    m_pSndBGM(NULL),
    m_pGUI(NULL),
    m_bFirst(true),
    m_pFire(NULL),
    m_pCGBoard(NULL),
    m_emState(SCS_NO_CG),
    m_nCGPage(0)
{
    __init();
}

xaeWelcomeScene::~xaeWelcomeScene(void)
{
    release_sprite_pool();

    if(NULL != m_pSndBGM)
    {
        m_pSndBGM->release();
        delete m_pSndBGM;
    }
    
    if(NULL != m_pGUI)
    {
        delete m_pGUI;
    }

    if(NULL != m_pFire)
    {
        m_pFire->release();
        delete m_pFire;
    }

    SAFEDEL(m_pCGBoard);
    SAFEDEL(m_pShowCG);
}

void xaeWelcomeScene::__init()
{
    xaeSpriteLoader::Instance().set_xml("media/loader/welcome.xml");
    xaeSoundLoader::Instance().set_xml("media/loader/welcome.xml");

    /** 衬底 */
    add_sprite_to_pool(
        "Underpainting",
        xaeSpriteLoader::Instance().get_sprite("BG", "Underpainting"),
        0,
        0
        );
    /** 背景图片 */
    add_sprite_to_pool(
        "Background",
        xaeSpriteLoader::Instance().get_sprite("BG", "Background"),
        0,
        0
        );
    /** Logo */
    add_sprite_to_pool(
        "Logo",
        xaeSpriteLoader::Instance().get_sprite("BG", "Logo"),
        5,
        15
        );

    /** GUI */
    m_pGUI = new xaeGUIObject();
    m_pGUI->add_control(new xaeGUIButton(0, "Start", "GUI*Start%d", 50, 320, "", "", "Cursor*Pointer"));
    m_pGUI->add_control(new xaeGUIButton(1, "CG", "GUI*CG%d", 50, 320 + 45, "", "", "Cursor*Pointer"));
    m_pGUI->add_control(new xaeGUIButton(2, "Settings", "GUI*Settings%d", 50, 320 + 45 * 2, "", "", "Cursor*Pointer"));
    m_pGUI->add_control(new xaeGUIButton(3, "Exit", "GUI*Exit%d", 50, 320 + 45 * 3, "", "", "Cursor*Pointer"));

    /** 背景音乐 */
    m_pSndBGM = xaeSoundLoader::Instance().get_sound("BGM", "BGM");

    /** 火焰 */
    m_pFire = new xaeParticles("media/particles/fire.cfg");
    m_pFire->fire_at(490, 250);

    /** CG GUI */
    xaeGUIStaticImage* pCGBoard;
    m_pCGBoard = new xaeGUIStaticImage(0, "CGBoard", "CGBoard*CGAlpha", 0, 0);
    m_pCGBoard->add_control((xaeGUIObject*)new xaeGUIStaticImage(0, "CGBoard", "CGBoard*Board", (GAME_WIDTH - 548) >> 1, (GAME_HEIGHT - 383) >> 1));
    pCGBoard = (xaeGUIStaticImage*)m_pCGBoard->get_control("CGBoard");
    pCGBoard->add_control(new xaeGUIStaticImage(0, "title", "CGBoard*title", (549 - 61) >> 1, 10));
    pCGBoard->add_control(new xaeGUIButton(1, "close", "CGBoard*close%d", 549 - 25, 6, "", "", "Cursor*Pointer"));
    pCGBoard->add_control(new xaeGUITextButton(10, "Prev", "CGBoard*page%d", 30, 40, "上一页", "黑体", 12, 0xffffffff, 0xffffffff, false, false, true, "", "", "Cursor*Pointer"));
    pCGBoard->add_control(new xaeGUITextButton(11, "Next", "CGBoard*page%d", 549 - 30 - 104, 40, "下一页", "黑体", 12, 0xffffffff, 0xffffffff, false, false, true, "", "", "Cursor*Pointer"));
    m_pShowCG = new xaeGUIStaticImage(0, "CGBoard", "CGBoard*Black", 0, 0);
    m_pShowCG->set_pos(0, 0);
    __init_cg_list(pCGBoard);

    m_pCGBoard->set_show_mode(false);
    m_pShowCG->set_show_mode(false);
}

void xaeWelcomeScene::__init_cg_list(xaeGUIStaticImage* pBoard)
{
    /** CG页 */
    pBoard->add_control(new xaeGUIObject(CG_PAGE_GUI_START_ID + 0, "page1"));
    pBoard->add_control(new xaeGUIObject(CG_PAGE_GUI_START_ID + 1, "page2"));

    const int startx = 15;
    const int starty = 80;

    int j = 0;
    for(int i = 0; i < 2; i++)
    {
        xaeGUIObject* page = pBoard->get_control(CG_PAGE_GUI_START_ID + i);
        page->set_pos(0, 0);
        for(int k = 0; k < 12 && j < g_nCGCount; k++, j++)
        {
            int row = (k / 4), col = (k % 4);
            int x = startx + col * (10 + 122);
            int y = starty + row * (5 + 92);

            int id = j;
            char key[16];
            sprintf(key, "btn%d", id);
            string path = (g_tagSave.cglist[j]) ? (string("CG*") + key + string("%d")) : string("CG*NoCG%d");

            page->add_control(new xaeGUIAlphaButton(id, key, path, x, y, "", "", "Cursor*Pointer"));
            if(!g_tagSave.cglist[j])
            {
                page->get_control(id)->set_enable_mode(false);
            }
        }
    }

    /** CG显示页 */
    for(int i = 0; i < g_nCGCount; i++)
    {
        char key[16];
        sprintf(key, "CG%d", i);
        m_pShowCG->add_control(new xaeGUIStaticImage(i, key, string("CG*") + key, (GAME_WIDTH - 800) >> 1, (GAME_HEIGHT - 480) >> 1));
    }
}

bool xaeWelcomeScene::update(float fDT)
{
    /** 播放BGM */
    if(m_bFirst)
    {
        m_bFirst = true;
        m_pSndBGM->set_loop();
        m_pSndBGM->play();
    }

    m_pFire->update(fDT);

    /** 更新GUI */
    float mousex, mousey;
    m_pHGE->Input_GetMousePos(&mousex, &mousey);
    m_pGUI->update(fDT, mousex, mousey);
    m_pCGBoard->update(fDT, mousex, mousey);
    m_pShowCG->update(fDT, mousex, mousey);

    /** 相应GUI */
    if(m_pGUI->get_control("Exit")->get_mouse_lbutton_clicked())
    {
        return true;
    }
    if(m_pGUI->get_control("Start")->get_mouse_lbutton_clicked())
    {
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Login", "Login");
        if(NULL != scene) xaeSceneMgr::Instance().add_scene("Login", scene);
        xaeSceneMgr::Instance().set_current_scene("Login");

        return false;
    }
    if(m_pGUI->get_control("Settings")->get_mouse_lbutton_clicked())
    {
        xae::Instance().alert("此功能努力复刻中...", "祈祷");
        return false;
    }
    if(m_pGUI->get_control("CG")->get_mouse_lbutton_clicked())
    {
        m_pCGBoard->set_show_mode(true);
        m_pGUI->set_enable_mode(false);
        m_nCGPage = 0;
        m_pCGBoard->get_control("CGBoard")->get_control("page2")->set_show_mode(false);

        return false;
    }

    /** 相应CG GUI */
    if(m_pCGBoard->get_show_mode())
    {
        if(m_pCGBoard->get_control("CGBoard")->get_control("close")->get_mouse_lbutton_clicked())
        {
            m_pCGBoard->set_show_mode(false);
            m_pGUI->set_enable_mode(true);
        }

        if(m_pCGBoard->get_control("CGBoard")->get_control("Next")->get_mouse_lbutton_clicked() && m_nCGPage == 0)
        {
            m_pCGBoard->get_control("CGBoard")->get_control("page1")->set_show_mode(false);
            m_pCGBoard->get_control("CGBoard")->get_control("page2")->set_show_mode(true);
            m_nCGPage = 1;
        }

        if(m_pCGBoard->get_control("CGBoard")->get_control("Prev")->get_mouse_lbutton_clicked() && m_nCGPage == 1)
        {
            m_pCGBoard->get_control("CGBoard")->get_control("page2")->set_show_mode(false);
            m_pCGBoard->get_control("CGBoard")->get_control("page1")->set_show_mode(true);
            m_nCGPage = 0;
        }

        /** 如果显示CG了 */
        int j = 0;
        for(int i = 0; i < 2; i++)
        {
            for(int k = 0; k < 12 && j < g_nCGCount; k++, j++)
            {
                if(m_pCGBoard->get_control("CGBoard")->get_control(CG_PAGE_GUI_START_ID + i)->get_control(j)->get_mouse_lbutton_clicked())
                {
                    m_pCGBoard->set_show_mode(false);
                    m_pShowCG->set_show_mode(true);
                    for(int l = 0; l < g_nCGCount; l++)
                    {
                        m_pShowCG->get_control(l)->set_show_mode(l == j ? true : false);
                    }
                }
            }
        }
    }
    else
    /** 相应SHOW CG GUI */
    if(m_pShowCG->get_show_mode())
    {
        for(int i = 0; i < g_nCGCount; i++)
        {
            if(m_pShowCG->get_control(i)->get_show_mode() && m_pShowCG->get_control(i)->get_mouse_lbutton_clicked())
            {
                m_pShowCG->set_show_mode(false);
                m_pCGBoard->set_show_mode(true);
                for(int i = 0; i < 2; i++) m_pCGBoard->get_control("CGBoard")->get_control(CG_PAGE_GUI_START_ID + i)->set_show_mode(false);
                m_pCGBoard->get_control("CGBoard")->get_control(CG_PAGE_GUI_START_ID + m_nCGPage)->set_show_mode(true);

                break;
            }
        }
    }

    return false;
}

bool xaeWelcomeScene::render()
{
    render_sprite_pool();
    m_pFire->render();

    /** 绘制GUI */
    m_pGUI->render(0, 0);
    m_pCGBoard->render(0, 0);
    m_pShowCG->render(0, 0);

    /** 绘制鼠标 */
    xaeGUIObject::render_mouse();

    return false;
}
