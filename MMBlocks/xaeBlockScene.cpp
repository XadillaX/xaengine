#include "xaeBlockScene.h"
#include "xaeAVGScene.h"
#include "xaeWelcomeScene.h"
#include <cmath>
#include <gui/xaeGUIStaticImage.h>

DWORD WINAPI createBlockThread(LPVOID lpParam)
{
    loadingThreadPara* para = (loadingThreadPara*)lpParam;
    xaeBlockScene* scene;
    para->m_szSceneName = para->m_szSceneType = "Block";
    scene = (xaeBlockScene*)(xaeSceneMgr::Instance().create_scene(para->m_szSceneName, para->m_szSceneType));

    scene->loadLevel(*((string*)para->m_pOtherPara));

    LockScope oScope(para->m_CriticalSection);
    para->m_pScene = scene;
    para->m_bCreated = true;

    delete para->m_pOtherPara;

    return 0;
}

xaeBlockScene::xaeBlockScene(void) :
    m_szLevelName(""),

    m_hTexNake(0),
    m_hTexClothHot(0),

    m_pNake(NULL),
    m_pCloth(NULL),
    m_pFlap(NULL),
    m_dwFlapNum(4),

    m_dwBlockLeft(0),

    m_fFlapX(300.0f - 2 * g_Setting.Block.m_nFlapLength),
    m_fFlapY(g_Setting.Block.m_fFlapY),

    m_fBallSpeedX(0.0f),
    m_fBallSpeedY(-g_Setting.Block.m_fBallSpeed),

    m_fBallX(300.0f),
    m_fBallY(g_Setting.Block.m_fFlapY - g_Setting.Block.m_nBallLength / 2),
    m_pBall(NULL),

    m_emStatus(XBPS_READY),

    m_pLoseBG(NULL),
    m_pGUI(NULL),
    m_pScoreFont(NULL),
    m_pLevelFont(NULL)
{
    memset(m_bClothes, 0, sizeof(m_bClothes));

#ifdef __DEBUG
    SAFEDEL(g_pDebugFont);
    g_pDebugFont = new xaeFont("宋体", 12, 0, 0, 0);
    g_pDebugFont->SetColor(0xff000000);
#endif

    __loadResource();
}

xaeBlockScene::~xaeBlockScene(void)
{
    SAFEDEL(m_pNake);
    SAFEDEL(m_pCloth);
    SAFEDEL(m_pBall);
    SAFEDEL(m_pFlap);
    SAFEDEL(m_pLoseBG);

    SAFEDEL(m_pGUI);
    SAFEDEL(m_pScoreFont);
    SAFEDEL(m_pLevelFont);

#ifdef __DEBUG
    SAFEDEL(g_pDebugFont);
#endif

    //if(m_hTexClothHot != 0) m_pHGE->Texture_Free(m_hTexClothHot);
}

void xaeBlockScene::__loadResource()
{
    m_pFlap = xaeSpriteLoader::Instance().get_sprite("Block", "flap");
    m_pBall = xaeSpriteLoader::Instance().get_sprite("Block", "ball");
    m_pLoseBG = xaeSpriteLoader::Instance().get_sprite("Block", "loseBG");
    m_pBall->get_sprite()->SetHotSpot(g_Setting.Block.m_nBallLength / 2, g_Setting.Block.m_nBallLength / 2);

    m_pGUI = new xaeGUIObject();
    m_pGUI->add_control(new xaeGUIStaticImage(0, "Right", "Block*rightPanel", 500, 0));

    m_pScoreFont = new hgeFont("Font/scoreNum.fnt");
    m_pLevelFont = new xaeFont(g_szDefaultFont.c_str(), 20);
    m_pLevelFont->SetColor(0xffffffff);
}

