#ifndef XAEBLOCKSCENE_H
#define XAEBLOCKSCENE_H

#pragma once
#include <hgesprite.h>
#include <xaeSceneObject.h>
#include "xaeBlockTreasure.h"
#include <hge/hgefont.h>

struct coor
{
    float x, y;
    coor() { x = y = 0.0f; }
    coor(float _x, float _y) { x = _x, y = _y; }
};

enum XAE_BLOCK_PLAY_STATUS
{
    XBPS_READY,
    XBPS_PLAYING,
    XBPS_WIN,
    XBPS_LOSE,

    XBPS_PAUSE
};

class xaeBlockScene : public xaeSceneObject
{
public:
    xaeBlockScene(void);
    virtual ~xaeBlockScene(void);

    /** 载入关卡 */
    bool                        loadLevel(string levelName);

    /** 逻辑函数 */
    virtual bool                update(float fDT);
    /** 渲染函数 */
    virtual bool                render();

private:
    /** 判断纹理某个分块是否全透明 */
    bool                        blockTransparent(HTEXTURE hTex, int x, int y, int w, int h);

    /** 纹理的某个分块与当下小球位置的碰撞检测 */
    coor                        collisionCheck(HTEXTURE hTex, int x, int y, int w, int h);
    /** 小球与分块有碰撞，重新设置速度向量 */
    void                        setRebound(coor c);
    
    /** 载入资源 */
    void                        __loadResource();

private:
    string                      m_szLevelName;              ///< 关卡名字

    HTEXTURE                    m_hTexNake;                 ///< 裸体纹理
    HTEXTURE                    m_hTexClothHot;             ///< 衣服热点图片纹理

    xaeSprite*                  m_pNake;                    ///< 裸体精灵
    xaeSprite*                  m_pCloth;                   ///< 衣服精灵

    DWORD                       m_dwFlapNum;                ///< 挡板方块数量
    float                       m_fFlapX;                   ///< 挡板x轴
    float                       m_fFlapY;                   ///< 挡板y轴
    xaeSprite*                  m_pFlap;                    ///< 挡板精灵

    DWORD                       m_dwBlockLeft;              ///< 剩余的砖块
    bool                        m_bClothes[600][600];       ///< 各砖块的存活状态

    float                       m_fBallSpeedX;              ///< 球速度x矢量
    float                       m_fBallSpeedY;              ///< 球速度y矢量
    float                       m_fBallX;                   ///< 小球x轴
    float                       m_fBallY;                   ///< 小球y轴
    xaeSprite*                  m_pBall;                    ///< 小球精灵
    xaeSprite*                  m_pLoseBG;                  ///< 输了的黑屏精灵

    XAE_BLOCK_PLAY_STATUS       m_emStatus;                 ///< 游戏状态

    xaeBlockTreasure            m_Treasure;                 ///< 道具

    xaeGUIObject*               m_pGUI;                     ///< GUI
    hgeFont*                    m_pScoreFont;               ///< 分数字体
    xaeFont*                    m_pLevelFont;               ///< 关卡字体
};

#endif
