#include "xaePlayScene.h"
#include <tinyxml/tinyxml.h>
#include <algorithm>
using namespace std;

bool _para_msg_sort(para_msg* a, para_msg* b)
{
    return a->paragraph < b->paragraph;
}

bool _para_content_sort(para_content a, para_content b)
{
    return a.index < b.index;
}

bool _para_jpg_sort(para_jpg* a, para_jpg* b)
{
    return a->paragraph < b->paragraph;
}

xaePlayScene::xaePlayScene(void) :
    m_bStageLoaded(false),
    m_szDate(""),
    m_szWeather(""),
    m_szWeek(""),
    m_nCurrentParagraph(0),
    m_nCurrentContent(0),
    m_szCurrentScene(""),
    m_emState(PSS_FADING_IN_BG),
    m_pCurrentSceneSprite(NULL),
    m_pCurrentSound(NULL),
    m_nShakingYOffset(0),
    m_szCurrentText(""),
    m_pFlash(NULL),
    m_pVideo(NULL),
    m_pLaba(NULL),
    m_nStageNum(0),
    m_pCurrentJpg(NULL),
    m_pCurrentMsg(NULL),
    m_pReadLog(NULL)
{
    _init();
}

xaePlayScene::~xaePlayScene(void)
{
    _release_msg();

    for(map<string, xaeSprite*>::iterator it = m_pScenes.begin(); it != m_pScenes.end(); it++)
    {
        it->second->release();
        SAFEDEL(it->second);
    }
    m_pScenes.clear();

    m_pTextBox[0]->release();
    m_pTextBox[1]->release();
    delete m_pTextBox[0];
    delete m_pTextBox[1];

    m_pLaba->release();
    SAFEDEL(m_pLaba);
    SAFEDEL(m_pReadLog);
}

void xaePlayScene::_init()
{
    /** 设置Loader */
    xaeSpriteLoader::Instance().set_xml("media/loader/playui.xml");
    m_pReadLog = new xaePlayReadLog();

    /** 背景 */
    add_sprite_to_pool(
        "StageBackground",
        xaeSpriteLoader::Instance().get_sprite("UI", "StageBackground"),
        (1 + (GAME_WIDTH - 832)) >> 1,
        (GAME_HEIGHT - 504) >> 1
        );

    /** TextBox */
    m_pTextBox[0] = xaeSpriteLoader::Instance().get_sprite("UI", "TextBox");
    m_pTextBox[1] = xaeSpriteLoader::Instance().get_sprite("UI", "TextBoxTalker");

    /** 说话喇叭 */
    m_pLaba = xaeSpriteLoader::Instance().get_sprite("UI", "Horn");

    /** 字体 */
    m_pWRYH = new xaeFont(g_szDefaultFont.c_str(), 16);
    m_pWRYH->SetKerningHeight(3);

    /** GUI */
    m_pGUI = new xaeGUIObject();

    /** 省略号 */
    m_pGUI->add_control(new xaeGUIButton(GUI_ELLIPSIS_ID, "Ellipsis", "UI*Ellipsis%d", 650, 470, "", "", "media/cursor_point.png"));
    m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false);
    m_pGUI->add_control(new xaeGUIButton(GUI_MOUSE_ID, "Mouse", "UI*Mouse%d", 695, 470, "", "", "media/cursor_point.png"));
    m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false);
}

void xaePlayScene::new_stage()
{
    int nextStage = m_nStageNum + 1;

    /** 更新存档 */
    g_tagSave.stage = nextStage;
    save_save();

    if(nextStage != 4 && nextStage != 12 && nextStage != 13)
    {
        char stagename[16];
        sprintf(stagename, "stage%d", 1 + nextStage);
        xaePlayScene* scene = (xaePlayScene*)(xaeSceneMgr::Instance().create_scene("Play", stagename));
        if(NULL != scene)
        {
            xaeSceneMgr::Instance().add_scene(stagename, scene);
            scene->load_stage(nextStage);
        }
        xaeSceneMgr::Instance().set_current_scene(stagename);
    }
    else
    {
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Login", "Login");
        if(NULL != scene) xaeSceneMgr::Instance().add_scene("Login", scene);
        xaeSceneMgr::Instance().set_current_scene("Login");
    }
}

string xaePlayScene::format_text_box_string(string content)
{
    /** 寻找百分号 */
    string n_content = "";
    for(size_t i = 0; i < content.length(); i++)
    {
        n_content += content[i];
        if(content[i] == '%') n_content += '%';
    }
    content = n_content;

    vector<string> strs;
    strs.push_back("");
    int line_cnt = 0;
    for(size_t i = 0; i < content.length(); i++)
    {
        strs[line_cnt] += content[i];

        /** 中文字符 */
        if(content[i] < 0)
        {
            strs[line_cnt] += content[++i];
        }

        if(strs[line_cnt].length() >= LINE_MAX_TEXT)
        {
            line_cnt++;
            strs.push_back("");
        }
    }

    string szStr = "";
    for(size_t i = 0; i < strs.size(); i++)
    {
        if("" != strs[i])
        {
            szStr += strs[i];
            szStr += "\n";
        }
    }

    return szStr;
}

