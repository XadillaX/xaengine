/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Particles class
 */
#ifndef XAEPARTICLES_H
#define XAEPARTICLES_H
#pragma once
#include "utils.h"
#include "xaeSprite.h"
#include "hge/hgeparticle.h"

#define PAR_BLEND_DEFAULT (BLEND_COLORMUL | BLEND_ALPHAADD | BLEND_NOZWRITE)

class XAEEXP xaeParticles : public xaeSprite
{
public:
    xaeParticles();
    xaeParticles(const char* path);
    virtual ~xaeParticles();

    /** 载入资源 */
    virtual bool                load_resource(string path);
    virtual bool                load_resource(string path, int x, int y, int w, int h);
    /** 从CFG载入资源 */
    virtual bool                load_resource_from_cfg(string path);
    virtual bool                load_resource_from_pic(string path, int x = 0, int y = 0, int w = 0, int h = 0);
    virtual bool                load_resource_from_psi(string path, DWORD startcolor = -1, DWORD endcolor = -1, int blend = PAR_BLEND_DEFAULT, float scale = 1.0f);

    /** 设置一些属性 */
    virtual void                set_blend_mode(int blend_mode);
    virtual void                set_blend_mode(string blend_mode);
    virtual void                set_sprite(hgeSprite* sprite) {};
    virtual void                set_color(DWORD start = -1, DWORD end = -1) {};
    virtual void                set_scale(float scale) {};

    /** 渲染相关 */
    virtual void                fire();
    virtual void                fire_at(int x, int y);
    virtual void                move_to(int x, int y);
    virtual void                get_pos(int* x, int* y);
    virtual void                update(float fDT);
    virtual void                render();

    virtual hgeParticleSystem*  get_particle_core() { return m_pPar; };

    /** 释放 */
    virtual void                release();

protected:
    hgeParticleSystemInfo       m_PSI;
    hgeParticleSystem*          m_pPar;
    DWORD                       m_dwStartColor;
    DWORD                       m_dwEndColor;
    int                         m_nBlendMode;
    float                       m_fScale;
};

#endif
