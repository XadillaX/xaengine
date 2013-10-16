#include "xaeLoadingScene.h"

xaeLoadingScene::xaeLoadingScene(void) :
    m_nDotCount(1),
    m_fDotChangeTimer(XAE_DOT_CHANGE_TIMER),
    m_pFont(NULL),
    m_pThreadPara(NULL)
{
    __initResource();

    m_pThreadPara = new loadingThreadPara();

    /** ��ʼ���̲߳��� */
    InitializeCriticalSection(&(m_pThreadPara->m_CriticalSection));
    __resetThreadPara();
}

xaeLoadingScene::~xaeLoadingScene(void)
{
    DeleteCriticalSection(&(m_pThreadPara->m_CriticalSection));
    SAFEDEL(m_pThreadPara);

    release_sprite_pool();
}

void xaeLoadingScene::__initResource()
{
    add_sprite_to_pool("Background",
        xaeSpriteLoader::Instance().get_sprite("Loading", "BG"),
        0,
        0);

    m_pFont = new xaeFont(g_szDefaultFont.c_str(), 16);
    m_pFont->SetColor(0xffffffff);
}

void xaeLoadingScene::loadingSceneAndShow(loadingThreadFuncType ThreadFunc, void* para)
{
    LockScope oScope(m_pThreadPara->m_CriticalSection);

    __resetThreadPara();
    m_pThreadPara->m_pOtherPara = para;
    CreateThread(NULL, 0, ThreadFunc, (LPVOID)(m_pThreadPara), NULL, 0);
}

bool xaeLoadingScene::update(float fDT)
{
    /** ��� */
    xaeGUIObject::set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());

    /** ��ı仯 */
    m_fDotChangeTimer -= fDT;
    if(m_fDotChangeTimer < 0.0f)
    {
        m_fDotChangeTimer = XAE_DOT_CHANGE_TIMER;
        __changeDot();
    }

    /** ���������ı仯 */
    LockScope oScope(m_pThreadPara->m_CriticalSection);
    if(m_pThreadPara->m_bCreated)
    {
        /** ���������볡�������� */
        xaeSceneMgr::Instance().add_scene(m_pThreadPara->m_szSceneName, m_pThreadPara->m_pScene);

        /** �л����� */
        xaeSceneMgr::Instance().set_current_scene(m_pThreadPara->m_szSceneName, false);

        /** ��ʼ��m_tagThreadPara */
        __resetThreadPara();

        return false;
    }

    return false;
}

bool xaeLoadingScene::render()
{
    LockScope oScope(m_pThreadPara->m_CriticalSection);
    render_sprite_pool();

    /** ���� */
    string szDot = "��Ů������";
    for(int i = 0; i < m_nDotCount; i++) szDot += '.';
    m_pFont->Print(50, 550, szDot.c_str());

    xaeGUIObject::render_mouse();

    return false;
}

void xaeLoadingScene::__changeDot()
{
    m_nDotCount++;
    if(m_nDotCount > 6) m_nDotCount = 1;
}

void xaeLoadingScene::__resetThreadPara()
{
    LockScope oScope(m_pThreadPara->m_CriticalSection);

    m_pThreadPara->m_pScene = NULL;
    m_pThreadPara->m_bCreated = false;
    m_pThreadPara->m_szSceneName = m_pThreadPara->m_szSceneType = "";
    m_pThreadPara->m_pOtherPara = NULL;
}