void xaePlayScene::to_paragraph(int para)
{
    /** 清除Sprite */
    if(NULL != m_pCurrentJpg)
    {
        if(m_pCurrentJpg->pLefter != NULL)
        {
            m_pCurrentJpg->pLefter->release();
            delete m_pCurrentJpg->pLefter;
            m_pCurrentJpg->pLefter = NULL;
        }

        if(m_pCurrentJpg->pCenter != NULL)
        {
            m_pCurrentJpg->pCenter->release();
            delete m_pCurrentJpg->pCenter;
            m_pCurrentJpg->pCenter = NULL;
        }

        if(m_pCurrentJpg->pRighter != NULL)
        {
            m_pCurrentJpg->pRighter->release();
            delete m_pCurrentJpg->pRighter;
            m_pCurrentJpg->pRighter = NULL;
        }
    }

    /** 清除分支 */
    if(m_pCurrentMsg)
    {
        for(size_t i = 0; i < m_pCurrentMsg->selects.size(); i++)
        {
            m_pGUI->del_control(i + SELECT_LABEL_START_ID);
        }
    }

    /** Base Data */
    m_nCurrentParagraph = para;
    m_nCurrentContent = 0;
    m_pCurrentMsg = get_msg(m_nCurrentParagraph);
    m_pCurrentJpg = get_jpg(m_nCurrentParagraph);

    /** 本stage结束 */
    if(!m_pCurrentMsg && !m_pCurrentMsg)
    {
        new_stage();
        m_emState = PSS_ALL_END;
        return;
    }

    m_fFadingTimer = FADE_MAX_TIMER;
    m_emState = PSS_PAUSING_TEXT;
    m_fFadingTimer = FADE_MAX_TIMER;
    m_fPausingTimer = PAUSE_MAX_TIMER;
    m_pCurrentSceneSprite = NULL;
    m_szCurrentText = "";
    m_szBufferText = "";
    m_PE.stop_content_snd();
    m_PE.stop_para_snd();

    /** 是否结束场景背景音乐 */
    if(m_pCurrentMsg->end_scene_sound)
    {
        m_PE.stop_scene_snd();
    }
    /** 是否开始场景背景音乐 */
    if(m_pCurrentMsg->scene_sound != "")
    {
        m_PE.set_scene_snd(m_pCurrentMsg->scene_sound);
    }
    /** 是否段落音乐 */
    if(m_pCurrentMsg->sound != "")
    {
        m_PE.set_para_snd(m_pCurrentMsg->sound);
    }

    /** 第一句话 */
    if(m_pCurrentMsg->contents.size() > 0)
    {
        //m_szCurrentText = format_text_box_string(m_pCurrentMsg->contents[0].content);
        m_szBufferText = format_text_box_string(m_pCurrentMsg->contents[0].content);
        if(m_szBufferText.length() > 0)
        {
            m_szCurrentText = m_szBufferText[0];
            if(m_szBufferText.length() > 1 && m_szBufferText[0] < 0)
            {
                m_szCurrentText += m_szBufferText[1];
            }
            else
            /** 转义符 XadillaX+ 2012-11-19 修复bug */
            if(m_szCurrentText == "%")
            {
                m_szCurrentText += m_szBufferText[1];
            }
        }

        if(m_pCurrentMsg->contents[0].sound != "")
        {
            m_PE.set_content_snd(m_pCurrentMsg->contents[0].sound);
        }
    }

    /** Scene */
    if(m_pCurrentJpg != NULL)
    {
        /** 立绘 */
        m_pCurrentJpg->pLefter = NULL;
        m_pCurrentJpg->pCenter = NULL;
        m_pCurrentJpg->pRighter = NULL;
        if(m_pCurrentJpg->lefter != "") m_pCurrentJpg->pLefter = xaeSpriteLoader::Instance().get_sprite("立绘", m_pCurrentJpg->lefter);
        if(m_pCurrentJpg->center != "") m_pCurrentJpg->pCenter = xaeSpriteLoader::Instance().get_sprite("立绘", m_pCurrentJpg->center);
        if(m_pCurrentJpg->righter != "") m_pCurrentJpg->pRighter = xaeSpriteLoader::Instance().get_sprite("立绘", m_pCurrentJpg->righter);

        /** 是否播放影片 */
        if(m_pCurrentMsg->is_movie)
        {
            /** 读取影片数据 */
            xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
            xaeResourceBody tagRB;
            bool bSucc = pRMO->get_resource_bin("media/video/op.avi", tagRB);
            /** 读取失败 */
            if(!bSucc)
            {
                m_emState = PSS_CAN_NEXT;
                return;
            }

            /** 写入临时目录 */
            xae::Instance().create_temp_folder(GAME_VIDEO_PATH);
            FILE* fp = fopen(string(string(GAME_VIDEO_PATH) + "/op.avi").c_str(), "wb+");
            if(!fp)
            {
                m_emState = PSS_CAN_NEXT;
                return;
            }
            fwrite(tagRB.m_pRes, tagRB.m_nSize, 1, fp);
            fclose(fp);

            /** 创建播放器指针 */
            m_pHGE->System_SetState(HGE_WINDOWED, true);
            m_pVideo = new CXadillaXVideoPlayer();
            RECT playrect;
            playrect.left = (GAME_WIDTH - GAME_REAL_WIDTH) >> 1;
            playrect.top = (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1;
            playrect.right = playrect.left + GAME_REAL_WIDTH;
            playrect.bottom = playrect.top + GAME_REAL_HEIGHT;
            m_pVideo->LoadFile(
                string(string(GAME_VIDEO_PATH) + "/op.avi").c_str(),
                playrect,
                g_hWnd,
                true
                );
            m_pVideo->Play();

            m_emState = PSS_PLAYING_MOVIE;

            m_pGUI->set_mouse(IDC_ARROW);
        }
        else
        /** 载入场景背景图 */
        if(m_pCurrentJpg->scence_swf == "")
        {
            char scenenum[16];
            sprintf(scenenum, "%d", m_pCurrentJpg->scence_num);
            m_pCurrentSceneSprite = get_scene_sprite(scenenum);

            if(m_szCurrentScene != string(scenenum))
            {
                m_szCurrentScene = scenenum;
                m_emState = PSS_FADING_IN_BG;

                /** 是否是CG，如果是则保存更新状态：更新此CG */
                for(int i = 0; i < g_nCGCount; i++)
                {
                    if(m_pCurrentJpg->scence_num == g_nCGList[i])
                    {
                        g_tagSave.cglist[i] = true;
                        save_save();
                        break;
                    }
                }
            }
        }
        /** 是否播放Flash */
        else
        {
            if(!m_pCurrentMsg->is_movie)
            {
                m_pFlash = new xaeFlashSprite();
                char *fn = new char[16 + m_pCurrentJpg->scence_swf.length()];
                sprintf(fn, "media/flash/%s", m_pCurrentJpg->scence_swf.c_str());
                m_pFlash->StartFlash(fn, GAME_REAL_WIDTH, GAME_REAL_HEIGHT);
                delete []fn;

                m_pCurrentSceneSprite = NULL;
                m_szCurrentScene = "";

                m_emState = PSS_PLAYING_FLASH;
            }
        }
    }
}

int xaePlayScene::get_next_paragraph(int para)
{
    for(size_t i = 0; i < m_Msg.size(); i++)
    {
        if(m_Msg[i]->paragraph == para)
        {
            if(m_Msg[i]->next_p != -1) return m_Msg[i]->next_p;
            else
            {
                if(i < m_Msg.size() - 1) return m_Msg[i + 1]->paragraph;
                else return -1;
            }
        }
    }

    return -1;
}

bool xaePlayScene::update(float fDT)
{
    float mx, my;
    m_pHGE->Input_GetMousePos(&mx, &my);
    m_pGUI->update(fDT, mx, my);

    /** 返回 */
    if(xae::Instance().keyup(HGEK_ESCAPE))
    {
        xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Login", "Login");
        if(NULL != scene) xaeSceneMgr::Instance().add_scene("Login", scene);
        xaeSceneMgr::Instance().set_current_scene("Login");
    }

    switch(m_emState)
    {
        /** 淡入背景 */
    case PSS_FADING_IN_BG: _update_fade_in_bg(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false); break;
        /** 暂停文字 */
    case PSS_PAUSING_TEXT: _update_pause_text(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false); break;
        /** 可以到下一段文字 */
    case PSS_CAN_NEXT: _update_can_next(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(true); break;
        /** 淡出背景 */
    case PSS_FADING_OUT_BG: _update_fade_out_bg(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false); break;
        /** 播放Flash */
    case PSS_PLAYING_FLASH: _update_flash(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false); break;
        /** 播放影片 */
    case PSS_PLAYING_MOVIE: _update_movie(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false); break;
        /** 分支选择 */
    case PSS_SELECT: _update_select(fDT); m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(true); break;
        /** 浏览记录 */
    case PSS_READING_LOG:
        {
            m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false);
            if(m_pReadLog->update(fDT))
            {
                m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(true);
                m_emState = PSS_CAN_NEXT;
                break;
            }
        }

    default: break;
    }

    if(m_szBufferText.length() == 0 || !(m_emState == PSS_PAUSING_TEXT || m_emState == PSS_CAN_NEXT))
    {
        m_pGUI->get_control(GUI_ELLIPSIS_ID)->set_show_mode(false);
        m_pGUI->get_control(GUI_MOUSE_ID)->set_show_mode(false);
    }
    else
    if(m_szBufferText.length() != 0 && (m_emState == PSS_PAUSING_TEXT || m_emState == PSS_CAN_NEXT))
    {
        m_pGUI->get_control(GUI_MOUSE_ID)->set_show_mode(true);
    }

    m_PE.update(fDT);

    return false;
}

void xaePlayScene::_update_fade_in_bg(float fDT)
{
    m_fFadingTimer -= fDT;
    if(m_fFadingTimer <= 0)
    {
        m_fFadingTimer = FADE_MAX_TIMER;
        m_emState = PSS_PAUSING_TEXT;
    }
}

void xaePlayScene::_update_fade_out_bg(float fDT)
{
    m_fFadingTimer -= fDT;
    if(m_fFadingTimer <= 0)
    {
        to_paragraph(get_next_paragraph(m_nCurrentParagraph));
    }
}

void xaePlayScene::_update_pause_text(float fDT)
{
    m_fPausingTimer -= fDT;

    /** 计时到 */
    if(m_fPausingTimer <= 0)
    {
        m_fPausingTimer = PAUSE_MAX_TIMER;

        /** 播放完毕 */
        if(m_szCurrentText == m_szBufferText)
        {
            if(m_pCurrentMsg->talker != "")
            {
                m_pReadLog->AddLine(m_pCurrentMsg->talker + "：" + m_pCurrentMsg->contents[m_nCurrentContent].content);
            }
            else m_pReadLog->AddLine(m_pCurrentMsg->contents[m_nCurrentContent].content);

            m_emState = PSS_CAN_NEXT;
        }
        else
        {
            m_szCurrentText += m_szBufferText[m_szCurrentText.length()];

            /** 遇到回车 */
            if(m_szCurrentText[m_szCurrentText.length() - 1] == '\n' && m_szCurrentText != m_szBufferText)
            {
                m_szCurrentText += m_szBufferText[m_szCurrentText.length()];
            }

            /** 遇到中文 */
            if(m_szCurrentText[m_szCurrentText.length() - 1] < 0 && m_szCurrentText != m_szBufferText)
            {
                m_szCurrentText += m_szBufferText[m_szCurrentText.length()];
            }

            /** 遇到百分号 */
            if(m_szCurrentText[m_szCurrentText.length() - 1] == '%' && m_szCurrentText != m_szBufferText)
            {
                m_szCurrentText += m_szBufferText[m_szCurrentText.length()];
            }
        }
    }

    /** 鼠标手动按 */
    bool lb = xae::Instance().keyup(HGEK_LBUTTON);
    bool sp = xae::Instance().keyup(HGEK_SPACE);
    bool en = xae::Instance().keyup(HGEK_ENTER);
    if(lb || sp || en)
    {
        bool fly = false;
        float mx, my;
        m_pHGE->Input_GetMousePos(&mx, &my);

        /** 按回车、空格或者对话框内按鼠标左键都可以到下一句 */
        if(lb && 
            (
            (
            m_szCurrentText != "" && mx >= (GAME_WIDTH - 690) >> 1 &&
            mx <= ((GAME_WIDTH - 690) >> 1) + 690 &&
            my >= ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 149 - 20 &&
            my <= ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 20
            ) ||
            m_szCurrentText == ""
            ) 
            )
        {
            fly |= true;
        }
        if(sp) fly |= true;
        if(en) fly |= true;

        /** 如果字要飞 */
        if(fly)
        {
            m_szCurrentText = m_szBufferText;
            if(m_pCurrentMsg->talker != "")
            {
                m_pReadLog->AddLine(m_pCurrentMsg->talker + "：" + m_pCurrentMsg->contents[m_nCurrentContent].content);
            }
            else m_pReadLog->AddLine(m_pCurrentMsg->contents[m_nCurrentContent].content);
            m_fPausingTimer = PAUSE_MAX_TIMER;
            m_emState = PSS_CAN_NEXT;
        }
    }
}

void xaePlayScene::_update_can_next(float fDT)
{
    /** 若是浏览记录 */
    if(m_pGUI->get_control(GUI_ELLIPSIS_ID)->get_mouse_lbutton_clicked())
    {
        m_emState = PSS_READING_LOG;
        return;
    }

    bool lb = xae::Instance().keyup(HGEK_LBUTTON);
    bool sp = xae::Instance().keyup(HGEK_SPACE);
    bool en = xae::Instance().keyup(HGEK_ENTER);
    if(lb || sp || en)
    {
        bool change = false;
        float mx, my;
        m_pHGE->Input_GetMousePos(&mx, &my);

        /** 按回车、空格或者对话框内按鼠标左键都可以到下一句 */
        if(lb && 
            (
                (
                    m_szCurrentText != "" && mx >= (GAME_WIDTH - 690) >> 1 &&
                    mx <= ((GAME_WIDTH - 690) >> 1) + 690 &&
                    my >= ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 149 - 20 &&
                    my <= ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 20
                ) ||
                m_szCurrentText == ""
            ) 
        )
        {
            change |= true;
        }
        if(sp) change |= true;
        if(en) change |= true;

        /** 如果在对话框之中 */
        if(change)
        {
            m_nCurrentContent++;

            if(m_nCurrentContent >= m_pCurrentMsg->contents.size())
            {
                /** 如果有分支 */
                if(!m_pCurrentMsg->selects.empty())
                {
                    /** 原内容大小 */
                    SIZE sz = m_pWRYH->GetTextSize(C2W(m_szCurrentText.c_str()));
                    //SIZE sz = m_pWRYH->GetTextSize(C2W(m_szBufferText.c_str()));

                    /** 第一个分支的位置 */
                    int firstx = ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1) + 110;
                    int firsty = ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 125;
                    firsty += (sz.cy - m_pWRYH->GetFontSize());

                    /** 添加GUI */
                    for(size_t i = 0; i < m_pCurrentMsg->selects.size(); i++)
                    {
                        /** 新建分支GUI */
                        xaeSelectLabel* pSel = new xaeSelectLabel(
                            SELECT_LABEL_START_ID + i,
                            m_pCurrentMsg->selects[i].content,
                            m_pCurrentMsg->selects[i].start_select_paragrapha,
                            i,
                            firstx,
                            firsty + (m_pWRYH->GetFontSize() + m_pWRYH->GetKerningHeight()) * i);

                        /** 加入GUI中 */
                        m_pGUI->add_control((xaeGUIObject*)pSel);
                    }

                    m_emState = PSS_SELECT;

                    return;
                }
                else
                /** 如果没分支 */
                {
                    int para = get_next_paragraph(m_nCurrentParagraph);

                    /** 获取下一场景图片 */
                    para_jpg* pJpg = get_jpg(para);
                    char scenenum[16] = "";
                    if(NULL != pJpg)
                    {
                        sprintf(scenenum, "%d", pJpg->scence_num);
                    }

                    /** 若场景不同，则需要淡入淡出 */
                    if(m_szCurrentScene != string(scenenum))
                    {
                        m_emState = PSS_FADING_OUT_BG;
                    }
                    else
                    /** 否则直接转 */
                    {
                        to_paragraph(para);
                    }
                }
            }
            else
            {
                //m_szCurrentText += format_text_box_string(m_pCurrentMsg->contents[m_nCurrentContent].content);
                m_szBufferText += format_text_box_string(m_pCurrentMsg->contents[m_nCurrentContent].content);
                m_emState = PSS_PAUSING_TEXT;
                m_PE.stop_content_snd();

                if(m_pCurrentMsg->contents[m_nCurrentContent].sound != "")
                {
                    m_PE.set_content_snd(m_pCurrentMsg->contents[m_nCurrentContent].sound);
                }
            }
        }
    }
}

