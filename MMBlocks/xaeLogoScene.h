#ifndef XAELOGOSCENE_H
#define XAELOGOSCENE_H

#pragma once
#include "global.h"
#include "xaesceneobject.h"

enum XAE_FADE_STATE
{
    XFS_FADE_IN,
    XFS_FADE_PAUSE,
    XFS_FADE_OUT
};

class xaeLogoScene : public xaeSceneObject
{
public:
    xaeLogoScene(void);
    virtual ~xaeLogoScene(void);

    virtual bool                update(float fDT);
    virtual bool                render();

private:
    xaeSprite*                  m_pLogo;
    
    float                       m_fTimer;
    XAE_FADE_STATE              m_emState;

    BYTE                        m_byteAlpha;
};

#endif
