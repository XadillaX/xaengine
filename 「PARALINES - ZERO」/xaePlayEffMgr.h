#ifndef XAEPLAYEFFMGR_H
#define XAEPLAYEFFMGR_H
#pragma once

#include "global.h"
#include <xaeSound.h>
#include <xae.h>
#include <xaeSoundLoader.h>

#define EFF_FADE_TIMER (0.7f)
class xaePlayEffMgr
{
public:
    xaePlayEffMgr(void);
    virtual ~xaePlayEffMgr(void);

    void                            set_content_snd(string name);
    void                            stop_content_snd();

    void                            set_scene_snd(string name);
    void                            stop_scene_snd();

    void                            set_para_snd(string name);
    void                            stop_para_snd();

    void                            update(float fDT);

private:
    xaeSound*                       m_pSceneSnd;
    xaeSound*                       m_pParaSnd;
    xaeSound*                       m_pContentSnd;

    xaeSound*                       m_pNxtSceneSnd;
    xaeSound*                       m_pNxtParaSnd;
    xaeSound*                       m_pNxtContentSnd;

    bool                            m_bSSFadingOut;
    bool                            m_bPSFadingOut;
    bool                            m_bCSFadingOut;

    float                           m_fSSFading;
    float                           m_fPSFading;
    float                           m_fCSFading;
};
#endif