void xaePlayScene::_update_flash(float fDT)
{
    if(m_pFlash)
    {
        m_pFlash->Update(fDT);

        if(!m_pFlash->IsPlaying())
        {
            m_emState = PSS_CAN_NEXT;
            SAFEDEL(m_pFlash);
        }
        else
        {
            if(xae::Instance().keystate(HGEK_SPACE))
            {
                m_emState = PSS_CAN_NEXT;
                SAFEDEL(m_pFlash);
            }
        }
    }
    else
    {
        m_emState = PSS_CAN_NEXT;
    }
}

void xaePlayScene::_update_movie(float fDT)
{
    xae::Instance().set_no_more_render();
    if(m_pVideo)
    {
        if(!m_pVideo->IsPlaying())
        {
            m_emState = PSS_CAN_NEXT;
            SAFEDEL(m_pVideo);

            xae::Instance().set_no_more_render(false);

            DeleteFile(string(string(GAME_VIDEO_PATH) + "/op.avi").c_str());

            m_pGUI->set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());
        }
        else
        {
            if(xae::Instance().keystate(HGEK_SPACE))
            {
                m_emState = PSS_CAN_NEXT;
                SAFEDEL(m_pVideo);

                xae::Instance().set_no_more_render(false);

                DeleteFile(string(string(GAME_VIDEO_PATH) + "/op.avi").c_str());

                m_pGUI->set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());
            }
        }
    }
    else m_emState = PSS_CAN_NEXT;
}

