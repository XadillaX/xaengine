/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: GUI Static Image
 */
#ifndef XAEGUISTATICIMAGE_H
#define XAEGUISTATICIMAGE_H
#include "../utils.h"
#include "../xae.h"
#include "../xaeSprite.h"
#include "xaeGUIObject.h"

class XAEEXP xaeGUIStaticImage : public xaeGUIObject
{
public:
    xaeGUIStaticImage(int id, string key, string path, int x, int y, int w = 0, int h = 0);
    virtual ~xaeGUIStaticImage();

    virtual void                on_render(int x, int y);

private:
    xaeSprite*                  m_pImage;
};

#endif
