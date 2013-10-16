#ifndef XAEREVIVESCENE_H
#define XAEREVIVESCENE_H

#pragma once
#include <xaesceneobject.h>
#include "global.h"
#include <xaeFont.h>
#include <gui/xaeGUIButton.h>

class xaeReviveScene : public xaeSceneObject
{
public:
    xaeReviveScene(void);
    virtual ~xaeReviveScene(void);

    void                        __init();

    virtual bool                render();
    virtual bool                update(float fDT);

    void                        setLevel(string szLevel) { m_szLevel = szLevel; }

private:
    xaeGUIObject*               m_pGUI;
    string                      m_szLevel;
    string                      m_szBGMFilename;
};

#endif
