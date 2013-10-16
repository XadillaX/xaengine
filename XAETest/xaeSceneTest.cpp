#include "xaeSceneTest.h"

xaeSceneObject* newtestscene() { return new xaeSceneTest(); }
xaeSceneObject* newblankscene() { return new BlankScene(); }

void init_scene_factory()
{
    xaeSceneMgr::Instance().register_scene("test", newtestscene);
    xaeSceneMgr::Instance().register_scene("blank", newblankscene);
}
