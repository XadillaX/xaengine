#ifndef XAEWELCOMESCENE_H
#define XAEWELCOMESCENE_H

#pragma once
#include <xaesceneobject.h>
#include "global.h"

class xaeWelcomeScene : public xaeSceneObject
{
public:
    xaeWelcomeScene(void);
    virtual ~xaeWelcomeScene(void);

    virtual bool                    update(float fDT);
    virtual bool                    render();

private:
    void                            __init();

private:
    xaeGUIObject*                   m_pGUI;
};

#endif
