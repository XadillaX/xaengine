#ifndef XAELOGINSCENE_H
#define XAELOGINSCENE_H
#pragma once
#include "global.h"
#include <xaeSceneObject.h>
#include <gui/xaeGUIObject.h>
#include <gui/xaeGUIButton.h>

class xaeLoginScene : public xaeSceneObject
{
public:
    xaeLoginScene(void);
    virtual ~xaeLoginScene(void);

    virtual bool                    update(float fDT);
    virtual bool                    render();

private:
    void                            _init();
    void                            _create_stage_gui();

private:
    xaeGUIObject*                   m_pGUI;
};

#endif
