/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: Sprite Manager
 */
#ifndef XAESPRITELOADER_H
#define XAESPRITELOADER_H
#pragma once
#include <map>
#include <string>
#include "utils.h"
#include "ixaeobject.h"
#include "xae.h"
#include "xaeSprite.h"
#include "tinyxml/tinyxml.h"
using namespace std;

struct xaeSpriteMgrElement
{
    string type;
    string name;

    bool bCFG;
    int x;
    int y;
    int w;
    int h;
    int hotspotx;
    int hotspoty;
    string path;
};

typedef map<string, map<string, xaeSpriteMgrElement>> xae_spr_mgr_element_map;
#define XAE_SPRITE_LDR_VALUE_DEFAULT (-2000000000)

class XAEEXP xaeSpriteLoader : public iXAEObject, public Singleton<xaeSpriteLoader>
{
public:
    xaeSpriteLoader();
    virtual ~xaeSpriteLoader();

    bool                        set_xml(string szXmlPath);
    xaeSprite*                  get_sprite(string type,
                                           string name,
                                           int x = XAE_SPRITE_LDR_VALUE_DEFAULT,
                                           int y = XAE_SPRITE_LDR_VALUE_DEFAULT,
                                           int w = XAE_SPRITE_LDR_VALUE_DEFAULT,
                                           int h = XAE_SPRITE_LDR_VALUE_DEFAULT,
                                           int hotspotx = XAE_SPRITE_LDR_VALUE_DEFAULT,
                                           int hotspoty = XAE_SPRITE_LDR_VALUE_DEFAULT
                                           );

protected:
    void                        parse_xml();

protected:
    string                      m_szXmlPath;
    TiXmlDocument               m_Xml;

    xae_spr_mgr_element_map     m_List;

    XStringFunc                 xstr;
};

#define xaeSpriteLdrIns xaeSpriteLoader::Instance()

#endif
