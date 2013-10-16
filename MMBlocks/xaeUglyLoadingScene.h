#ifndef XAEUGLYLOADINGSCENE_H
#define XAEUGLYLOADINGSCENE_H

#pragma once
#include <utils.h>
#include <xaesceneobject.h>
#include "global.h"

class xaeUglyLoadingScene : public xaeSceneObject
{
public:
    xaeUglyLoadingScene(void);
    virtual ~xaeUglyLoadingScene(void);

    virtual bool                update(float fDT);
    virtual bool                render();

private:
    void                        __initResource();

private:
    xaeFont*                    m_pFont;
    string                      m_szPee;

    bool                        m_bRendered;
};

#endif
