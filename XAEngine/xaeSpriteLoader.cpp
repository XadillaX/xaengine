/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: Impl of Sprite Loader
 */
#include <xaeSpriteLoader.h>

xaeSpriteLoader::xaeSpriteLoader() :
    m_szXmlPath("")
{
    m_pHGE = hgeCreate(HGE_VERSION);
    assert(m_pHGE);
}

xaeSpriteLoader::~xaeSpriteLoader()
{
    m_pHGE->Release();
    m_List.clear();
}

bool xaeSpriteLoader::set_xml(string szXmlPath)
{
    /** 获取XML内容 */
    xaeResourceBody tagBody;
    bool bSucc = xae::Instance().get_resource_mgr()->get_resource_bin(szXmlPath, tagBody);

    /** 不成功 */
    if(!bSucc)
    {
        m_pHGE->System_Log("[x] Can't load SpriteMgr File: [%s]", szXmlPath.c_str());
        return false;
    }
    else
    {
        m_Xml.Clear();
        m_Xml.Parse((char*)tagBody.m_pRes, 0);

        if(m_Xml.Error())
        {
            m_pHGE->System_Log("[x] Parsing [%s] error in %s: %s", szXmlPath.c_str(), m_Xml.Value(), m_Xml.ErrorDesc());
            return false;
        }

        parse_xml();

        return true;
    }

    return false;
}

xaeSprite* xaeSpriteLoader::get_sprite(string type, string name, int x, int y, int w, int h, int hotspotx, int hotspoty)
{
    /** 大写化 */
    xstr.SetString(type);
    xstr.ToUpper();
    type = xstr.GetString();
    xstr.SetString(name);
    xstr.ToUpper();
    name = xstr.GetString();

    /** 找不到 */
    if(m_List.find(type) == m_List.end())
    {
        m_pHGE->System_Log("[!] Can't find the sprite type [%s].", type.c_str());
        return new xaeSprite();
    }
    if(m_List[type].find(name) == m_List[type].end())
    {
        m_pHGE->System_Log("[!] Can't find the sprite [%s][%s].", type.c_str(), name.c_str());
        return new xaeSprite();
    }

    /** 新建Sprite */
    xaeSpriteMgrElement tagSME = m_List[type][name];

    if(!tagSME.bCFG)
    {
        int tx = (x == XAE_SPRITE_LDR_VALUE_DEFAULT) ? tagSME.x : x;
        int ty = (y == XAE_SPRITE_LDR_VALUE_DEFAULT) ? tagSME.y : y;
        int tw = (w == XAE_SPRITE_LDR_VALUE_DEFAULT) ? tagSME.w : w;
        int th = (h == XAE_SPRITE_LDR_VALUE_DEFAULT) ? tagSME.h : h;
        int thx = (hotspotx == XAE_SPRITE_LDR_VALUE_DEFAULT) ? tagSME.hotspotx : hotspotx;
        int thy = (hotspoty == XAE_SPRITE_LDR_VALUE_DEFAULT) ? tagSME.hotspoty : hotspoty;

        xaeSprite* spr = new xaeSprite();
        spr->load_resource_from_pic(tagSME.path, tx, ty, tw, th, thx, thy);
        return spr;
    }
    else
    {
        xaeSprite* spr = new xaeSprite();
        spr->load_resource_from_cfg(tagSME.path);
        return spr;
    }

    return NULL;
}

void xaeSpriteLoader::parse_xml()
{
    /** 清空先前记录 */
    m_List.clear();

    TiXmlNode* pNode = m_Xml.RootElement();

    /** 从第一个type开始遍历 */
    for(TiXmlElement* pEle = pNode->FirstChildElement("SpriteInformation");
        pEle != NULL;
        pEle = pEle->NextSiblingElement("SpriteInformation")
        )
    {
        /** 大写 */
        xstr.SetString(pEle->Attribute("type"));
        xstr.ToUpper();
        string szType = xstr.GetString();

        /** 遍历 */
        for(TiXmlElement* pSubEle = pEle->FirstChildElement("Sprite");
            pSubEle != NULL;
            pSubEle = pSubEle->NextSiblingElement("Sprite")
            )
        {
            xstr.SetString(pSubEle->Attribute("name"));
            xstr.ToUpper();
            string szName = xstr.GetString();

            /** 填充 */
            int bResult = 0;
            xaeSpriteMgrElement tagSME;
            tagSME.path = pSubEle->GetText();
            tagSME.name = szName;
            tagSME.type = szType;

            if(pSubEle->QueryBoolAttribute("cfg", &tagSME.bCFG) != TIXML_SUCCESS) tagSME.bCFG = false;
            if(pSubEle->QueryIntAttribute("x", &tagSME.x) != TIXML_SUCCESS) tagSME.x = 0;
            if(pSubEle->QueryIntAttribute("y", &tagSME.y) != TIXML_SUCCESS) tagSME.y = 0;
            if(pSubEle->QueryIntAttribute("h", &tagSME.h) != TIXML_SUCCESS) tagSME.h = 0;
            if(pSubEle->QueryIntAttribute("w", &tagSME.w) != TIXML_SUCCESS) tagSME.w = 0;
            if(pSubEle->QueryIntAttribute("hotspotx", &tagSME.hotspotx) != TIXML_SUCCESS) tagSME.hotspotx = 0;
            if(pSubEle->QueryIntAttribute("hotspoty", &tagSME.hotspoty) != TIXML_SUCCESS) tagSME.hotspoty = 0;

            m_List[szType][szName] = tagSME;
        }
    }
}
