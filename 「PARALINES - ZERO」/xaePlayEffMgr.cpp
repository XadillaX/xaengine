#include "xaePlayEffMgr.h"

xaePlayEffMgr::xaePlayEffMgr(void) :
    m_pParaSnd(NULL),
    m_pContentSnd(NULL),
    m_pSceneSnd(NULL),

    m_bCSFadingOut(false),
    m_bPSFadingOut(false),
    m_bSSFadingOut(false),

    m_fCSFading(EFF_FADE_TIMER),
    m_fPSFading(EFF_FADE_TIMER),
    m_fSSFading(EFF_FADE_TIMER),

    m_pNxtSceneSnd(NULL),
    m_pNxtContentSnd(NULL),
    m_pNxtParaSnd(NULL)
{
    xaeSoundLoader::Instance().set_xml("media/loader/playsound.xml");
}

xaePlayEffMgr::~xaePlayEffMgr(void)
{
    SAFEDEL(m_pContentSnd);
    SAFEDEL(m_pNxtContentSnd);

    SAFEDEL(m_pParaSnd);
    SAFEDEL(m_pNxtParaSnd);

    SAFEDEL(m_pSceneSnd);
    SAFEDEL(m_pNxtSceneSnd);
}

void xaePlayEffMgr::update(float fDT)
{
    /** ֹͣContent Sound */
    if(m_bCSFadingOut)
    {
        m_fCSFading -= fDT;
        if(m_fCSFading <= 0)
        {
            m_pContentSnd->stop();
            m_pContentSnd->release();
            SAFEDEL(m_pContentSnd);

            if(m_pNxtContentSnd)
            {
                m_pContentSnd = m_pNxtContentSnd;
                m_pNxtContentSnd = NULL;

                m_pContentSnd->set_loop();
                m_pContentSnd->play();
                m_pContentSnd->set_volume(0);
                m_pContentSnd->slide_to(EFF_FADE_TIMER, 100);
            }

            m_fCSFading = EFF_FADE_TIMER;
            m_bCSFadingOut = false;
        }
    }

    /** ֹͣScene Sound */
    if(m_bSSFadingOut)
    {
        m_fSSFading -= fDT;
        if(m_fSSFading <= 0)
        {
            m_pSceneSnd->stop();
            m_pSceneSnd->release();
            SAFEDEL(m_pSceneSnd);

            if(m_pNxtSceneSnd)
            {
                m_pSceneSnd = m_pNxtSceneSnd;
                m_pNxtSceneSnd = NULL;

                m_pSceneSnd->set_loop();
                m_pSceneSnd->play();
                m_pSceneSnd->set_volume(0);
                m_pSceneSnd->slide_to(EFF_FADE_TIMER, 100);
            }

            m_fSSFading = EFF_FADE_TIMER;
            m_bSSFadingOut = false;
        }
    }

    /** ֹͣPara Sound */
    if(m_bPSFadingOut)
    {
        m_fPSFading -= fDT;
        if(m_fPSFading <= 0)
        {
            m_pParaSnd->stop();
            m_pParaSnd->release();
            SAFEDEL(m_pParaSnd);

            if(m_pNxtParaSnd)
            {
                m_pParaSnd = m_pNxtParaSnd;
                m_pNxtParaSnd = NULL;

                m_pParaSnd->set_loop();
                m_pParaSnd->play();
                m_pParaSnd->set_volume(0);
                m_pParaSnd->slide_to(EFF_FADE_TIMER, 100);
            }

            m_fPSFading = EFF_FADE_TIMER;
            m_bPSFadingOut = false;
        }
    }
}

void xaePlayEffMgr::set_content_snd(string name)
{
    if(NULL == m_pContentSnd)
    {
        m_pContentSnd = xaeSoundLoader::Instance().get_sound("Sound", name);
        
        m_pContentSnd->set_loop();
        m_pContentSnd->play();
        m_pContentSnd->set_volume(0);
        m_pContentSnd->slide_to(EFF_FADE_TIMER, 100);
    }
    else
    {
        stop_content_snd();
        m_pNxtContentSnd = xaeSoundLoader::Instance().get_sound("Sound", name);
    }
}

void xaePlayEffMgr::stop_content_snd()
{
    if(NULL != m_pContentSnd)
    {
        m_pContentSnd->slide_to(EFF_FADE_TIMER, 0);
        m_bCSFadingOut = true;
        m_fCSFading = EFF_FADE_TIMER;
    }
}

void xaePlayEffMgr::set_scene_snd(string name)
{
    if(NULL == m_pSceneSnd)
    {
        m_pSceneSnd = xaeSoundLoader::Instance().get_sound("Sound", name);
        
        m_pSceneSnd->set_loop();
        m_pSceneSnd->play();
        m_pSceneSnd->set_volume(0);
        m_pSceneSnd->slide_to(EFF_FADE_TIMER, 100);
    }
    else
    {
        stop_scene_snd();
        m_pNxtSceneSnd = xaeSoundLoader::Instance().get_sound("Sound", name);
    }
}

void xaePlayEffMgr::stop_scene_snd()
{
    if(NULL != m_pSceneSnd)
    {
        m_pSceneSnd->slide_to(EFF_FADE_TIMER, 0);
        m_bSSFadingOut = true;
        m_fSSFading = EFF_FADE_TIMER;
    }
}

void xaePlayEffMgr::set_para_snd(string name)
{
    if(NULL == m_pParaSnd)
    {
        m_pParaSnd = xaeSoundLoader::Instance().get_sound("Sound", name);
        
        m_pParaSnd->set_loop();
        m_pParaSnd->play();
        m_pParaSnd->set_volume(0);
        m_pParaSnd->slide_to(EFF_FADE_TIMER, 100);
    }
    else
    {
        stop_para_snd();
        m_pNxtParaSnd = xaeSoundLoader::Instance().get_sound("Sound", name);
    }
}

void xaePlayEffMgr::stop_para_snd()
{
    if(NULL != m_pParaSnd)
    {
        m_pParaSnd->slide_to(EFF_FADE_TIMER, 0);
        m_bPSFadingOut = true;
        m_fPSFading = EFF_FADE_TIMER;
    }
}
