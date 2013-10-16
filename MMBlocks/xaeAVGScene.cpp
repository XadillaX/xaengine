#include "xaeAVGScene.h"
#include "xaeWelcomeScene.h"
#include "xaeBlockScene.h"

DWORD WINAPI createAVGThread(LPVOID lpParam)
{
    loadingThreadPara* para = (loadingThreadPara*)lpParam;
    xaeAVGScene* scene;
    para->m_szSceneName = para->m_szSceneType = "AVG";

    printf("Creating AVG Scene...\n");
    scene = new xaeAVGScene();
    scene->set_scene_name(para->m_szSceneName);
    printf("Created...\n");

    LockScope oScope(para->m_CriticalSection);
    para->m_pScene = scene;
    para->m_bCreated = true;

    return 0;
}

xaeAVGScene::xaeAVGScene(void) :
    m_szScript(""),
    m_dwScriptCursor(0),
    m_emStatus(XADS_PAUSED),
    m_pBackground(NULL),
    m_bRevived(false),
    m_bPreBossed(false),
    m_pVideo(NULL)
{
    /** ֹͣBGM */
    g_BGMPlayer.stopBGM();

    /** ��ʼ������map */
    m_mpMadeDraw.clear();

    /** ��ʼ���ز� */
    __loadResource();

    /** ����ű� */
    __loadSript();

    /** ��ʼ�� */
    parseNextPause();
}

xaeAVGScene::~xaeAVGScene(void)
{
    if(NULL != m_pBackground)
    {
        delete m_pBackground;
        m_pBackground = NULL;
    }

    for(map<string, xaeMadeDraw>::iterator it = m_mpMadeDraw.begin(); it != m_mpMadeDraw.end(); it++)
    {
        delete it->second.m_pScript;
    }
    m_mpMadeDraw.clear();

    SAFEDEL(m_pVideo);
}

void xaeAVGScene::__loadSript()
{
    /** ����Դ�������л�ȡ�ű��ļ� */
    xaeResourceBody tagRB;
    xae::Instance().get_resource_mgr()->get_resource_bin(g_Setting.File.m_szScriptFile, tagRB);

    /** ���ű�Ū���ַ����� */
    char* m_pScript = new char[tagRB.m_nSize + 5];
    memcpy(m_pScript, tagRB.m_pRes, tagRB.m_nSize * sizeof(char));
    m_pScript[tagRB.m_nSize] = '\0';
    m_szScript = m_pScript;
    delete []m_pScript;

    /** �ű�λ���α�����Ϊ0 */
    setScriptCursor(0);
}

void xaeAVGScene::__loadResource()
{
    //m_pTextbox = xaeSpriteLoader::Instance().get_sprite("AVG", "textbox");
}

void xaeAVGScene::setScriptCursor(DWORD scriptCursor)
{
    if(scriptCursor >= m_szScript.length())
    {
        m_dwScriptCursor = m_szScript.length() - 1;
    }
    else m_dwScriptCursor = scriptCursor;
}

DWORD xaeAVGScene::getScriptCursor()
{
    return m_dwScriptCursor;
}

bool xaeAVGScene::render()
{
    /** ���� */
    if(NULL != m_pBackground)
    {
        m_pBackground->render();
    }

    /** ���� */
    for(map<string, xaeMadeDraw>::iterator it = m_mpMadeDraw.begin(); it != m_mpMadeDraw.end(); it++)
    {
        it->second.m_pScript->render(it->second.x, it->second.y);
    }

    m_Textbox.render();

    xaeGUIObject::render_mouse();

    return false;
}

