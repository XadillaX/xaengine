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

    /** ����ؿ� */
    bool                        loadLevel(string levelName);

    /** �߼����� */
    virtual bool                update(float fDT);
    /** ��Ⱦ���� */
    virtual bool                render();

private:
    /** �ж�����ĳ���ֿ��Ƿ�ȫ͸�� */
    bool                        blockTransparent(HTEXTURE hTex, int x, int y, int w, int h);

    /** �����ĳ���ֿ��뵱��С��λ�õ���ײ��� */
    coor                        collisionCheck(HTEXTURE hTex, int x, int y, int w, int h);
    /** С����ֿ�����ײ�����������ٶ����� */
    void                        setRebound(coor c);
    
    /** ������Դ */
    void                        __loadResource();

private:
    string                      m_szLevelName;              ///< �ؿ�����

    HTEXTURE                    m_hTexNake;                 ///< ��������
    HTEXTURE                    m_hTexClothHot;             ///< �·��ȵ�ͼƬ����

    xaeSprite*                  m_pNake;                    ///< ���徫��
    xaeSprite*                  m_pCloth;                   ///< �·�����

    DWORD                       m_dwFlapNum;                ///< ���巽������
    float                       m_fFlapX;                   ///< ����x��
    float                       m_fFlapY;                   ///< ����y��
    xaeSprite*                  m_pFlap;                    ///< ���徫��

    DWORD                       m_dwBlockLeft;              ///< ʣ���ש��
    bool                        m_bClothes[600][600];       ///< ��ש��Ĵ��״̬

    float                       m_fBallSpeedX;              ///< ���ٶ�xʸ��
    float                       m_fBallSpeedY;              ///< ���ٶ�yʸ��
    float                       m_fBallX;                   ///< С��x��
    float                       m_fBallY;                   ///< С��y��
    xaeSprite*                  m_pBall;                    ///< С����
    xaeSprite*                  m_pLoseBG;                  ///< ���˵ĺ�������

    XAE_BLOCK_PLAY_STATUS       m_emStatus;                 ///< ��Ϸ״̬

    xaeBlockTreasure            m_Treasure;                 ///< ����

    xaeGUIObject*               m_pGUI;                     ///< GUI
    hgeFont*                    m_pScoreFont;               ///< ��������
    xaeFont*                    m_pLevelFont;               ///< �ؿ�����
};

#endif
