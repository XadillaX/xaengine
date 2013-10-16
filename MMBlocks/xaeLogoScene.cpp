#include "xaeLogoScene.h"
#include "xaeWelcomeScene.h"

xaeLogoScene::xaeLogoScene(void) :
    m_pLogo(NULL),
    m_fTimer(0.0f),
    m_byteAlpha(0),
    m_emState(XFS_FADE_IN)
{
    m_pLogo = xaeSpriteLoader::Instance().get_sprite("Logo", "Logo");
}

xaeLogoScene::~xaeLogoScene(void)
{
    SAFEDEL(m_pLogo);
}

bool xaeLogoScene::update(float fDT)
{
    switch(m_emState)
    {
    case XFS_FADE_IN:
        {
            m_fTimer += fDT; // Delta Time
            if(m_fTimer > g_Setting.Logo.m_fLogoFadeTimer)
            {
                m_emState = XFS_FADE_PAUSE;
                m_fTimer = 0.0f;

                m_byteAlpha = 255;

                return false;
            }

            m_byteAlpha = (BYTE)(255.0f * (m_fTimer / g_Setting.Logo.m_fLogoFadeTimer));
            break;
        }

    case XFS_FADE_PAUSE:
        {
            m_fTimer += fDT;
            if(m_fTimer > g_Setting.Logo.m_fLogoPauseTimer)
            {
                m_emState = XFS_FADE_OUT;
                m_fTimer = 0.0f;

                m_byteAlpha = 255;

                return false;
            }

            m_byteAlpha = 255;
            break;
        }

    case XFS_FADE_OUT:
        {
            m_fTimer += fDT;
            if(m_fTimer > g_Setting.Logo.m_fLogoFadeTimer)
            {
                m_emState = XFS_FADE_OUT;
                m_fTimer = 0.0f;

                m_byteAlpha = 0;

                xaeWelcomeScene* scene = (xaeWelcomeScene*)(xaeSceneMgr::Instance().create_scene("Welcome", "Welcome"));
                if(NULL != scene)
                {
                    xaeSceneMgr::Instance().add_scene("Welcome", scene);
                }
                xaeSceneMgr::Instance().set_current_scene("Welcome");

                return false;
            }

            m_byteAlpha = (BYTE)(255.0f * (1.0f - m_fTimer / g_Setting.Logo.m_fLogoFadeTimer));;
            break;
        }
    }

    return 0;
}

bool xaeLogoScene::render()
{
    int nWidth, nHeight;
    nWidth = m_pLogo->get_sprite()->GetWidth();
    nHeight = m_pLogo->get_sprite()->GetHeight();

    m_pLogo->get_sprite()->SetColor(ARGB(m_byteAlpha, 255, 255, 255));
    m_pLogo->render((800 - nWidth) / 2, (600 - nHeight) / 2);

    return 0;
}