#include "xaePackage.h"
bool xaeBlockScene::loadLevel(string levelName)
{
    /** 删除原图片 */
    SAFEDEL(m_pNake);
    SAFEDEL(m_pCloth);

    m_hTexClothHot = m_hTexNake = 0;

    /** 载入关卡图片 */
    string nakeFilename = g_Setting.File.m_szLevelPath + levelName + ".nake.png";
    string clothFilename = g_Setting.File.m_szLevelPath + levelName + ".cloth.png";
    string clothHotFilename = g_Setting.File.m_szLevelPath + levelName + ".cloth.hot.png";
    
    m_pNake = new xaeSprite(nakeFilename.c_str());
    m_pCloth = new xaeSprite(clothFilename.c_str());

    /** 载入失败 */
    if(m_pNake->get_sprite() == NULL || m_pCloth->get_sprite() == NULL)
    {
        SAFEDEL(m_pNake);
        SAFEDEL(m_pCloth);
        return false;
    }

    /** 大小不对 */
    if(m_pNake->get_sprite()->GetWidth() != 600 ||
        m_pNake->get_sprite()->GetHeight() != 600 ||
        m_pCloth->get_sprite()->GetWidth() != 600 ||
        m_pCloth->get_sprite()->GetHeight() != 600)
    {
        SAFEDEL(m_pNake);
        SAFEDEL(m_pCloth);
        return false;
    }

    /** 热点图片 */
    xaeResourceBody tagRB;
    if(!xae::Instance().get_resource_mgr()->get_resource_bin(clothHotFilename.c_str(), tagRB))
    {
        SAFEDEL(m_pNake);
        SAFEDEL(m_pCloth);
        return false;
    }

    /** 都对了 */
    m_hTexNake = m_pNake->get_sprite()->GetTexture();
    m_hTexClothHot = m_pHGE->Texture_Load((char*)tagRB.m_pRes, tagRB.m_nSize);

    /** 开始切割 */
    m_dwBlockLeft = 0;
    for(int i = 0; i < g_Setting.Block.m_nBlockRowCount; i++)
    {
        for(int j = 0; j < g_Setting.Block.m_nBlockColCount; j++)
        {
            /** 看看分块是不是全透明的 */
            if(!blockTransparent(m_hTexClothHot,
                j * g_Setting.Block.m_nBlockWidth,
                i * g_Setting.Block.m_nBlockHeight,
                g_Setting.Block.m_nBlockWidth,
                g_Setting.Block.m_nBlockHeight))
            {
                m_bClothes[i][j] = true;
                m_dwBlockLeft++;
            }
            else m_bClothes[i][j] = false;
        }
    }

    /** 立绘 */
    if(m_pGUI->get_control("Right")->get_control("Maid") != NULL)
    {
        m_pGUI->del_control("Right");
    }
    m_pGUI->get_control("Right")->add_control(new xaeGUIStaticImage(0, "Maid", g_Setting.File.m_szLevelPath + levelName + ".maid.png", 100, 240));

    m_szLevelName = levelName;

    return true;
}

