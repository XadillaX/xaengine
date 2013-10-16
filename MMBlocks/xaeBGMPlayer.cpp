#include "xaeBGMPlayer.h"
#include "global.h"

xaeBGMPlayer::xaeBGMPlayer(void) :
    m_pBGM(NULL),
    m_pNxtBGM(NULL),
    m_fFading(BGM_FADE_TIMER),
    m_bFadingOut(false),
    m_szFilename(""),
    m_szNxtFilename("")
{
}

xaeBGMPlayer::~xaeBGMPlayer(void)
{
}

void xaeBGMPlayer::release()
{
    if(m_pBGM != NULL)
    {
        delete m_pBGM;
        m_pBGM = NULL;
    }

    if(m_pNxtBGM != NULL)
    {
        delete m_pNxtBGM;
        m_pNxtBGM = NULL;
    }
}

void xaeBGMPlayer::setBGM(string filename)
{
    if(NULL == m_pBGM)
    {
        m_pBGM = new xaeSound(filename, true, 0);
        m_pBGM->play();
        m_pBGM->slide_to(BGM_FADE_TIMER, g_nVolume);

        m_szFilename = filename;
    }
    else
    {
        stopBGM();
        
        if(NULL != m_pNxtBGM)
        {
            delete m_pNxtBGM;
            m_pNxtBGM = NULL;
        }

        m_pNxtBGM = new xaeSound(filename, true, 0);
        m_szNxtFilename = filename;
    }
}

void xaeBGMPlayer::stopBGM()
{
    if(NULL != m_pBGM)
    {
        m_pBGM->slide_to(BGM_FADE_TIMER, 0);
        m_bFadingOut = true;
        m_fFading = BGM_FADE_TIMER;
    }
}

void xaeBGMPlayer::update(float fDT)
{
    if(m_bFadingOut)
    {            
        if(m_pNxtBGM != NULL)
        {
            m_pBGM->stop();
            m_pBGM->release();
            delete m_pBGM;
            m_pBGM = NULL;

            m_pBGM = m_pNxtBGM;
            m_pNxtBGM = NULL;
            m_pBGM->play();
            m_pBGM->slide_to(BGM_FADE_TIMER, g_nVolume);

            m_bFadingOut = false;
            m_fFading = BGM_FADE_TIMER;

            m_szFilename = m_szNxtFilename;
            m_szNxtFilename = "";

            return;
        }
        else
        {
            m_fFading -= fDT;
            if(m_fFading <= 0)
            {
                m_pBGM->stop();
                m_pBGM->release();
                delete m_pBGM;
                m_pBGM = NULL;

                m_bFadingOut = false;
                m_fFading = BGM_FADE_TIMER;

                m_szFilename = "";
            }
        }
    }
}
