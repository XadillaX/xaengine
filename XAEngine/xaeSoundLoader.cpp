/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: Sound Loader
 */
#include "xaeSoundLoader.h"

xaeSoundLoader::xaeSoundLoader() :
    m_szXmlPath("")
{
    m_pHGE = hgeCreate(HGE_VERSION);
    assert(m_pHGE);
}

xaeSoundLoader::~xaeSoundLoader()
{
    m_pHGE->Release();
    m_List.clear();
}

bool xaeSoundLoader::set_xml(string szXmlPath)
{
    /** 获取XML内容 */
    xaeResourceBody tagBody;
    bool bSucc = xae::Instance().get_resource_mgr()->get_resource_bin(szXmlPath, tagBody);

    /** 不成功 */
    if(!bSucc)
    {
        m_pHGE->System_Log("[x] Can't load SoundMgr File: [%s]", szXmlPath.c_str());
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

xaeSound* xaeSoundLoader::get_sound(string type, string name)
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
        m_pHGE->System_Log("[!] Can't find the sound type [%s].", type.c_str());
        return new xaeSound();
    }
    if(m_List[type].find(name) == m_List[type].end())
    {
        m_pHGE->System_Log("[!] Can't find the sound [%s][%s].", type.c_str(), name.c_str());
        return new xaeSound();
    }

    /** 新建Sound */
    xaeSoundMgrElement tagSME = m_List[type][name];

    xaeSound* pSound = new xaeSound();
    pSound->load_resource(tagSME.path);

    return pSound;
}

void xaeSoundLoader::parse_xml()
{
    /** 清空先前记录 */
    m_List.clear();

    TiXmlNode* pNode = m_Xml.RootElement();

    /** 从第一个type开始遍历 */
    for(TiXmlElement* pEle = pNode->FirstChildElement("SoundInformation");
        pEle != NULL;
        pEle = pEle->NextSiblingElement("SoundInformation")
        )
    {
        /** 大写 */
        xstr.SetString(pEle->Attribute("type"));
        xstr.ToUpper();
        string szType = xstr.GetString();

        /** 遍历 */
        for(TiXmlElement* pSubEle = pEle->FirstChildElement("Sound");
            pSubEle != NULL;
            pSubEle = pSubEle->NextSiblingElement("Sound")
            )
        {
            xstr.SetString(pSubEle->Attribute("name"));
            xstr.ToUpper();
            string szName = xstr.GetString();

            xaeSoundMgrElement tagSME;

            tagSME.name = szName;
            tagSME.path = pSubEle->GetText();
            tagSME.type = szType;

            m_List[szType][szName] = tagSME;
        }
    }
}