#include "xaeReviveScene.h"
bool xaeBlockScene::update(float fDT)
{
    g_BGMPlayer.update(fDT);

#ifdef __DEBUG
    if(xae::Instance().keyup(HGEK_ESCAPE))
    {
        g_bDebugStop ^= true;
    }

    if(g_bDebugStop) return false;

    /** 彩旦：直接赢了 */
    if(xae::Instance().keystate(HGEK_CTRL) && xae::Instance().keystate(HGEK_E))
    {
        xaeAVGScene* scene = (xaeAVGScene*)(xaeSceneMgr::Instance().get_scene("AVG"));

        /** 若死过了而且是PreBoss关 */
        if(scene->getRevived() && m_szLevelName == g_Setting.Block.m_szPreBoss)
        {
            xaeSceneMgr::Instance().del_scene("AVG");
            xaeSceneObject* sceneo = xaeSceneMgr::Instance().create_scene("Welcome", "Welcome");
            xaeSceneMgr::Instance().add_scene("Welcome", sceneo);
            xaeSceneMgr::Instance().set_current_scene("Welcome");

            return false;
        }

        /** 否则跳到AVG画面 */
        scene->parseNextPause();
        xaeSceneMgr::Instance().set_current_scene("AVG");

        /** Pre Boss干翻了 */
        if(m_szLevelName == g_Setting.Block.m_szPreBoss)
        {
            scene->setPreBossed();
        }

        return false;
    }
#endif


    /** 暂停 */
    if(xae::Instance().keyup(HGEK_ENTER))
    {
        if(m_emStatus == XBPS_PLAYING)
        {
            m_emStatus = XBPS_PAUSE;
            return false;
        }
        else
        if(m_emStatus == XBPS_PAUSE)
        {
            m_emStatus = XBPS_PLAYING;
        }
    }
    if(m_emStatus == XBPS_PAUSE) return false;

    /** GUI */
    float mousex, mousey;
    m_pHGE->Input_GetMousePos(&mousex, &mousey);
    m_pGUI->update(fDT, mousex, mousey);

    /** 挡板左右转 */
    float dx = fDT * g_Setting.Block.m_fFlapSpeed;
    if(xae::Instance().keystate(HGEK_RIGHT))
    {
        if(m_fFlapX + dx + g_Setting.Block.m_nFlapLength * m_dwFlapNum <= 600.0f)
        {
            m_fFlapX += dx;
            if(m_emStatus == XBPS_READY)
            {
                m_fBallX += dx;
            }
        }
        else
        {
            m_fFlapX = 600.0f - g_Setting.Block.m_nFlapLength * m_dwFlapNum;
            if(m_emStatus == XBPS_READY)
            {
                m_fBallX = 600.0f - (g_Setting.Block.m_nFlapLength * m_dwFlapNum) / 2.0f;
            }
        }
    }
    if(xae::Instance().keystate(HGEK_LEFT))
    {
        if(m_fFlapX - dx >= 0)
        {
            m_fFlapX -= dx;
            if(m_emStatus == XBPS_READY)
            {
                m_fBallX -= dx;
            }
        }
        else
        {
            m_fFlapX = 0.0f;
            if(m_emStatus == XBPS_READY)
            {
                m_fBallX = (g_Setting.Block.m_nFlapLength * m_dwFlapNum) / 2.0f;
            }
        }
    }

    /** 道具更新 */
    m_Treasure.update(fDT, m_fFlapX, m_fFlapY);
    /** 挡板长度 */
    float fFlapCenterX = m_fFlapX + (m_dwFlapNum * g_Setting.Block.m_nFlapLength) / 2.0f;
    m_dwFlapNum = m_Treasure.getFlapNum();
    m_fFlapX = fFlapCenterX - (m_dwFlapNum * g_Setting.Block.m_nFlapLength) / 2;
    if(m_fFlapX < 0) m_fFlapX = 0;
    else
    if(m_fFlapX + m_dwFlapNum * g_Setting.Block.m_nFlapLength > 600.0f) m_fFlapX = 600.0f - m_dwFlapNum * g_Setting.Block.m_nFlapLength;

    /** 空格或者回车 */
    if(xae::Instance().keyup(HGEK_SPACE) || xae::Instance().keyup(HGEK_ENTER))
    {
        /** 变成开始状态 */
        if(m_emStatus == XBPS_READY)
        {
            m_emStatus =XBPS_PLAYING;
        }
        else
        /** 赢了按回车回到AVG画面 */
        if(m_emStatus == XBPS_WIN)
        {
            xaeAVGScene* scene = (xaeAVGScene*)(xaeSceneMgr::Instance().get_scene("AVG"));

            /** 若死过了而且是PreBoss关 */
            if(scene->getRevived() && m_szLevelName == g_Setting.Block.m_szPreBoss)
            {
                xaeSceneMgr::Instance().del_scene("AVG");
                xaeSceneObject* sceneo = xaeSceneMgr::Instance().create_scene("Welcome", "Welcome");
                xaeSceneMgr::Instance().add_scene("Welcome", sceneo);
                xaeSceneMgr::Instance().set_current_scene("Welcome");

                return false;
            }

            /** 否则跳到AVG画面 */
            scene->parseNextPause();
            xaeSceneMgr::Instance().set_current_scene("AVG");

            /** Pre Boss干翻了 */
            if(m_szLevelName == g_Setting.Block.m_szPreBoss)
            {
                scene->setPreBossed();
            }

            return false;
        }
    }

    /** 游戏在玩的状态 */
    if(m_emStatus == XBPS_PLAYING)
    {
        /** 球飞啦！ */
        float fbdx = m_fBallSpeedX * fDT, fbdy = m_fBallSpeedY * fDT;
        fbdx *= m_Treasure.getSpeedScale();
        fbdy *= m_Treasure.getSpeedScale();

        /** 横向飞 */
        if(m_fBallX + fbdx - g_Setting.Block.m_nBallLength / 2 >= 0.0f && m_fBallX + fbdx + g_Setting.Block.m_nBallLength / 2 <= 600.0f)
        {
            m_fBallX += fbdx;
        }
        else
        if(m_fBallX + fbdx - g_Setting.Block.m_nBallLength / 2 < 0.0f) m_fBallX = g_Setting.Block.m_nBallLength / 2, m_fBallSpeedX = -m_fBallSpeedX;
        else m_fBallX = 600 - g_Setting.Block.m_nBallLength / 2, m_fBallSpeedX = -m_fBallSpeedX;

        /** 纵向飞 */
        if(m_fBallY + fbdy - g_Setting.Block.m_nBallLength / 2 >= 0.0f)
        {
            m_fBallY += fbdy;
        }
        else
        if(m_fBallY + fbdy - g_Setting.Block.m_nBallLength / 2 < 0.0f) m_fBallY = g_Setting.Block.m_nBallLength / 2, m_fBallSpeedY = - m_fBallSpeedY;
        //else m_fBallY = 590.0f, m_fBallSpeedY = - m_fBallSpeedY;

        /** 碰到挡板：往上弹 */
        if(m_fBallY >= g_Setting.Block.m_fFlapY - g_Setting.Block.m_nBallLength / 2 && m_fBallY <= g_Setting.Block.m_fFlapY + 15.0f - g_Setting.Block.m_nBallLength / 2)
        {
            /** x轴在挡板范围内 */
            if(m_fBallX >= m_fFlapX && m_fBallX <= m_fFlapX + m_dwFlapNum * g_Setting.Block.m_nFlapLength)
            {
                /** 根据小球在挡板的位置确定x速度 */
                m_fBallSpeedX = (m_fBallX - (m_fFlapX + m_dwFlapNum * g_Setting.Block.m_nFlapLength / 2)) / (m_dwFlapNum * (g_Setting.Block.m_nFlapLength) / 2);
                m_fBallSpeedX *= g_Setting.Block.m_fMaxHorizontalRate;
                m_fBallSpeedX *= g_Setting.Block.m_fBallSpeed;

                /** 确定y轴速度：保证x和y的合速度为g_Setting.Block.m_fBallSpeed */
                m_fBallSpeedY = -sqrt(g_Setting.Block.m_fBallSpeed * g_Setting.Block.m_fBallSpeed - m_fBallSpeedX * m_fBallSpeedX);
            }
        }

        /** 飞丢了 */
        if(m_fBallY >= 600.0f)
        {
            /** 如果已经干翻了Pre Boss，则直接到welcome */
            xaeAVGScene* scenea = (xaeAVGScene*)xaeSceneMgr::Instance().get_scene("AVG");
            if(scenea->getPreBossed())
            {
                xaeSceneObject* sceneo = xaeSceneMgr::Instance().create_scene("Welcome", "Welcome");
                xaeSceneMgr::Instance().add_scene("Welcome", sceneo);
                xaeSceneMgr::Instance().del_scene("AVG");

                xaeSceneMgr::Instance().set_current_scene("Welcome");
                return false;
            }

            xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("Revive", "Revive");
            if(NULL != scene)
            {
                xaeSceneMgr::Instance().add_scene("Revive", scene);
            }
            xaeSceneMgr::Instance().set_current_scene("Revive");

            ((xaeReviveScene*)scene)->setLevel(m_szLevelName);

            /** 设置AVG为死过了 */
            ((xaeAVGScene*)xaeSceneMgr::Instance().get_scene("AVG"))->setRevived();

            return false;
        }

        /** 得到小球区域四个角的坐标 */
        coor zs(m_fBallX, m_fBallY), zx(m_fBallX, m_fBallY), ys(m_fBallX, m_fBallY), yx(m_fBallX, m_fBallY);
        zs.x -= (g_Setting.Block.m_nBallLength / 2), zs.y -= (g_Setting.Block.m_nBallLength / 2);
        ys.x += (g_Setting.Block.m_nBallLength / 2), ys.y -= (g_Setting.Block.m_nBallLength / 2);
        zx.x -= (g_Setting.Block.m_nBallLength / 2), zx.y += (g_Setting.Block.m_nBallLength / 2);
        yx.x -= (g_Setting.Block.m_nBallLength / 2), yx.y -= (g_Setting.Block.m_nBallLength / 2);

        /** 求四个角所在的各方块的下标 */
        int zsi = ((int)zs.y) / g_Setting.Block.m_nBlockHeight, zsj = ((int)zs.x) / g_Setting.Block.m_nBlockWidth;
        int ysi = ((int)ys.y) / g_Setting.Block.m_nBlockHeight, ysj = ((int)ys.x) / g_Setting.Block.m_nBlockWidth;
        int zxi = ((int)zx.y) / g_Setting.Block.m_nBlockHeight, zxj = ((int)zx.x) / g_Setting.Block.m_nBlockWidth;
        int yxi = ((int)yx.y) / g_Setting.Block.m_nBlockHeight, yxj = ((int)yx.x) / g_Setting.Block.m_nBlockWidth;

        /** 碰撞检测 */
        coor zspz = collisionCheck(m_hTexClothHot, zsj * g_Setting.Block.m_nBlockWidth, zsi * g_Setting.Block.m_nBlockHeight, g_Setting.Block.m_nBlockWidth, g_Setting.Block.m_nBlockHeight);
        coor zxpz = collisionCheck(m_hTexClothHot, zxj * g_Setting.Block.m_nBlockWidth, zxi * g_Setting.Block.m_nBlockHeight, g_Setting.Block.m_nBlockWidth, g_Setting.Block.m_nBlockHeight);
        coor yspz = collisionCheck(m_hTexClothHot, ysj * g_Setting.Block.m_nBlockWidth, ysi * g_Setting.Block.m_nBlockHeight, g_Setting.Block.m_nBlockWidth, g_Setting.Block.m_nBlockHeight);
        coor yxpz = collisionCheck(m_hTexClothHot, yxj * g_Setting.Block.m_nBlockWidth, yxi * g_Setting.Block.m_nBlockHeight, g_Setting.Block.m_nBlockWidth, g_Setting.Block.m_nBlockHeight);

        /** 处理碰撞 */
        if(zspz.x >= 0.0f && zspz.y >= 0.0f)
        {
            /** 这个方块消失 */
            m_bClothes[zsi][zsj] = false;
            
            /** 方块数减少 */
            m_dwBlockLeft--;

            m_Treasure.generateTreasure(zsj * g_Setting.Block.m_nBlockWidth + g_Setting.Block.m_nBlockWidth / 2,
                zsi * g_Setting.Block.m_nBlockHeight + g_Setting.Block.m_nBlockHeight / 2);

            /** 下同 */
        }

        if(zxpz.x >= 0.0f && zxpz.y >= 0.0f && (zsi != zxi || zsj != zxj))
        {
            m_bClothes[zxi][zxj] = false;
            m_dwBlockLeft--;
            m_Treasure.generateTreasure(zxj * g_Setting.Block.m_nBlockWidth + g_Setting.Block.m_nBlockWidth / 2,
                zxi * g_Setting.Block.m_nBlockHeight + g_Setting.Block.m_nBlockHeight / 2);
        }

        if(yspz.x >= 0.0f && yspz.y >= 0.0f && (zxi != ysi || zxj != ysj) && (zsi != ysi || zsj != ysj))
        {
            m_bClothes[ysi][ysj] = false;
            m_dwBlockLeft--;
            m_Treasure.generateTreasure(ysj * g_Setting.Block.m_nBlockWidth + g_Setting.Block.m_nBlockWidth / 2,
                ysi * g_Setting.Block.m_nBlockHeight + g_Setting.Block.m_nBlockHeight / 2);
        }

        if(yxpz.x >= 0.0f && yxpz.y >= 0.0f && (zxi != yxi || zxj != yxj) && (ysi != yxi || ysj != yxj) && (zsi != yxi || zsj != yxj))
        {
            m_bClothes[yxi][yxj] = false;
            m_dwBlockLeft--;
            m_Treasure.generateTreasure(yxj * g_Setting.Block.m_nBlockWidth + g_Setting.Block.m_nBlockWidth / 2,
                yxi * g_Setting.Block.m_nBlockHeight + g_Setting.Block.m_nBlockHeight / 2);
        }

        /** 重新设置速度 */
        if(!m_Treasure.getHits())
        {
            if(zspz.x >= 0.0f && zspz.y >= 0.0f) setRebound(zspz);
            else
            if(yspz.x >= 0.0f && yspz.y >= 0.0f) setRebound(yspz);
            else
            if(zxpz.x >= 0.0f && zxpz.y >= 0.0f) setRebound(zxpz);
            else
            if(yxpz.x >= 0.0f && yxpz.y >= 0.0f) setRebound(yxpz);
        }

        /** 所有方块打完了就赢了 */
        if(m_dwBlockLeft == 0) m_emStatus = XBPS_WIN;
    }
    
    return false;
}