bool xaeAVGScene::update(float fDT)
{
    g_BGMPlayer.update(fDT);

    /** ����ڲ�����Ƶ */
    if(m_pVideo != NULL)
    {
        xae::Instance().set_no_more_render();

        /** ��������� */
        bool bFinished = false;
        if(!m_pVideo->IsPlaying())
        {
            string szFilename = m_pVideo->GetFilename();
            SAFEDEL(m_pVideo);
            xae::Instance().set_no_more_render(false);

            DeleteFile(szFilename.c_str());

            xaeGUIObject::set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());
            bFinished = true;
        }
        else
        {
            if(xae::Instance().keystate(HGEK_SPACE))
            {
                string szFilename = m_pVideo->GetFilename();
                SAFEDEL(m_pVideo);

                xae::Instance().set_no_more_render(false);

                DeleteFile(szFilename.c_str());

                xaeGUIObject::set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());
                bFinished = true;
            }
        }

        if(!bFinished) return false;
        else
        {
            if(!parseNextPause())
            {
                xaeWelcomeScene* scene = (xaeWelcomeScene*)(xaeSceneMgr::Instance().create_scene("Welcome", "Welcome"));
                if(NULL != scene)
                {
                    xaeSceneMgr::Instance().add_scene("Welcome", scene);
                }
                xaeSceneMgr::Instance().set_current_scene("Welcome");
            }

            return false;
        }
    }

    /** �������� */
    if(XADS_PLAYING_VOICE == m_emStatus || XADS_PAUSED == m_emStatus)
    {
        float mx, my;
        m_pHGE->Input_GetMousePos(&mx, &my);

        if((my >= g_Setting.AVG.m_nTextboxTop && my <= g_Setting.AVG.m_nTextboxBottom &&
            mx >= g_Setting.AVG.m_nTextboxLeft && mx <= g_Setting.AVG.m_nTextboxRight &&
            xae::Instance().keyup(HGEK_LBUTTON)) ||
            xae::Instance().keyup(HGEK_SPACE) ||
            xae::Instance().keyup(HGEK_ENTER) || !m_Textbox.hasTextbox())
        {
            m_Textbox.clearVoice();
            
            /** ������һ���䣬��������ͷ�� */
            if(!parseNextPause())
            {
                xaeWelcomeScene* scene = (xaeWelcomeScene*)(xaeSceneMgr::Instance().create_scene("Welcome", "Welcome"));
                if(NULL != scene)
                {
                    xaeSceneMgr::Instance().add_scene("Welcome", scene);
                }
                xaeSceneMgr::Instance().set_current_scene("Welcome");

                /** ����Welcome��������ʾ */
                //xaeLoadingScene* loadingScene = (xaeLoadingScene*)(xaeSceneMgr::Instance().get_scene("Loading"));
                //loadingScene->loadingSceneAndShow(createWelcomeThread);
                //xaeSceneMgr::Instance().set_current_scene("Loading");
            }

            return false;
        }
    }

    /** �����ı��� */
    XAE_AVG_DISPLAY_STATUS emTextStatus = m_Textbox.update(fDT);

    /** ������ʾ״̬ */
    m_emStatus = emTextStatus;

    return false;
}

