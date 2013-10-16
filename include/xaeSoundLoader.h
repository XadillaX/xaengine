/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: Sound Loader
 */
#ifndef XAESOUNDLOADER_H
#define XAESOUNDLOADER_H
#include "utils.h"
#include "ixaeobject.h"
#include "xae.h"
#include "tinyxml/tinyxml.h"
#include "xaeSound.h"

struct xaeSoundMgrElement
{
    string type;
    string name;
    string path;
};

typedef map<string, map<string, xaeSoundMgrElement>> xae_snd_mgr_element_map;

class XAEEXP xaeSoundLoader : public iXAEObject, public Singleton<xaeSoundLoader>
{
public:
    xaeSoundLoader();
    virtual ~xaeSoundLoader();

    bool                        set_xml(string szXmlPath);
    xaeSound*                   get_sound(string type, string name);

protected:
    void                        parse_xml();

protected:
    string                      m_szXmlPath;
    TiXmlDocument               m_Xml;

    xae_snd_mgr_element_map     m_List;

    XStringFunc                 xstr;
};

#endif
