#include "global.h"

#include "xaeWelcomeScene.h"
#include "xaeAVGScene.h"
#include "xaeBlockScene.h"
#include "xaeLoadingScene.h"
#include "xaeLogoScene.h"
#include "xaeReviveScene.h"
#include "xaeUglyLoadingScene.h"

HWND g_hWnd = 0;
string g_szDefaultFont = "黑体";
xaeBGMPlayer g_BGMPlayer;
int g_nReboundAlgorithm = 1;
bool g_bDebugStop = false;
xaeFont* g_pDebugFont = NULL;
gameSettingStruct g_Setting;
int g_nVolume = 100;

xaeSceneObject* _newwelcomescene() { return new xaeWelcomeScene(); }
xaeSceneObject* _newavgscene() { return new xaeAVGScene(); }
xaeSceneObject* _newblockscene() { return new xaeBlockScene(); }
xaeSceneObject* _newloadingscene() { return new xaeLoadingScene(); }
xaeSceneObject* _newlogoscene() { return new xaeLogoScene(); }
xaeSceneObject* _newrevivescene() { return new xaeReviveScene(); }
xaeSceneObject* _newuglyloadingscene() { return new xaeUglyLoadingScene(); }

void init_scene_factory()
{
    xaeSceneMgr::Instance().register_scene("Welcome", _newwelcomescene);
    xaeSceneMgr::Instance().register_scene("AVG", _newavgscene);
    xaeSceneMgr::Instance().register_scene("Block", _newblockscene);
    //xaeSceneMgr::Instance().register_scene("Loading", _newloadingscene);
    xaeSceneMgr::Instance().register_scene("Logo", _newlogoscene);
    xaeSceneMgr::Instance().register_scene("Revive", _newrevivescene);
    xaeSceneMgr::Instance().register_scene("Loading", _newuglyloadingscene);
}

/** 读取配置文件 */
#include <CCFGReader.h>

/** 从字符串获取颜色 */
DWORD getColorFromString(string color)
{
    char c[3][3];
    int nc[3];
    for(int i = 0, idx = 0; i < 3; i++)
    {
        for(int j = 0; j < 2; j++, idx++)
        {
            c[i][j] = color[idx];
        }
        c[i][2] = '\0';

        sscanf(c[i], "%x", nc + i);
    }

    return ARGB(255, nc[0], nc[1], nc[2]);
}

bool getGameSetting()
{
    xaeResourceBody tagRB;
#ifndef __JP
    bool bRes = xae::Instance().get_resource_mgr()->get_resource_bin("media/setting.cfg", tagRB);
#else
    bool bRes = xae::Instance().get_resource_mgr()->get_resource_bin("media/setting-jp.cfg", tagRB);
#endif
    if(!bRes) return false;

    /** 配置文件 */
    CCFGReader reader;
    reader.SetFile((char*)tagRB.m_pRes, tagRB.m_nSize);

    /** 系统相关 */
    g_Setting.System.m_szGameTitle      = reader.GetString("GAME_TITLE");

    /** 文件相关 */
    g_Setting.File.m_szScriptFile       = reader.GetString("AVG_SCRIPT_FILE");
    g_Setting.File.m_szBGPath           = reader.GetString("AVG_BG_PATH");
    g_Setting.File.m_szBGMPath          = reader.GetString("AVG_BGM_PATH");
    g_Setting.File.m_szMaidDrawPath     = reader.GetString("AVG_MAID_DRAW_PATH");
    g_Setting.File.m_szVoicePath        = reader.GetString("AVG_VOICE_PATH");
    g_Setting.File.m_szLevelPath        = reader.GetString("LEVEL_PATH");
    g_Setting.File.m_szVideoPath        = reader.GetString("VIDEO_PATH");
    g_Setting.File.m_szTempPath         = reader.GetString("TEMP_PATH");

    /** LOGO界面 */
    g_Setting.Logo.m_fLogoFadeTimer     = reader.GetFloat("LOGO_FADE_TIMER");
    g_Setting.Logo.m_fLogoPauseTimer    = reader.GetFloat("LOGO_PAUSE_TIMER");

    /** AVG相关 */
    g_Setting.AVG.m_fTextPerTimer       = reader.GetFloat("TEXT_CHAR_PER_TIMER");
    
    g_Setting.AVG.m_nTextboxLeft        = reader.GetInt("TEXTBOX_RECT_LEFT");
    g_Setting.AVG.m_nTextboxRight       = reader.GetInt("TEXTBOX_RECT_RIGHT");
    g_Setting.AVG.m_nTextboxTop         = reader.GetInt("TEXTBOX_RECT_TOP");
    g_Setting.AVG.m_nTextboxBottom      = reader.GetInt("TEXTBOX_RECT_BOTTOM");

    g_Setting.AVG.m_nTalkerLeft         = reader.GetInt("TALKER_RECT_LEFT");
    g_Setting.AVG.m_nTalkerRight        = reader.GetInt("TALKER_RECT_RIGHT");
    g_Setting.AVG.m_nTalkerTop          = reader.GetInt("TALKER_RECT_TOP");
    g_Setting.AVG.m_dwTalkerColor       = getColorFromString(reader.GetString("TALKER_COLOR"));
    g_Setting.AVG.m_nTalkerSize         = reader.GetInt("TALKER_SIZE");

    g_Setting.AVG.m_nTextLeft           = reader.GetInt("TEXT_POS_LEFT");
    g_Setting.AVG.m_nTextTop            = reader.GetInt("TEXT_POS_TOP");
    g_Setting.AVG.m_nTextMaxPerLine     = reader.GetInt("MAX_TEXT_NUM_PER_LINE");
    g_Setting.AVG.m_dwTextColor         = getColorFromString(reader.GetString("TEXT_COLOR"));
    g_Setting.AVG.m_nTextSize           = reader.GetInt("TEXT_SIZE");

    /** 砖块相关 */
    g_Setting.Block.m_nBlockWidth       = reader.GetInt("BLOCK_WIDTH");
    g_Setting.Block.m_nBlockHeight      = reader.GetInt("BLOCK_HEIGHT");
    g_Setting.Block.m_nBlockRowCount    = reader.GetInt("BLOCK_ROW_COUNT");
    g_Setting.Block.m_nBlockColCount    = reader.GetInt("BLOCK_COL_COUNT");
    g_Setting.Block.m_nBallLength       = reader.GetInt("BALL_LENGTH");
    g_Setting.Block.m_fFlapY            = reader.GetFloat("FLAP_Y");
    g_Setting.Block.m_nFlapLength       = reader.GetInt("FLAP_LENGTH");
    g_Setting.Block.m_fTreasureMaxSpeed = reader.GetFloat("TREASURE_MAX_SPEED");
    g_Setting.Block.m_fTreasureMinSpeed = reader.GetFloat("TREASURE_MIN_SPEED");
    g_Setting.Block.m_fTreasureTimer    = reader.GetFloat("TREASURE_TIME");
    g_Setting.Block.m_fTreasureSpeedScale = reader.GetFloat("SPEED_PER_SCALE");
    g_Setting.Block.m_nTreasureProb     = reader.GetFloat("TREASURE_PROB");
    g_Setting.Block.m_fFlapSpeed        = reader.GetFloat("FLAP_SPEED");
    g_Setting.Block.m_fBallSpeed        = reader.GetFloat("BALL_SPEED");
    g_Setting.Block.m_szPreBoss         = reader.GetString("PRE_BOSS");
    g_Setting.Block.m_fMaxHorizontalRate= reader.GetFloat("MAX_HORIZONTAL_RATE");

    return true;
}
