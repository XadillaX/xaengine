#ifndef XAEAVGTEXTBOX_H
#define XAEAVGTEXTBOX_H

#include "global.h"
#include <xaeFont.h>
#include <string>
#include <xaeSound.h>
using namespace std;

#pragma once

class xaeAVGTextbox : iXAEObject
{
public:
    xaeAVGTextbox(void);
    virtual ~xaeAVGTextbox(void);

    void                        render();
    XAE_AVG_DISPLAY_STATUS      update(float fDT);

    void                        setTalker(string szTalker);
    void                        addText(string szText);
    void                        clearText();
    void                        setVoice(string szFilename);
    void                        clearVoice();

    bool                        hasTextbox();                       ///< 文本框是否显示

    string                      formatTextboxString(string szText); ///< 格式化对话框里的文字

private:
    void                        __loadResource();       ///< 载入资源

private:
    xaeSprite*                  m_pTextbox;             ///< AVG文本框背景

    string                      m_szDisplayTextBuffer;  ///< 当前显示文字缓存
    string                      m_szDisplayText;        ///< 当前真实显示的文字
    string                      m_szCurrentTalker;      ///< 当前说话者

    xaeFont*                    m_pTalkerFont;          ///< 说话者字体
    xaeFont*                    m_pTextFont;            ///< 正文字体

    float                       m_fCharTimer;           ///< 文字显示计时器

    xaeSound*                   m_pVoice;
};

#endif
