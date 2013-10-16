#ifndef GLOBAL_H
#define GLOBAL_H

#include <utils.h>
#include <xae.h>
#include "xaePackage.h"
#include <gui/xaeGUIObject.h>
#include <xaeSceneMgr.h>
#include <vector>
#include <string>
using namespace std;

#include <xaeSpriteLoader.h>
#include <xaeSoundLoader.h>
#include <gui/xaeGUIButton.h>

#define GAME_TITLE              "- Å¾À²ºÚ¡¤ÁãÕÂ ¸´¿Ì°æ -"
#define GAME_WIDTH              800
#define GAME_HEIGHT             600
#define GAME_REAL_WIDTH         800
#define GAME_REAL_HEIGHT        480
#define GAME_STATE_PATH         "state.game.paralines"
#define GAME_VIDEO_PATH         "video.paralines.tmp"
#define GAME_SAVE_FILENAME      "paralines.prsv"
#define GAME_SAVE_MAGIC_NUM     "PRXA"

struct para_save_struct
{
    char mg[16];
    int  stage;
    bool cglist[32];

    para_save_struct()
    {
        memset(mg, 0, sizeof(mg));
        stage = 0;
        memset(cglist, 0, sizeof(cglist));
    }
};

extern void read_save();
extern void save_save();
extern void init_scene_factory();
extern const int g_nStageCount;
extern const char g_szStageNum[16][8];
extern HWND g_hWnd;
extern string g_szDefaultFont;
extern const int g_nCGList[20];
extern const int g_nCGCount;
extern para_save_struct g_tagSave;

#endif
