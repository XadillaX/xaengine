#ifndef XAEPLAYREADLOG_H
#define XAEPLAYREADLOG_H
#pragma once

#include <ixaeobject.h>
#include "global.h"
#include <gui/xaeGUIButton.h>
#include "xaeSelectLabel.h"
#include <vector>
#include <string>
#include <xaeFont.h>
using namespace std;

#define LINES_PER_PAGE              7               ///< Ã¿Ò³ÐÐÊý

struct xaePlayReadLogDate
{
    string                          Date;
    string                          Week;
    string                          Weather;

    xaePlayReadLogDate()            { Date = Week = Weather = ""; }
};

class xaePlayReadLog : public iXAEObject
{
public:
    xaePlayReadLog(void);
    virtual ~xaePlayReadLog(void);

    void                            SetDate(string date, string week, string weather);
    void                            AddLine(string line);
    void                            ReloadIdxGUI();

    void                            render();
    bool                            update(float fDT);

    void                            SetCurrentPage(int nPage);
    void                            ToLastPage();

private:
    xaePlayReadLogDate              m_tagDate;
    vector<string>                  m_szContent;

    int                             m_nTotPage;
    int                             m_nCurrentPage;

    vector<int>                     m_nShowPageIdx;

    xaeGUIObject*                   m_pPageIdx;

    xaeSprite*                      m_pBG;

    xaeFont*                        m_pDateFont;
    xaeFont*                        m_pIdxFont;
    xaeFont*                        m_pContentFont;
};

#endif
