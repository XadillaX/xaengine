/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* XAE Sprite : based on hgeSprite
*/
#ifndef XAESPRITE_H
#define XAESPRITE_H
#pragma once
#include "xaeResourceObject.h"

class XAEEXP xaeSprite : public xaeResourceObject
{
public:
    xaeSprite(void);
    xaeSprite(const char* path, int x = 0, int y = 0, int w = 0, int h = 0);
    virtual ~xaeSprite(void);

    /** ������Դ */
    virtual bool                load_resource(string path);
    virtual bool                load_resource(string path, int x, int y, int w, int h);
    /** ��CFG������Դ */
    virtual bool                load_resource_from_cfg(string path);
    /** ��ͼƬ�ļ�������Դ */
    virtual bool                load_resource_from_pic(string path, int x = 0, int y = 0, int w = 0, int h = 0, int hx = 0, int hy = 0);
    /** ��Ⱦ���� */
    virtual void                render(float x = 0.0f, float y = 0.0f);
    virtual void                render_stretch(float x1, float y1, float x2, float y2);
    virtual void                render_ex(float x, float y, float rot = 0.0f, float hscale = 1.0f, float vscale = 0.0f);
    /** ��ȡhgeSprite */
    virtual hgeSprite*          get_sprite();

    /** ��������� */
    virtual void                lock(bool light_lock = true);
    virtual void                unlock(bool light_lock = true);

    /** !�ͷ�: ������������ */
    virtual void                release();

protected:
    hgeSprite*                  m_pSprite;
    HTEXTURE                    m_hTex;
};
#endif
