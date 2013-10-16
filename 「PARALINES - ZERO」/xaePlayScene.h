#ifndef XAEPLAYSCENE_H
#define XAEPLAYSCENE_H
#pragma once
#include "global.h"
#include <xaeSceneObject.h>
#include <gui/xaeGUIObject.h>
#include <gui/xaeGUIButton.h>
#include <xaeSprite.h>
#include <xaeFont.h>
#include <xaeFlashSprite.h>
#include "cxadillaxvideoplayer.h"
#include "xaePlayEffMgr.h"
#include "xaeSelectLabel.h"
#include "xaePlayReadLog.h"

#define LINE_MAX_TEXT               (36 * 2)
#define SELECT_LABEL_START_ID       100

enum para_effect
{
    PE_NO_EFFECT
    /** TODO: δ֪Effect */
};

struct para_jpg
{
    int                             paragraph;
    int                             scence_num;
    string                          scence_swf;
    string                          lefter;
    string                          center;
    string                          righter;

    xaeSprite*                      pLefter;
    xaeSprite*                      pCenter;
    xaeSprite*                      pRighter;

    para_jpg()
    {
        paragraph = scence_num = 0;
        lefter = center = righter = "";
        scence_swf = "";

        pLefter = pRighter = pCenter = NULL;
    }
};

#define SHAKE_MAX_TIMER             (0.1f)
#define FADE_MAX_TIMER              (0.1f)
#define PAUSE_MAX_TIMER             (0.05f)
struct para_content
{
    int                             index;
    int                             effect_num;
    bool                            isselect;
    string                          content;
    bool                            shake_start;
    bool                            shake_end;
    bool                            shake_timer;
    string                          sound;

    para_content()
    {
        index = effect_num = 0;
        isselect = shake_start = shake_end = shake_timer = false;
        content = "";
        sound = "";
    }
};

struct para_select
{
    int                             select_type;
    int                             start_select_paragrapha;
    string                          content;
};

struct para_msg
{
    int                             paragraph;
    string                          talker;
    string                          scene_sound;
    string                          sound;
    bool                            end_scene_sound;
    int                             next_p;
    bool                            is_movie;
    vector<para_content>            contents;
    vector<para_select>             selects;

    para_msg()
    {
        paragraph = next_p = 0;
        end_scene_sound = false;
        talker = scene_sound = "";
    }
};

enum xaePlaySceneState
{
    PSS_FADING_IN_BG,
    PSS_FADING_OUT_BG,
    PSS_PAUSING_TEXT,
    PSS_CAN_NEXT,
    PSS_SELECT,
    PSS_ALL_END,

    PSS_PLAYING_FLASH,
    PSS_PLAYING_MOVIE,

    PSS_READING_LOG
};

#define GUI_ELLIPSIS_ID 1000
#define GUI_MOUSE_ID    1001

class xaePlayScene : public xaeSceneObject
{
public:
    xaePlayScene(void);
    virtual ~xaePlayScene(void);

    virtual bool                    update(float fDT);
    virtual bool                    render();

    bool                            load_stage(int idx);
    void                            to_paragraph(int para);
    int                             get_next_paragraph(int para);

    para_msg*                       get_msg(int paragraph);
    para_jpg*                       get_jpg(int paragraph);

    xaeSprite*                      get_scene_sprite(string scenename);

    string                          format_text_box_string(string content);

    void                            new_stage();

private:
    void                            _init();
    void                            _parse_msg(TiXmlDocument& xml);
    void                            _parse_content(TiXmlElement* pMsgNode, para_msg* pMsg);
    void                            _parse_jpg(TiXmlDocument& xml);
    void                            _release_msg();

    void                            _update_fade_in_bg(float fDT);
    void                            _update_pause_text(float fDT);
    void                            _update_can_next(float fDT);
    void                            _update_fade_out_bg(float fDT);
    void                            _update_flash(float fDT);
    void                            _update_movie(float fDT);
    void                            _update_select(float fDT);
   
private:
    int                             m_nStageNum;
    xaeSprite*                      m_pTextBox[2];

    bool                            m_bStageLoaded;

    string                          m_szDate;
    string                          m_szWeek;
    string                          m_szWeather;

    vector<para_msg*>               m_Msg;
    vector<para_jpg*>               m_Jpg;
    map<string, xaeSprite*>         m_pScenes;

    bool                            m_bShaking;
    float                           m_fShakingLeft;
    int                             m_nShakingYOffset;

    int                             m_nCurrentParagraph;
    int                             m_nCurrentContent;
    string                          m_szCurrentScene;
    para_msg*                       m_pCurrentMsg;
    para_jpg*                       m_pCurrentJpg;
    xaeSprite*                      m_pCurrentSceneSprite;
    xaeSprite*                      m_pLaba;
    xaeSound*                       m_pCurrentSound;
    string                          m_szCurrentText;
    string                          m_szBufferText;

    xaePlaySceneState               m_emState;

    float                           m_fFadingTimer;
    float                           m_fPausingTimer;

    xaeFont*                        m_pWRYH;
    xaeFlashSprite*                 m_pFlash;
    CXadillaXVideoPlayer*           m_pVideo;

    xaePlayEffMgr                   m_PE;

    xaeGUIObject*                   m_pGUI;

    xaePlayReadLog*                 m_pReadLog;
};
#endif
