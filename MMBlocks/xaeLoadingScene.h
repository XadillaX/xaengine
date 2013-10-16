#ifndef XAELOADINGSCENE_H
#define XAELOADINGSCENE_H

#pragma once
#include "global.h"
#include <xaeFont.h>
#include <xaeSceneObject.h>

#define XAE_DOT_CHANGE_TIMER    (0.2f)

class xaeLoadingScene : public xaeSceneObject
{
public:
    xaeLoadingScene(void);
    virtual ~xaeLoadingScene(void);

    virtual bool                update(float fDT);
    virtual bool                render();

    void                        loadingSceneAndShow(loadingThreadFuncType ThreadFunc, void* para = NULL);
    
private:
    void                        __initResource();
    void                        __changeDot();
    void                        __resetThreadPara();

private:
    int                         m_nDotCount;
    float                       m_fDotChangeTimer;

    loadingThreadPara*          m_pThreadPara;

    xaeFont*                    m_pFont;
};

#endif
