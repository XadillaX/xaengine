#ifndef XAEBGMPLAYER_H
#define XAEBGMPLAYER_H

#pragma once

#include <xaeSound.h>
#include <xaeSoundLoader.h>
#include <string>
using namespace std;

#define BGM_FADE_TIMER (0.7f)

class xaeBGMPlayer
{
public:
    xaeBGMPlayer(void);
    virtual ~xaeBGMPlayer(void);

    void                        setBGM(string filename);
    void                        stopBGM();

    void                        update(float fDT);

    void                        release();

    string                      getFilename() { return m_szFilename; }
    string                      getNextFilename() { return m_szNxtFilename; }

private:
    xaeSound*                   m_pBGM;
    xaeSound*                   m_pNxtBGM;
    bool                        m_bFadingOut;
    float                       m_fFading;

    string                      m_szFilename;
    string                      m_szNxtFilename;
};

#endif
