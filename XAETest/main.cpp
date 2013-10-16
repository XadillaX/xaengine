
#include "vld.h"

#include <xae.h>
#include <xaeSceneMgr.h>
#include "xaeSceneTest.h"
#include <xaeSpriteLoader.h>
#include <ctime>

extern void init_scene_factory();



#ifdef _DEBUG
int main()
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#endif
{
    VLDSetOptions(VLD_OPT_MODULE_LIST_INCLUDE|VLD_OPT_AGGREGATE_DUPLICATES, 0, -1);
    srand(time(NULL));
    xae::Instance().set_core("Ü½Ìm¶äÂ¶¤Î‘n‚û", 920, 600, 100, 32, true, "test.log");
    init_scene_factory();
    if(xae::Instance().init_core())
    {
        xae::Instance().set_resource_mgr(new xaeResMgr());
        xaeSpriteLoader::Instance().set_xml("resourcemgr/sprite.xml");

        xaeSceneTest* scene = new xaeSceneTest();
        xaeSceneMgr::Instance().add_scene("test", scene);
        xaeSceneMgr::Instance().set_current_scene("test");

        xaeSceneMgr::Instance().set_default_mouse("Cursor*Pointer");

        xae::Instance().start_core();
        xae::Instance().shutdown_core();
        xae::Instance().release_core();
    }
    else xae::Instance().alert("ÄÚºË³õÊ¼»¯´íÎó¡£", "´íÎó", MB_ICONERROR);

    return 0;
}