void xaePlayScene::_update_select(float fDT)
{
    if(!m_pCurrentMsg) return;
    for(size_t i = 0; i < m_pCurrentMsg->selects.size(); i++)
    {
        xaeSelectLabel* pSel = (xaeSelectLabel*)m_pGUI->get_control(SELECT_LABEL_START_ID + i);
        if(pSel && pSel->get_mouse_lbutton_clicked())
        {
            m_pGUI->set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());
            to_paragraph(pSel->get_next_para());
            return;
        }
    }
}

bool xaePlayScene::render()
{
    //m_pHGE->Gfx_Clear(0xffafafaf);
    render_sprite_pool();

    switch(m_emState)
    {
        /** 淡入背景 */
    case PSS_FADING_IN_BG:
        {
            if(NULL != m_pCurrentSceneSprite)
            {
                /** 背景透明度 */
                hgeColor Clr;
                Clr.SetHWColor(0x00ffffff);
                Clr.a = 1.0f - (m_fFadingTimer / FADE_MAX_TIMER);
                m_pCurrentSceneSprite->get_sprite()->SetColor(Clr.GetHWColor());

                /** 渲染背景 */
                m_pCurrentSceneSprite->render(
                    (GAME_WIDTH - GAME_REAL_WIDTH) >> 1,
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }

            break;
        }

        /** 淡出背景 */
    case PSS_FADING_OUT_BG:
        {
            if(NULL != m_pCurrentSceneSprite)
            {
                /** 背景透明度 */
                hgeColor Clr;
                Clr.SetHWColor(0x00ffffff);
                Clr.a = (m_fFadingTimer / FADE_MAX_TIMER);
                m_pCurrentSceneSprite->get_sprite()->SetColor(Clr.GetHWColor());

                /** 渲染背景 */
                m_pCurrentSceneSprite->render(
                    (GAME_WIDTH - GAME_REAL_WIDTH) >> 1,
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }

            break;
        }

        /** 播放flash */
    case PSS_PLAYING_FLASH:
        {
            if(NULL != m_pFlash)
            {
                m_pFlash->Render(
                    (GAME_WIDTH - GAME_REAL_WIDTH) >> 1,
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }

            break;
        }

    case PSS_PLAYING_MOVIE: break;

        /** 显示文字 */
    case PSS_PAUSING_TEXT:
    case PSS_CAN_NEXT:
    case PSS_READING_LOG:
    case PSS_SELECT:
        {
            if(NULL != m_pCurrentSceneSprite)
            {
                m_pCurrentSceneSprite->get_sprite()->SetColor(0xffffffff);

                /** 渲染背景 */
                m_pCurrentSceneSprite->render(
                    (GAME_WIDTH - GAME_REAL_WIDTH) >> 1,
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }

            /** 渲染立绘 */
            if(m_pCurrentJpg->pLefter != NULL)
            {
                m_pCurrentJpg->pLefter->render(
                    (GAME_WIDTH - GAME_REAL_WIDTH) >> 1,
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }
            if(m_pCurrentJpg->pRighter != NULL)
            {
                m_pCurrentJpg->pRighter->render(
                    ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1) + GAME_REAL_WIDTH - 480,
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }
            if(m_pCurrentJpg->pCenter != NULL)
            {
                m_pCurrentJpg->pCenter->render(
                    ((GAME_WIDTH - 480) >> 1),
                    (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1
                    );
            }

            /** 如果有Talker */
            xaeSprite* pTextBox;
            pTextBox = (m_pCurrentMsg->talker != "") ? m_pTextBox[1] : m_pTextBox[0];

            /** 对话框 */
            if(m_szCurrentText != "" || !m_pCurrentMsg->selects.empty())
            {
                pTextBox->render(
                    (GAME_WIDTH - 690) >> 1,
                    ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 149 - 20
                    );
            }

            /** Talker */
            if(m_pCurrentMsg->talker != "")
            {
                m_pWRYH->SetColor(0xffff7800);
                m_pWRYH->Print(
                    ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1) + 110,
                    ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + 323,
                    m_pCurrentMsg->talker.c_str()
                    );

                /** Horn */
                if(m_pLaba) m_pLaba->render(
                    ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1) + 70,
                    ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + 323
                    );
            }

            /** 文字 */
            SIZE fontSize = m_pWRYH->GetTextSize(C2W(m_szCurrentText.c_str()));
            m_pWRYH->SetColor(0xff000000);
            m_pWRYH->Print(
                ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1) + 110,
                ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + GAME_REAL_HEIGHT - 125,
                m_szCurrentText.c_str()
                );

            /** 浏览记录 */
            if(m_emState == PSS_READING_LOG)
            {
                m_pReadLog->render();
            }

            break;
        }

    default: break;
    }

    m_pGUI->render(0, 0);
    if(m_emState != PSS_READING_LOG) m_pGUI->render_mouse();

    return false;
}

bool xaePlayScene::load_stage(int idx)
{
    if(idx >= g_nStageCount) return false;

    /** 读入Msg XML */
    string stageXMLPath = "media/xml/main_config_msg" + string(g_szStageNum[idx]) + ".xml";
    TiXmlDocument tagXML;
    xaeResourceBody tagBody;
    bool bSucc = xae::Instance().get_resource_mgr()->get_resource_bin(stageXMLPath, tagBody);
    if(!bSucc) return false;
    tagXML.Clear();
    tagXML.Parse((char*)tagBody.m_pRes);
    _parse_msg(tagXML);

    /** 读入Jpg XML */
    stageXMLPath = "media/xml/main_config_jpg" + string(g_szStageNum[idx]) + ".xml";
    bSucc = xae::Instance().get_resource_mgr()->get_resource_bin(stageXMLPath, tagBody);
    if(!bSucc) return false;
    tagXML.Clear();
    tagXML.Parse((char*)tagBody.m_pRes);
    _parse_jpg(tagXML);

    /** 设置Loader XML */
    xaeSpriteLoader::Instance().set_xml("media/loader/playimg.xml");

    if(m_Msg.size() == 0) return false;

    /** 初始化 */
    to_paragraph(m_Msg[0]->paragraph);

    m_nStageNum = idx;
    return true;
}

void xaePlayScene::_parse_msg(TiXmlDocument& xml)
{
    TiXmlNode* pNode = xml.RootElement();
    TiXmlElement* pParaDate = pNode->FirstChildElement("para_date");

    /** 天气情况 */
    m_szDate = pParaDate->Attribute("date") == 0 ? "" : pParaDate->Attribute("date");
    m_szWeather = pParaDate->Attribute("weather") == 0 ? "" : pParaDate->Attribute("weather");
    m_szWeek = pParaDate->Attribute("week") == 0 ? "" : pParaDate->Attribute("week");

    m_pReadLog->SetDate(m_szDate, m_szWeek, m_szWeather);

    /** Msg */
    for(TiXmlElement* pMsg = pParaDate->FirstChildElement("para_msg");
        pMsg != NULL;
        pMsg = pMsg->NextSiblingElement("para_msg"))
    {
        para_msg* msg = new para_msg();
        
        int result;
        
        /** 段落 */
        result = pMsg->QueryIntAttribute("paragraph", &msg->paragraph);
        
        /** 下一段落 */
        result = pMsg->QueryIntAttribute("next_p", &msg->next_p);
        if(result == TIXML_NO_ATTRIBUTE) msg->next_p = -1;

        /** Start Sound */
        msg->scene_sound = pMsg->Attribute("start_scence_sound") == 0 ? "" : pMsg->Attribute("start_scence_sound");

        /** Para sound */
        msg->sound = pMsg->Attribute("sound") == 0 ? "" : pMsg->Attribute("sound");

        /** End Sound */
        result = pMsg->QueryBoolAttribute("end_scence_sound", &msg->end_scene_sound);
        if(result == TIXML_NO_ATTRIBUTE) msg->end_scene_sound = false;

        /** Is Movie */
        result = pMsg->QueryBoolAttribute("is_flv", &msg->is_movie);
        if(result == TIXML_NO_ATTRIBUTE) msg->is_movie = false;
        if(!msg->is_movie)
        {
            result = pMsg->QueryBoolAttribute("isflv", &msg->is_movie);
            if(result == TIXML_NO_ATTRIBUTE) msg->is_movie = false;
        }

        /** Content */
        _parse_content(pMsg, msg);

        m_Msg.push_back(msg);
    }

    /** 排序 */
    sort(m_Msg.begin(), m_Msg.end(), ::_para_msg_sort);
}

void xaePlayScene::_parse_content(TiXmlElement* pMsgNode, para_msg* pMsg)
{
    /** Content */
    for(TiXmlElement* pCntNode = pMsgNode->FirstChildElement("content");
        pCntNode != NULL;
        pCntNode = pCntNode->NextSiblingElement("content"))
    {
        para_content content;
        int result;

        /** Effect: TODO */

        /** Index */
        result = pCntNode->QueryIntAttribute("index", &content.index);
        if(result == TIXML_NO_ATTRIBUTE) content.index = 1;

        /** Talker */
        string talker;
        talker = pCntNode->Attribute("talker") == 0 ? "" : pCntNode->Attribute("talker");
        if(talker != "") pMsg->talker = talker;

        /** Shake */
        string shake;
        shake = pCntNode->Attribute("effect_type") == 0 ? "" : pCntNode->Attribute("effect_type");
        if(shake != "")
        {
            XStringFunc xstr(shake);
            xstr.ToUpper();
            if(xstr.GetString() == "CLICKOPEN")
            {
                content.shake_start = true;
            }
            if(xstr.GetString() == "CLICKSTOP")
            {
                content.shake_end = true;
            }
            if(xstr.GetString() == "TIMERSTOP")
            {
                content.shake_timer = true;
            }
        }

        /** Sound */
        string sound;
        sound = pCntNode->Attribute("sound") == 0 ? "" : pCntNode->Attribute("sound");

        /** Content */
        content.content = pCntNode->GetText() == 0 ? "" : pCntNode->GetText();

        /** 转义符 */
        //string tmpContent = "";
        //for(int i = 0; i < content.content.length(); i++)
        //{
        //    if(content.content[i] < 0)
        //    {
        //        tmpContent += content.content[i];
        //        i++;
        //        tmpContent += content.content[i];
        //        continue;
        //    }

        //    if(content.content[i] == '\\')
        //    {
        //        tmpContent += "\\\\";
        //        continue;
        //    }

        //    if(content.content[i] == '%')
        //    {
        //        tmpContent += "%%";
        //        continue;
        //    }

        //    tmpContent += content.content[i];
        //}
        //content.content = tmpContent;

        pMsg->contents.push_back(content);
    }
    /** Sort */
    sort(pMsg->contents.begin(), pMsg->contents.end(), _para_content_sort);

    /** Select Label */
    for(TiXmlElement* pSltNode = pMsgNode->FirstChildElement("select_label");
        pSltNode != NULL;
        pSltNode = pSltNode->NextSiblingElement("select_label"))
    {
        para_select select;
        int result;

        /** Next Paragraph */
        result = pSltNode->QueryIntAttribute("start_select_paragrapha", &select.start_select_paragrapha);

        /** Type */
        result = pSltNode->QueryIntAttribute("select_type", &select.select_type);

        /** Content */
        select.content = pSltNode->GetText() == 0 ? "" : pSltNode->GetText();

        pMsg->selects.push_back(select);
    }
}

void xaePlayScene::_parse_jpg(TiXmlDocument& xml)
{
    TiXmlNode* pNode = xml.RootElement();
    TiXmlElement* pParaDate = pNode->FirstChildElement("para_date");

    /** 遍历结点 */
    for(TiXmlElement* pJpgNode = pParaDate->FirstChildElement("para_jpg");
        pJpgNode != NULL;
        pJpgNode = pJpgNode->NextSiblingElement("para_jpg"))
    {
        para_jpg* jpg = new para_jpg();
        int result;

        /** Paragraph */
        result = pJpgNode->QueryIntAttribute("paragraph", &jpg->paragraph);
        if(result == TIXML_NO_ATTRIBUTE) jpg->paragraph = 0;

        /** Scene Number */
        string swf = "";
        swf = pJpgNode->Attribute("scence_num") == 0 ? "" : pJpgNode->Attribute("scence_num");
        XStringFunc xstr(swf);
        xstr.ToUpper();
        swf = xstr.GetString();
        if(swf.find(".SWF") != string::npos)
        {
            jpg->scence_swf = swf;
            jpg->scence_num = -1;
        }
        else
        {
            result = pJpgNode->QueryIntAttribute("scence_num", &jpg->scence_num);
            if(result == TIXML_NO_ATTRIBUTE) jpg->scence_num = 0;
        }

        /** 立绘 */
        jpg->lefter = pJpgNode->Attribute("lefter") == 0 ? "" : pJpgNode->Attribute("lefter");
        jpg->center = pJpgNode->Attribute("center") == 0 ? "" : pJpgNode->Attribute("center");
        jpg->righter = pJpgNode->Attribute("righter") == 0 ? "" : pJpgNode->Attribute("righter");

        m_Jpg.push_back(jpg);
    }
    sort(m_Jpg.begin(), m_Jpg.end(), ::_para_jpg_sort);
}

void xaePlayScene::_release_msg()
{
    for(size_t i = 0; i < m_Msg.size(); i++)
    {
        SAFEDEL(m_Msg[i]);
    }

    m_Msg.clear();

    for(size_t i = 0; i < m_Jpg.size(); i++)
    {
        if(NULL != m_Jpg[i])
        {
            if(m_Jpg[i]->pLefter != NULL)
            {
                m_Jpg[i]->pLefter->release();
                SAFEDEL(m_Jpg[i]->pLefter);
            }
            if(m_Jpg[i]->pRighter != NULL)
            {
                m_Jpg[i]->pRighter->release();
                SAFEDEL(m_Jpg[i]->pRighter);
            }
            if(m_Jpg[i]->pCenter != NULL)
            {
                m_Jpg[i]->pCenter->release();
                SAFEDEL(m_Jpg[i]->pCenter);
            }
        }
        SAFEDEL(m_Jpg[i]);
    }

    m_Jpg.clear();
}

para_msg* xaePlayScene::get_msg(int paragraph)
{
    for(size_t i = 0; i < m_Msg.size(); i++)
    {
        if(m_Msg[i]->paragraph == paragraph) return m_Msg[i];
    }

    return NULL;
}

para_jpg* xaePlayScene::get_jpg(int paragraph)
{
    for(size_t i = 0; i < m_Jpg.size(); i++)
    {
        if(m_Jpg[i]->paragraph == paragraph) return m_Jpg[i];
    }

    return NULL;
}

xaeSprite* xaePlayScene::get_scene_sprite(string scenename)
{
    if(m_pScenes.find(scenename) != m_pScenes.end()) return m_pScenes[scenename];
    else
    {
        xaeSprite* spr = xaeSpriteLoader::Instance().get_sprite("scene", scenename);
        m_pScenes[scenename] = spr;
        return spr;
    }

    return NULL;
}
