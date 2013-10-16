#include "xaeBlockTreasure.h"

xaeBlockTreasure::xaeBlockTreasure(void) :
    m_dwFlapNum(4),
    m_fSpeedScale(1.0f),
    m_bHits(false)
{
    m_pHGE = hgeCreate(HGE_VERSION);

    memset(m_pTreasureSprite, 0, sizeof(m_pTreasureSprite));

    __loadResource();
}

xaeBlockTreasure::~xaeBlockTreasure(void)
{
    for(int i = 0; i < XAE_TREASURE_TYPE_NUM; i++)
    {
        SAFEDEL(m_pTreasureSprite[i]);
    }

    m_pHGE->Release();
}

void xaeBlockTreasure::__loadResource()
{
    for(int i = 0; i < XAE_TREASURE_TYPE_NUM; i++)
    {
        char name[20];
        sprintf(name, "treasure%d", i);
        m_pTreasureSprite[i] = xaeSpriteLoader::Instance().get_sprite("Block", name);
        
        float fHeight, fWidth;
        fHeight = m_pTreasureSprite[i]->get_sprite()->GetHeight();
        fWidth = m_pTreasureSprite[i]->get_sprite()->GetWidth();
        m_pTreasureSprite[i]->get_sprite()->SetHotSpot(fWidth / 2.0f, fHeight / 2.0f);
    }
}

void xaeBlockTreasure::generateTreasure(float x, float y)
{
    /** 几率 */
    int nProb = m_pHGE->Random_Int(0, 99);

    /** 不爆 */
    if(nProb >= g_Setting.Block.m_nTreasureProb) return;

    /** 物品几率 */
    int nTreasureProb = m_pHGE->Random_Int(0, 99);

    /** 各物品 */
    for(int i = 0; i < XAE_TREASURE_TYPE_NUM; i++)
    {
        if(nTreasureProb < (100 / XAE_TREASURE_TYPE_NUM) * (i + 1))
        {
            xaeDroppingTreasure tagDT;
            tagDT.emType = (XAE_TREASURE_TYPE)i;
            tagDT.fSpeed = m_pHGE->Random_Float(g_Setting.Block.m_fTreasureMinSpeed, g_Setting.Block.m_fTreasureMaxSpeed);
            tagDT.tagCoor.x = x;
            tagDT.tagCoor.y = y;

            m_lstDropping.push_back(tagDT);

            break;
        }
    }
}

void xaeBlockTreasure::render()
{
    /** 渲染掉落物品 */
    for(size_t i = 0; i < m_lstDropping.size(); i++)
    {
        m_pTreasureSprite[m_lstDropping[i].emType]->render(
            m_lstDropping[i].tagCoor.x,
            m_lstDropping[i].tagCoor.y
            );

#ifdef __DEBUG
        if(g_bDebugStop)
        {
            g_pDebugFont->Print(m_lstDropping[i].tagCoor.x, m_lstDropping[i].tagCoor.y, "(%d, %d)", (int)m_lstDropping[i].tagCoor.x, (int)m_lstDropping[i].tagCoor.y);
        }
#endif
    }
}

void xaeBlockTreasure::update(float fDT, float fFlapX, float fFlapY)
{
    /** 更新掉落物品 */
    for(size_t i = 0; i < m_lstDropping.size(); i++)
    {
        m_lstDropping[i].tagCoor.y += fDT * m_lstDropping[i].fSpeed;

        float nHeight = m_pTreasureSprite[m_lstDropping[i].emType]->get_sprite()->GetHeight();
        float nWidth = m_pTreasureSprite[m_lstDropping[i].emType]->get_sprite()->GetWidth();
        
        /** 碰到挡板 */
        if(m_lstDropping[i].tagCoor.y + nHeight / 2 >= fFlapY &&
            m_lstDropping[i].tagCoor.y + nHeight / 2 <= fFlapY + 25.0f &&
            m_lstDropping[i].tagCoor.x + nWidth / 2 >= fFlapX &&
            m_lstDropping[i].tagCoor.x - nWidth / 2 <= fFlapX + g_Setting.Block.m_nFlapLength * m_dwFlapNum)
        {
            xaeTreasureOwn tagTO;
            tagTO.emType = m_lstDropping[i].emType;
            tagTO.fTimeLeft = g_Setting.Block.m_fTreasureTimer;
            m_lstOwn.push_back(tagTO);

            /** 从掉落物品数组中移除 */
            m_lstDropping[i] = m_lstDropping[m_lstDropping.size() - 1];
            m_lstDropping.pop_back();
            i--;

            continue;
        }
        else
        /** 掉出去 */
        if(m_lstDropping[i].tagCoor.y - nHeight / 2 >= 600.0f)
        {
            /** 从掉落物品数组中移除 */
            m_lstDropping[i] = m_lstDropping[m_lstDropping.size() - 1];
            m_lstDropping.pop_back();
            i--;

            continue;
        }
    }

    /** 更新背包中的物品 */
    float fSpeedScale = 1.0f;
    int dwFlapNum = 4;
    bool bHit = false;
    for(size_t i = 0; i < m_lstOwn.size(); i++)
    {
        if(m_lstOwn[i].emType == XTT_HITS)
        {
            m_lstOwn[i].fTimeLeft -= fDT;
        }

        /** 时间到 */
        if(m_lstOwn[i].fTimeLeft < 0.0f)
        {
            m_lstOwn[i] = m_lstOwn[m_lstOwn.size() - 1];
            m_lstOwn.pop_back();
            i--;
            continue;
        }

        /** 更新状态 */
        switch(m_lstOwn[i].emType)
        {
        case XTT_SPEED_UP: fSpeedScale *= g_Setting.Block.m_fTreasureSpeedScale; break;
        case XTT_SPEED_DOWN: fSpeedScale /= g_Setting.Block.m_fTreasureSpeedScale; break;
        case XTT_FLAP_WIDEN: dwFlapNum += 2; break;
        case XTT_FLAP_NARROW: dwFlapNum -= 2; break;
        case XTT_HITS: bHit = true; break;
        }
    }
    if(dwFlapNum < 2) dwFlapNum = 2;
    else
    if(dwFlapNum > 30) dwFlapNum = 30;

    m_fSpeedScale = fSpeedScale;
    m_dwFlapNum = dwFlapNum;
    m_bHits = bHit;
}

DWORD xaeBlockTreasure::getFlapNum()
{
    return m_dwFlapNum;
}

float xaeBlockTreasure::getSpeedScale()
{
    return m_fSpeedScale;
}

bool xaeBlockTreasure::getHits()
{
    return m_bHits;
}
