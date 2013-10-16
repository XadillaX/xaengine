#ifndef XAEWELCOMESCENE_H
#define XAEWELCOMESCENE_H
#pragma once

#include "global.h"
#include <xaeSceneObject.h>
#include <xaeSound.h>
#include <xaeParticles.h>
#include <gui/xaeGUIStaticImage.h>

enum PARA_SHOW_CG_STATE
{
    SCS_NO_CG,
    SCS_CG_LIST,
    SCS_CG
};

class xaeWelcomeScene : public xaeSceneObject
{
public:
    xaeWelcomeScene(void);
    virtual ~xaeWelcomeScene(void);

    virtual bool                    update(float fDT);
    virtual bool                    render();

private:
    void                            __init();
    void                            __init_cg_list(xaeGUIStaticImage* pBoard);

private:
    xaeParticles*                   m_pFire;
    xaeSound*                       m_pSndBGM;
    bool                            m_bFirst;
    xaeGUIObject*                   m_pGUI;
    xaeGUIStaticImage*              m_pShowCG;
    xaeGUIStaticImage*              m_pCGBoard;

    PARA_SHOW_CG_STATE              m_emState;
    bool                            m_nCGPage;
};
#endif