bool xaeAVGScene::parsePause(DWORD scriptCursor)
{
    /** ��ͷ�� */
    if(scriptCursor >= m_szScript.length()) return false;

    int pos = scriptCursor;
    string szCommand = "";

    /** ����ָ�� */
    do {
        szCommand = "";

        while(pos < m_szScript.length() && (m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r')) pos++;
        while(pos < m_szScript.length() && !(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
        {
            szCommand += m_szScript[pos];
            pos++;
        }
        if(pos == m_szScript.length() && szCommand == "")
        {
            /** ������ */
            return false;
        }

        /** ָ���д�� */
        for(int i = 0; i < szCommand.length(); i++)
        {
            if(szCommand[i] < 0)
            {
                i++;
                continue;
            }

            if(szCommand[i] >= 'a' && szCommand[i] <= 'z')
            {
                szCommand[i] = szCommand[i] - 'a' + 'A';
            }
        }

        /** ����ָ�� */
        if(szCommand == "����" || szCommand == "BG")
        {
            string szFilename = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(m_szScript[pos] != '\n' && m_szScript[pos] != '\r')
            {
                szFilename += m_szScript[pos];
                pos++;
            }

            /** ���뱳�� */
            if(NULL != m_pBackground)
            {
                delete m_pBackground;
                m_pBackground = NULL;
            }

            szFilename = g_Setting.File.m_szBGPath + szFilename;
            m_pBackground = new xaeSprite(szFilename.c_str());
        }
        else
        if(szCommand == "��������" || szCommand == "BGM")
        {
            string szFilename = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(m_szScript[pos] != '\n' && m_szScript[pos] != '\r')
            {
                szFilename += m_szScript[pos];
                pos++;
            }

            /** ���뱳������ */
            szFilename = g_Setting.File.m_szBGMPath + szFilename;
            g_BGMPlayer.setBGM(szFilename);
        }
        else
        if(szCommand == "����" || szCommand == "MAID")
        {
            string szName = "";
            string szFilename = "";
            string szX = "", szY = "";
            int x, y;
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szName += m_szScript[pos];
                pos++;
            }

            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szFilename += m_szScript[pos];
                pos++;
            }
            szFilename = g_Setting.File.m_szMaidDrawPath + szFilename;

            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szX += m_szScript[pos];
                pos++;
            }
            x = atoi(szX.c_str());

            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szY += m_szScript[pos];
                pos++;
            }
            y = atoi(szY.c_str());

            xaeMadeDraw tagMD;
            tagMD.m_pScript = new xaeSprite(szFilename.c_str());
            tagMD.x = x;
            tagMD.y = y;

            m_mpMadeDraw[szName] = tagMD;
        }
        else
        if(szCommand == "����" || szCommand == "NAME")
        {
            string szTalker = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(m_szScript[pos] != '\n' && m_szScript[pos] != '\r')
            {
                szTalker += m_szScript[pos];
                pos++;
            }

            m_Textbox.setTalker(szTalker);
        }
        else
        if(szCommand == "�ı�" || szCommand == "TEXT")
        {
            string szText = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(m_szScript[pos] != '\n' && m_szScript[pos] != '\r')
            {
                szText += m_szScript[pos];
                pos++;
            }

            m_Textbox.addText(szText);
        }
        else
        if(szCommand == "����" || szCommand == "VOICE")
        {
            string filename = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(m_szScript[pos] != '\n' && m_szScript[pos] != '\r')
            {
                filename += m_szScript[pos];
                pos++;
            }
            filename = g_Setting.File.m_szVoicePath + filename;

            m_Textbox.setVoice(filename);
        }
        else
        if(szCommand == "����ı�" || szCommand == "CLEAR-TEXT")
        {
            m_Textbox.clearText();
        }
        else
        if(szCommand == "�������" || szCommand == "CLEAR-NAME")
        {
            m_Textbox.setTalker("");
        }
        else
        if(szCommand == "�����������" || szCommand == "CLEAR-BGM")
        {
            g_BGMPlayer.stopBGM();
        }
        else
        if(szCommand == "�������" || szCommand == "CLEAR-MAID")
        {
            string szName = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szName += m_szScript[pos];
                pos++;
            }

            /** Ѱ�����沢��� */
            map<string, xaeMadeDraw>::iterator it = m_mpMadeDraw.find(szName);
            if(it != m_mpMadeDraw.end())
            {
                if(it->second.m_pScript != NULL)
                {
                    it->second.m_pScript->release();
                    delete it->second.m_pScript;
                }
                m_mpMadeDraw.erase(it);
            }
        }
        else
        if(szCommand == "ש��" || szCommand == "BLOCK")
        {
            string szName = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szName += m_szScript[pos];
                pos++;
            }

            setScriptCursor(pos);

            /** ���뷽�鳡������ʾ */
            xaeBlockScene* scene = (xaeBlockScene*)(xaeSceneMgr::Instance().create_scene("Block", "Block"));
            if(NULL != scene)
            {
                xaeSceneMgr::Instance().add_scene("Block", scene);
            }
            scene->loadLevel(szName);
            xaeSceneMgr::Instance().set_current_scene("Block", false);

            /** ���뷽�鳡������ʾ */
            //xaeLoadingScene* loadingScene = (xaeLoadingScene*)(xaeSceneMgr::Instance().get_scene("Loading"));
            //loadingScene->loadingSceneAndShow(createBlockThread, new string(szName));
            //xaeSceneMgr::Instance().set_current_scene("Loading", false);

            return true;
        }
        else
        if(szCommand == "��Ƶ" || szCommand  == "VIDEO")
        {
            string szFilename = "";
            string szX = "", szY = "";
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szFilename += m_szScript[pos];
                pos++;
            }

            int x, y, w, h;
            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szX += m_szScript[pos];
                pos++;
            }
            x = atoi(szX.c_str());

            while(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r') pos++;
            while(!(m_szScript[pos] == ' ' || m_szScript[pos] == '\t' || m_szScript[pos] == '\n' || m_szScript[pos] == '\r'))
            {
                szY += m_szScript[pos];
                pos++;
            }
            y = atoi(szY.c_str());

            w = x, h = y;
            x = (GAME_WIDTH - w) / 2;
            y = (GAME_HEIGHT - h) / 2;

            /** ������Ƶ */
            xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
            xaeResourceBody tagRB;
            bool bSucc = pRMO->get_resource_bin(g_Setting.File.m_szVideoPath + szFilename, tagRB);

            /** ��ȡʧ�� */
            if(!bSucc) continue;

            /** д����ʱĿ¼ */
            xae::Instance().create_temp_folder(g_Setting.File.m_szTempPath);
            FILE* fp = fopen(string(g_Setting.File.m_szTempPath + "/" + szFilename).c_str(), "wb+");
            if(!fp)
            {
                continue;
            }
            fwrite(tagRB.m_pRes, tagRB.m_nSize, 1, fp);
            fclose(fp);

            /** ����������ָ�� */
            m_pVideo = new CXadillaXVideoPlayer();
            RECT playrect;
            playrect.left = x;
            playrect.top = y;
            playrect.right = x + w;
            playrect.bottom = y + h;
            m_pVideo->LoadFile(
                string(g_Setting.File.m_szTempPath + "/" + szFilename).c_str(),
                playrect,
                g_hWnd,
                true
                );
            m_pVideo->Play();

            xaeGUIObject::set_mouse(IDC_ARROW);
            break;
        }
        else
        if(szCommand == "����" || szCommand == "END")
        {
            xaeWelcomeScene* scene = (xaeWelcomeScene*)(xaeSceneMgr::Instance().create_scene("Welcome", "Welcome"));
            if(NULL != scene)
            {
                xaeSceneMgr::Instance().add_scene("Welcome", scene);
            }
            xaeSceneMgr::Instance().set_current_scene("Welcome");
        }
    } while (szCommand != "��ͣ" && szCommand != "PAUSE");

    setScriptCursor(pos);

    m_emStatus = XADS_PAUSED;

    return true;
}

bool xaeAVGScene::parseNextPause()
{
    return parsePause(m_dwScriptCursor);
}

void xaeAVGScene::setRevived(bool bRevived)
{
    m_bRevived = bRevived;
}

bool xaeAVGScene::getRevived()
{
    return m_bRevived;
}
