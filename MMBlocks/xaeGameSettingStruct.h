#ifndef XAEGAMESETTINGSTRUCT_H
#define XAEGAMESETTINGSTRUCT_H

struct gameSettingStruct_System
{
    string                      m_szGameTitle;
};

struct gameSettingStruct_File
{
    string                      m_szScriptFile;
    string                      m_szBGPath;
    string                      m_szBGMPath;
    string                      m_szMaidDrawPath;
    string                      m_szVoicePath;
    string                      m_szLevelPath;
    string                      m_szVideoPath;
    string                      m_szTempPath;
};

struct gameSettingStruct_Logo
{
    float                       m_fLogoFadeTimer;
    float                       m_fLogoPauseTimer;
};

struct gameSettingStruct_AVG
{
    float                       m_fTextPerTimer;
    
    int                         m_nTextboxLeft;
    int                         m_nTextboxRight;
    int                         m_nTextboxTop;
    int                         m_nTextboxBottom;

    int                         m_nTalkerLeft;
    int                         m_nTalkerRight;
    int                         m_nTalkerTop;
    DWORD                       m_dwTalkerColor;
    int                         m_nTalkerSize;

    int                         m_nTextLeft;
    int                         m_nTextTop;
    int                         m_nTextMaxPerLine;
    DWORD                       m_dwTextColor;
    int                         m_nTextSize;
};

struct gameSettingStruct_Block
{
    int                         m_nBlockWidth;
    int                         m_nBlockHeight;
    int                         m_nBlockRowCount;
    int                         m_nBlockColCount;
    int                         m_nBallLength;
    float                       m_fFlapY;
    int                         m_nFlapLength;
    float                       m_fTreasureMinSpeed;
    float                       m_fTreasureMaxSpeed;
    float                       m_fTreasureTimer;
    float                       m_fTreasureSpeedScale;
    int                         m_nTreasureProb;
    float                       m_fFlapSpeed;
    float                       m_fBallSpeed;
    string                      m_szPreBoss;
    float                       m_fMaxHorizontalRate;
};

struct gameSettingStruct
{
    gameSettingStruct_System    System;
    gameSettingStruct_File      File;
    gameSettingStruct_Logo      Logo;
    gameSettingStruct_AVG       AVG;
    gameSettingStruct_Block     Block;
};

#endif
