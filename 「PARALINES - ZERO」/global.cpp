#include "global.h"

#include "xaeWelcomeScene.h"
#include "xaeLoginScene.h"
#include "xaePlayScene.h"
xaeSceneObject* _newwelcomescene() { return new xaeWelcomeScene(); }
xaeSceneObject* _newloginscene() { return new xaeLoginScene(); }
xaeSceneObject* _newplayscene() { return new xaePlayScene(); }

HWND g_hWnd = 0;
string g_szDefaultFont = "ºÚÌå";

const int g_nStageCount = 13;
const char g_szStageNum[16][8] = {
    "09", "10", "11", "12",
    "01", "02", "03", "04", "05", "06", "07", "08",
    "13"
};
const int g_nCGList[20] = {
    990001,
    990002,
    990003,
    990004,
    990005,
    990006,
    990007,
    990008,
    990009,
    990011,
    990012,
    990013,
    990015,
    990016,
    990020,
    990021,
    990022,
    990023,
    990024
};
const int g_nCGCount = 19;

para_save_struct g_tagSave;

void init_scene_factory()
{
    xaeSceneMgr::Instance().register_scene("Welcome", _newwelcomescene);
    xaeSceneMgr::Instance().register_scene("Login", _newloginscene);
    xaeSceneMgr::Instance().register_scene("Play", _newplayscene);
}

void read_save()
{
    strcpy(g_tagSave.mg, GAME_SAVE_MAGIC_NUM);
    FILE* fp = fopen(string(string(GAME_STATE_PATH) + "\\" + string(GAME_SAVE_FILENAME)).c_str(), "rb");
    if(NULL == fp) return;
    fread(&g_tagSave, sizeof(para_save_struct), 1, fp);
    fclose(fp);

    if(strcmp(g_tagSave.mg, GAME_SAVE_MAGIC_NUM) != 0)
    {
        memset(g_tagSave.cglist, 0, sizeof(g_tagSave.cglist));
        g_tagSave.stage = 0;
    }
}

void save_save()
{
    FILE* fp = fopen(string(string(GAME_STATE_PATH) + "\\" + string(GAME_SAVE_FILENAME)).c_str(), "wb+");
    if(NULL == fp) return;
    fwrite(&g_tagSave, sizeof(para_save_struct), 1, fp);
    fclose(fp);
}