#ifndef GLOBAL_H
#define GLOBAL_H

#include <utils.h>
#include <xae.h>
#include "xaePackage.h"
#include <gui/xaeGUIObject.h>
#include <xaeSceneMgr.h>
#include <vector>
#include <string>
#include <windows.h>
using namespace std;

#include <xaeSpriteLoader.h>
#include <xaeSoundLoader.h>
#include <gui/xaeGUIButton.h>
#include <xaeFont.h>

#include "xaeBGMPlayer.h"

#define GAME_WIDTH              800
#define GAME_HEIGHT             600

extern void read_save();
extern void save_save();
extern void init_scene_factory();
extern HWND g_hWnd;
extern string g_szDefaultFont;
extern int g_nVolume;
extern xaeBGMPlayer g_BGMPlayer;
extern int g_nReboundAlgorithm;

#ifdef __DEBUG
extern bool g_bDebugStop;
extern xaeFont* g_pDebugFont;
#endif

/** AVG播放状态 */
enum XAE_AVG_DISPLAY_STATUS
{
    XADS_SHOWING_TEXT,          ///< 显示文字中
    XADS_PLAYING_VOICE,         ///< 播放配音中
    XADS_PAUSED                 ///< 停止中
};

/** 创建场景线程参数 */
typedef LPTHREAD_START_ROUTINE loadingThreadFuncType;
struct loadingThreadPara
{
    xaeSceneObject*             m_pScene;
    CRITICAL_SECTION            m_CriticalSection;
    bool                        m_bCreated;

    string                      m_szSceneType;
    string                      m_szSceneName;

    void*                       m_pOtherPara;
};

extern DWORD WINAPI createWelcomeThread(LPVOID lpParam);
extern DWORD WINAPI createAVGThread(LPVOID lpParam);
extern DWORD WINAPI createBlockThread(LPVOID lpParam);

#include "xaeLoadingScene.h"

#include "xaeGameSettingStruct.h"
extern gameSettingStruct        g_Setting;
extern bool                     getGameSetting();

#endif