bool xaeBlockScene::render()
{
    if(m_pNake != NULL) m_pNake->render();
    for(int i = 0; i < g_Setting.Block.m_nBlockRowCount; i++)
    {
        for(int j = 0; j < g_Setting.Block.m_nBlockColCount; j++)
        {
            if(m_bClothes[i][j])
            {
                m_pCloth->get_sprite()->SetTextureRect(j * g_Setting.Block.m_nBlockWidth, i * g_Setting.Block.m_nBlockHeight, g_Setting.Block.m_nBlockWidth, g_Setting.Block.m_nBlockHeight);
                m_pCloth->render(j * g_Setting.Block.m_nBlockWidth, i * g_Setting.Block.m_nBlockHeight);
            }
        }
    }

    m_Treasure.render();
    for(size_t i = 0; i < m_dwFlapNum; i++)
    {
        m_pFlap->render(m_fFlapX + i * g_Setting.Block.m_nFlapLength, m_fFlapY);
    }
#ifdef __DEBUG
    if(g_bDebugStop)
    {
        g_pDebugFont->Print(m_fFlapX, m_fFlapY, "(%d, %d)", (int)m_fFlapX, (int)m_fFlapY);
    }
#endif
    //m_pBall->render(m_fBallX, m_fBallY);
    m_pBall->render_stretch(m_fBallX - g_Setting.Block.m_nBallLength / 2, m_fBallY - g_Setting.Block.m_nBallLength / 2, m_fBallX + g_Setting.Block.m_nBallLength / 2, m_fBallY + g_Setting.Block.m_nBallLength / 2);
#ifdef __DEBUG
    if(g_bDebugStop)
    {
        g_pDebugFont->Print(m_fBallX, m_fBallY, "(%d, %d)", (int)m_fBallX, (int)m_fBallY);
    }
#endif

    if(m_emStatus == XBPS_LOSE)
    {
        m_pLoseBG->render();
    }

    m_pGUI->render(0, 0);

    /** 剩余砖块 */
    char szScore[16];
    sprintf(szScore, "%d", m_dwBlockLeft);
    m_pScoreFont->printf(700, 80, HGETEXT_CENTER, szScore);

    /** 关卡 */
    char szLevel[128];
    sprintf(szLevel, "Level %s", m_szLevelName.c_str());
    SIZE levelFontSize = m_pLevelFont->GetTextSize(C2W(szLevel));
    m_pLevelFont->Print(600 + (200 - levelFontSize.cx) / 2, 155, szLevel);

    xaeGUIObject::render_mouse();

    return false;
}

