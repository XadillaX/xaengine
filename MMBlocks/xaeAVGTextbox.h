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

    bool                        hasTextbox();                       ///< �ı����Ƿ���ʾ

    string                      formatTextboxString(string szText); ///< ��ʽ���Ի����������

private:
    void                        __loadResource();       ///< ������Դ

private:
    xaeSprite*                  m_pTextbox;             ///< AVG�ı��򱳾�

    string                      m_szDisplayTextBuffer;  ///< ��ǰ��ʾ���ֻ���
    string                      m_szDisplayText;        ///< ��ǰ��ʵ��ʾ������
    string                      m_szCurrentTalker;      ///< ��ǰ˵����

    xaeFont*                    m_pTalkerFont;          ///< ˵��������
    xaeFont*                    m_pTextFont;            ///< ��������

    float                       m_fCharTimer;           ///< ������ʾ��ʱ��

    xaeSound*                   m_pVoice;
};

#endif
