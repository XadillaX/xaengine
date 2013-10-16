#ifndef XAEBLOCKTREASURE_H
#define XAEBLOCKTREASURE_H

#pragma once
#include "global.h"
#include <ixaeobject.h>
#include <hgevector.h>
#include <list>
using namespace std;

#define XAE_TREASURE_TYPE_NUM   5
enum XAE_TREASURE_TYPE
{
    XTT_SPEED_UP,
    XTT_SPEED_DOWN,
    XTT_FLAP_WIDEN,
    XTT_FLAP_NARROW,
    XTT_HITS
};

/** ��������ĵ��� */
struct xaeDroppingTreasure
{
    XAE_TREASURE_TYPE           emType;     ///< ����
    hgeVector                   tagCoor;    ///< ����

    float                       fSpeed;     ///< �����ٶ�
};

/** ӵ�еĵ��� */
struct xaeTreasureOwn
{
    XAE_TREASURE_TYPE           emType;     ///< ����

    float                       fTimeLeft;  ///< ʣ��ʱ��
};

class xaeBlockTreasure : public iXAEObject
{
public:
    xaeBlockTreasure(void);
    virtual ~xaeBlockTreasure(void);

    void                        generateTreasure(float x, float y);

    void                        update(float fDT, float fFlapX, float fFlapY);
    void                        render();

    DWORD                       getFlapNum();
    float                       getSpeedScale();
    bool                        getHits();

private:
    void                        __loadResource();

private:
    vector<xaeDroppingTreasure> m_lstDropping;
    vector<xaeTreasureOwn>      m_lstOwn;

    xaeSprite*                  m_pTreasureSprite[10];

    DWORD                       m_dwFlapNum;
    float                       m_fSpeedScale;
    bool                        m_bHits;
};

#endif
