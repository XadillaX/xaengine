//////////////////////////////////////////////////////////////////////////
//
//	视频播放类 Rewrite version
//
//	Program by 死月(XadillaX) (admin@xcoder.in)
//
//////////////////////////////////////////////////////////////////////////
#ifndef CXADILLAXVIDEOPLAYER_H
#define CXADILLAXVIDEOPLAYER_H

#include <tchar.h>
#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "quartz.lib")

#pragma once
class CXadillaXVideoPlayer
{
    friend DWORD ListenPlayStateThread(LPVOID lpParam);

public:
    CXadillaXVideoPlayer(void);
    virtual ~CXadillaXVideoPlayer(void);

    bool                                LoadFile(const char* filename, RECT rect, HWND hWnd, bool bLoop = false);
    bool                                Play();
    bool                                Pause();
    bool                                Stop();

    bool                                IsLoaded();
    bool                                IsPlaying();

private:
    void                                __Release();

private:
    int                                 m_nCurPlayID;
    int                                 m_nPlayID;
    int                                 m_nPlayerID;

    bool                                m_bPlaying;
    bool                                m_bStopped;
    bool                                m_bLoop;
    bool                                m_bLoaded;

    HANDLE                              m_hThreadHandle;
    HWND                                m_hWnd;

    IGraphBuilder*                      m_pGraph;
    IVideoWindow*                       m_pVidWnd;
    IMediaControl*                      m_pMediaControl;
    IMediaEventEx*                      m_pEvent;

    char                                m_szFilename[512];
    RECT                                m_tagRect;

    CRITICAL_SECTION                    m_CS;
};

#endif
