#ifndef XAEAVGSCENE_H
#define XAEAVGSCENE_H

#pragma once
#include "global.h"
#include <xaeSprite.h>
#include <xaesceneobject.h>
#include <string>
#include <queue>
#include <map>
#include "xaeBGMPlayer.h"
#include "xaeAVGTextbox.h"
#include "CXadillaXVideoPlayer.h"
using namespace std;

struct xaeAVGTextBlock
{
    string                      m_szText;
    string                      m_szVoice;
};

struct xaeMadeDraw
{
    int                         x;
    int                         y;
    xaeSprite*                  m_pScript;
};

class xaeAVGScene : public xaeSceneObject
{
public:
    xaeAVGScene(void);
    virtual ~xaeAVGScene(void);

    /** 设置、获取脚本游标位置 */
    void                        setScriptCursor(DWORD scriptCursor);
    DWORD                       getScriptCursor();

    virtual bool                render();
    virtual bool                update(float fDT);

    bool                        parsePause(DWORD scriptCursor);
    bool                        parseNextPause();

    /** 设置复活 */
    void                        setRevived(bool bRevived = true);
    bool                        getRevived();
    void                        setPreBossed(bool bBossed = true) { m_bPreBossed = bBossed; }
    bool                        getPreBossed() { return m_bPreBossed; }

private:
    void                        __loadSript();
    void                        __loadResource();

private:
    string                      m_szScript;
    DWORD                       m_dwScriptCursor;

    XAE_AVG_DISPLAY_STATUS      m_emStatus;

    xaeAVGTextBlock             m_tagCurrentTextBlock;

    /** 图片 */
    xaeSprite*                  m_pBackground;
    map<string, xaeMadeDraw>    m_mpMadeDraw;

    /** 对话框 */
    xaeAVGTextbox               m_Textbox;

    /** 是否复活 */
    bool                        m_bRevived;

    /** 是否玩到了PreBoss之后 */
    bool                        m_bPreBossed;

    /** 视频播放 */
    CXadillaXVideoPlayer*       m_pVideo;
};

#endif