bool xaeBlockScene::blockTransparent(HTEXTURE hTex, int x, int y, int w, int h)
{
    int nTexWidth = m_pHGE->Texture_GetWidth(hTex);
    DWORD* pPix = m_pHGE->Texture_Lock(hTex);
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            /** 某一个点不透明 */
            if(pPix[(y + i) * nTexWidth + x + j] != 0xffff00ff)
            {
                m_pHGE->Texture_Unlock(hTex);
                return false;
            }
        }
    }

    m_pHGE->Texture_Unlock(hTex);
    return true;
}

#define DISTANCE(x1, y1, x2, y2) (sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2)))

coor xaeBlockScene::collisionCheck(HTEXTURE hTex, int x, int y, int w, int h)
{
    if(!m_bClothes[y / h][x / w]) return coor(-1, -1);

    int nTexWidth = m_pHGE->Texture_GetWidth(hTex);
    DWORD* pPix = m_pHGE->Texture_Lock(hTex);
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            /** 某一个点不透明 */
            if((y + i) * nTexWidth + x + j >= nTexWidth * 600) break;
            if(pPix[(y + i) * nTexWidth + x + j] != 0xffff00ff)
            {
                /** 在圆的半径里 */
                if(DISTANCE(x + j, y + i, m_fBallX, m_fBallY) < (g_Setting.Block.m_nBallLength / 2))
                {
                    m_pHGE->Texture_Unlock(hTex);
                    return coor(x + j, y + i);
                }
            }
        }
    }

    m_pHGE->Texture_Unlock(hTex);
    return coor(-1, -1);
}

