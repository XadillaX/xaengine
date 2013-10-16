#include "xaeUglyLoadingScene.h"

xaeUglyLoadingScene::xaeUglyLoadingScene(void) :
    m_pFont(NULL),
    m_bRendered(false),
    m_szPee("")
{
    __initResource();
}

xaeUglyLoadingScene::~xaeUglyLoadingScene(void)
{
    release_sprite_pool();

    SAFEDEL(m_pFont);

    g_BGMPlayer.stopBGM();
}

void xaeUglyLoadingScene::__initResource()
{
    add_sprite_to_pool("Background",
        xaeSpriteLoader::Instance().get_sprite("Loading", "BG"),
        0,
        0);

    m_pFont = new xaeFont(g_szDefaultFont.c_str(), 16);
    m_pFont->SetColor(0xffffffff);

#ifdef __JP
    m_szPee = "�E�E�E�@�������A�� o(#�Pv�P)=O >>>>>> �E�E�E";
#else
    m_szPee = "����������Ů������ o(#��v��)=O >>>>>> ������";
#endif
}

bool xaeUglyLoadingScene::render()
{
    render_sprite_pool();

    m_pFont->Print(50, 550, m_szPee.c_str());

    m_bRendered = true;

    return false;
}

bool xaeUglyLoadingScene::update(float fDT)
{
    g_BGMPlayer.update(fDT);
    if(!m_bRendered) return false;

    xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("AVG", "AVG");
    xaeSceneMgr::Instance().add_scene("AVG", scene);
    xaeSceneMgr::Instance().set_current_scene("AVG");

    return true;
}
