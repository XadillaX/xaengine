#include "CXadillaXVideoPlayer.h"
#include <windows.h>
#include <wtypes.h>
#include <atlbase.h>
#include <vector>
#include "utils.h"

#define WM_GRAPHNOTIFY              (WM_APP + 1)

std::vector<CXadillaXVideoPlayer*>  _pPlayer;
CRITICAL_SECTION                    _cs;
bool                                _csinited = false;

struct _thread_param
{
    int PlayerID;
    int PlayID;
};

DWORD ListenPlayStateThread(LPVOID lpParam)
{
    _thread_param* tp = (_thread_param*)lpParam;
    int PlayerID = tp->PlayerID;
    int PlayID = tp->PlayID;
    delete tp;

    long evCode, param1, param2;
    HRESULT hr;

    /** 进入全局临界区 */
    /** Go into the "global" critical section */
    {
        LockScope oScope(_cs);

        /** 若自己监听的播放器对象不存在 */
        /** If the player monitored by this thread not exists any more */
        if(_pPlayer[PlayerID] == NULL)
        {
            return 0;
        }

        //::EnterCriticalSection(&_pPlayer[PlayerID]->m_CS);
        HRESULT hRes = _pPlayer[PlayerID]->m_pMediaControl->Run();
        //::LeaveCriticalSection(&_pPlayer[PlayerID]->m_CS);

        //if(S_FALSE == hRes)
        //{
        //    _pPlayer[PlayerID]->__Release();

        //    ::LeaveCriticalSection(&_cs);
        //    return 0;
        //}
    }

    while(true)
    {
        {
            /** 进入全局临界区 */
            /** Go into the "global" critical section */
            LockScope oScope(_cs);

            /** 若自己监听的播放器对象不存在 */
            /** If the player monitored by this thread not exists any more */
            if(_pPlayer[PlayerID] == NULL)
            {
                return 0;
            }

            CXadillaXVideoPlayer* player = _pPlayer[PlayerID];

            LockScope oScopePlayer(player->m_CS);

            /** 若已经不再播放当前视频 */
            /** If it not plays current video any more */
            if(PlayID != player->m_nCurPlayID)
            {
                return 0;
            }

            /** 监听 */
            /** Listening... */
            if(SUCCEEDED(player->m_pEvent->GetEvent(&evCode, &param1, &param2, 0)))
            {
                hr = player->m_pEvent->FreeEventParams(evCode, param1, param2);

                if(EC_COMPLETE == evCode)
                {
                    player->m_bStopped = true;

                    if(!player->m_bLoop)
                    {
                        oScopePlayer.Unlock();
                        player->__Release();
                    }
                    else
                    {
                        oScopePlayer.Unlock();
                        if(player->LoadFile(player->m_szFilename, player->m_tagRect, player->m_hWnd, player->m_bLoop))
                        {
                            player->Play();
                        }
                    }
                    return 0;
                }
            }
        }

        Sleep(1);
    }

    return 0;
}

CXadillaXVideoPlayer::CXadillaXVideoPlayer() :
    m_bPlaying(false),
    m_bStopped(false),
    m_bLoop(false),
    m_bLoaded(false),
    m_nPlayID(0),
    m_nCurPlayID(-1),
    m_hThreadHandle(0),
    m_hWnd(0),
    m_pGraph(NULL),
    m_pVidWnd(NULL),
    m_pMediaControl(NULL),
    m_pEvent(NULL)
{
    ::InitializeCriticalSection(&m_CS);

    /** 播放器全局临界区 */
    /** "global" critical section */
    if(!_csinited)
    {
        _csinited = true;
        ::InitializeCriticalSection(&_cs);
    }

    /** 播放器加入播放器列表 */
    /** Add this player to the player list */
    LockScope oScope(_cs);
    _pPlayer.push_back(this);
    m_nPlayerID = _pPlayer.size() - 1;
}

CXadillaXVideoPlayer::~CXadillaXVideoPlayer()
{
    /** 进入全局临界区 */
    /** Go into the "global" critical section */
    LockScope oScope(_cs);

    /** 从播放器列表中删除自己（线程中会用到） */
    /** delete itself from the player list. (because it will be used by thread) */
    _pPlayer[m_nPlayerID] = NULL;

    /** 释放资源 */
    /** release resources */
    __Release();

    /** 退出全局临界区 */
    /** leave critical section */
}

void CXadillaXVideoPlayer::__Release()
{
    LockScope oScope(m_CS);
    if(m_pVidWnd != NULL)
    {
        m_pVidWnd->put_Visible(OAFALSE);
    }

    if(m_pGraph != NULL)
    {
        m_pGraph->Release();
    }

    if(m_pMediaControl != NULL)
    {
        m_pMediaControl->Release();
        m_bPlaying = false;
    }

    if(m_pVidWnd)
    {
        m_pVidWnd->Release();
    }

    if(m_pEvent)
    {
        m_pEvent->Release();
    }

    m_pGraph = NULL;
    m_pVidWnd = NULL;
    m_pMediaControl = NULL;
    m_pEvent = NULL;

    m_bPlaying = false;
    m_bStopped = true;
    m_bLoaded = false;
    m_nCurPlayID = -1;

    //::CloseHandle(m_hThreadHandle);
}