#include "hgevector.h"

void xaeBlockScene::setRebound(coor c)
{
    /** 法线向量 */
    coor cNormal = c;
    cNormal.x -= m_fBallX;
    cNormal.y -= m_fBallY;

    /** 速度向量 */
    coor cSpeed(m_fBallSpeedX, m_fBallSpeedY);

    switch(g_nReboundAlgorithm)
    {
    case 1:
        {
            /** 许波同学的算法 */
            float a = cNormal.y / cNormal.x;
            float m = cSpeed.x;
            float n = cSpeed.y;

            float x = ((1 - a * a) * m + 2 * a * n) / (a * a + 1);
            float y = ((a * a - 1) * n + 2 * a * m) / (a * a + 1);

            m_fBallSpeedX = x;
            m_fBallSpeedY = y;
            break;
        }

    case 2:
        {
            /** 宋莹莹的算法 */
            float x1 = cSpeed.x, y1 = cSpeed.y;
            float x2 = cNormal.x, y2 = cNormal.y;

            float x3 = 2 * (x2 * (y1 * y2 + x1 * x2) / (x2 * x2 + y2 * y2)) - x1;
            float y3 = 2 * (y2 * (y1 * y2 + x1 * x2) / (x2 * x2 + y2 * y2)) - y1;

            m_fBallSpeedX = x3;
            m_fBallSpeedY = y3;
            break;
        }

    case 3:
        {
            /** 垃圾的算法 */
            m_fBallSpeedX = -m_fBallSpeedX;
            m_fBallSpeedY = -m_fBallSpeedY;

            m_fBallSpeedX += ((rand() % 10) - 5);
            m_fBallSpeedY += ((rand() % 10) - 5);
            break;
        }

    case 100:
        {
            /** 重写的算法 */
            int x = c.x - m_fBallX;
            int y = c.y - m_fBallY;
            
            /** 上下方碰撞 */
            if(abs(y) >= abs(x))
            {
                m_fBallSpeedY = -m_fBallSpeedY;
            }
            else
            /** 左右方碰撞 */
            {
                m_fBallSpeedX = -m_fBallSpeedX;
            }

            break;
        }

    case 4:
    default:
        {
            /** 陈洁操的算法 */
            hgeVector normalVector(cNormal.x, cNormal.y);
            hgeVector speedVector(cSpeed.x, cSpeed.y);

            float sink = ((normalVector.x * speedVector.y) + (normalVector.y * speedVector.x)) / (normalVector.Length() * speedVector.Length());
            float cosk = normalVector.Dot(&speedVector) / (normalVector.Length() * speedVector.Length());

            float newx = normalVector.x * cosk + normalVector.y * sink;
            float newy = normalVector.y * sink - normalVector.x * sink;

            hgeVector newSpeedVector(newx, newy);
            newSpeedVector = (newSpeedVector / newSpeedVector.Length()) * speedVector.Length();

            m_fBallSpeedX = newSpeedVector.x;
            m_fBallSpeedY = newSpeedVector.y;
            break;
        }
    }
}