bool CXadillaXVideoPlayer::Stop()
{
    {
        LockScope oScope(m_CS);

        /** 若未载入 */
        /** If not loaded */
        if(!m_bLoaded)
        {
            return false;
        }

        /** 若已暂停 */
        /** If paused of stopped */
        if(m_bStopped)
        {
            return true;
        }

        if(m_pMediaControl == NULL)
        {
            return true;
        }
        m_nCurPlayID = -1;
    }

    __Release();
    return true;
}

bool CXadillaXVideoPlayer::Pause()
{
    LockScope oScope(m_CS);
    /** 若未载入 */
    /** If not loaded */
    if(!m_bLoaded)
    {
        return false;
    }

    if(m_pMediaControl != NULL)
    {
        m_bPlaying = false;
        m_pMediaControl->Pause();
    }

    return true;
}

bool CXadillaXVideoPlayer::Play()
{
    LockScope oScope(m_CS);

    /** 若未载入 */
    /** If not loaded */
    if(!m_bLoaded)
    {
        return false;
    }

    if(m_pMediaControl != NULL)
    {
        m_bPlaying = true;
        //if(S_FALSE == hRes)
        //{
        //    ::LeaveCriticalSection(&m_CS);
        //    return false;
        //}

        /** 创建监听线程 */
        /** create the listening thread */
        m_nPlayID++;
        m_nCurPlayID = m_nPlayID;

        _thread_param* tp = new _thread_param();
        tp->PlayerID = m_nPlayerID;
        tp->PlayID = m_nPlayID;

        DWORD dwThreadID;
        m_hThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListenPlayStateThread,
            (LPVOID)tp, 0, &dwThreadID);

        return true;
    }

    return false;
}

bool CXadillaXVideoPlayer::LoadFile(const char* filename, RECT rect, HWND hWnd, bool bLoop)
{
    /** 释放上一次资源 */
    /** release the resource of previous time */
    __Release();

    LockScope oScope(m_CS);

    /** 创建Graph */
    /** create the graph */
    CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
        IID_IGraphBuilder, (void**)&m_pGraph);

    /** 初始化m_pGraph */
    /** init the graph */
    m_pGraph->QueryInterface(IID_IVideoWindow, (void**)&m_pVidWnd);
    m_pGraph->QueryInterface(IID_IMediaEventEx, (void**)&m_pEvent);
    m_pEvent->SetNotifyWindow((OAHWND)hWnd, WM_GRAPHNOTIFY, 0);
    m_pGraph->QueryInterface(IID_IMediaControl, (void**)&m_pMediaControl);

    /** 载入文件 */
    /** load file */
    m_hWnd = hWnd;
    WCHAR szFilename[1024];
    memcpy(szFilename, CA2WEX<1024>(filename), sizeof(szFilename));
    HRESULT hRes = m_pGraph->RenderFile(szFilename, NULL);

    /** 载入失败 */
    /** load failed */
    if(hRes != S_OK && hRes != VFW_S_AUDIO_NOT_RENDERED && hRes != VFW_S_DUPLICATE_NAME &&
        hRes != VFW_S_PARTIAL_RENDER && hRes != VFW_S_VIDEO_NOT_RENDERED)
    {
        m_bLoaded = false;

        m_pVidWnd->put_Visible(OAFALSE);
        m_pGraph->Release();
        m_pMediaControl->Release();
        m_pVidWnd->Release();
        m_pEvent->Release();

        m_pVidWnd = NULL;
        m_pGraph = NULL;
        m_pMediaControl = NULL;
        m_pEvent = NULL;

        return false;
    }

    /** 播放窗口位置 */
    /** position of the player window */
    m_pVidWnd->SetWindowPosition(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    m_pVidWnd->put_Owner((OAHWND)hWnd);
    m_pVidWnd->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

    m_bPlaying = true;
    m_bStopped = false;
    m_bLoop = bLoop;
    m_tagRect = rect;
    strcpy(m_szFilename, filename);
    m_bLoaded = true;
    m_nCurPlayID = -1;

    /** 刷新窗口，解决播放时黑屏问题 */
    GetClientRect(hWnd, &rect);
    InvalidateRect(hWnd, &rect, TRUE);

    return true;
}

bool CXadillaXVideoPlayer::IsLoaded()
{
    LockScope oScope(m_CS);
    bool rst = m_bLoaded;
    return rst;
}

bool CXadillaXVideoPlayer::IsPlaying()
{
    LockScope oScope(m_CS);
    bool rst = m_bPlaying;
    return rst;
}
