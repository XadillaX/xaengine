#ifndef FLASHPLAYER_H
#define FLASHPLAYER_H
#pragma once
#include "../utils.h"
#include "../xae.h"
#include "ShockwaveFlash.h"
#include <string>

class XAEEXP FlashListener
{
public:
	virtual void FlashAnimEnded() {}
	virtual void FlashCommand(const std::string& theCommand, const std::string& theParam) { };
};

class FlashSink;
class ControlSite;
class XAEEXP FlashPlayer :
	public FlashListener
{
	friend class FlashSink;
	friend class ControlSite;
public:
	// FLASH 播放状态
	typedef enum tagFlashState
	{
		STATE_IDLE,
		STATE_PLAYING,
		STATE_STOPPED
	}FLASHSTATE;

	// FLASH 播放品质
	typedef enum tagFlashQuality
	{
		QUALITY_LOW,
		QUALITY_MEDIUM,
		QUALITY_HIGH
	}FLASHQUALITY;

protected:
	FLASHSTATE				m_State;

	HMODULE					m_hFlashLibHandle;					//flash.ocx

	ControlSite*			m_pControlSite;						//控制台
	FlashSink*				m_pFlashSink;						//事件接收器
	ShockwaveFlashObjects::IShockwaveFlash* m_pFlashInterface;	//flash接口
	IOleObject*				m_pOleObject;						//OLE对象指针
	IOleInPlaceObjectWindowless* m_pWindowlessObject;			//

	int						m_nCOMCount;						//COM计数器
	int						m_nPauseCount;
	BOOL					m_bFlashDirty;
	RECT					m_rcFlashDirty;

	LONG					m_nWidth;
	LONG					m_nHeight;

	HWND					m_hWnd;
	HDC						m_hFlashDC;
	HBITMAP					m_hBitmap;
	void*					m_pFrameBuffer;

	int						m_nLastFrame;

private:
	void					CleanupBuffer();
	void					RebuildBuffer();

public:
	LONG					GetWidth();
	LONG					GetHeight();

	HDC						GetFlashDC();
	HBITMAP					GetFlashBitmap();
	void*					GetFlashFrameBuffer();

public:
	FlashPlayer();
	~FlashPlayer();

	static double			GetFlashVersion();

	bool					StartAnimation(const std::string& theFileName,LONG nWidth,LONG nHeight,HWND hWnd);	
	void					SetQuality(FLASHQUALITY theQuality);

	bool					IsPlaying();	
	void					Pause();
	void					Unpause();
	void					Back();
	void					Rewind();
	void					Forward();	
	void					GotoFrame(int theFrameNum);

	int						GetCurrentFrame();
	int						GetTotalFrames();
	std::string				GetCurrentLabel(const std::string& theTimeline);

	LONG					GetBackgroundColor();
	void					SetBackgroundColor(LONG theColor);

	BOOL					GetLoopPlay();
	void					SetLoopPlay(BOOL bLoop);

	void					CallFrame(const std::string& theTimeline, int theFrameNum);
	void					CallLabel(const std::string& theTimeline, const std::string& theLabel);

	std::string				GetVariable(const std::string& theName);
	void					SetVariable(const std::string& theName, const std::string& theValue);

	BOOL					Update();
	BOOL					Render();
	BOOL					Render(HDC hDC);

	void					MouseMove(int x, int y);
	void					MouseLButtonDown(int x, int y);		
	void					MouseLButtonUp(int x, int y);

	void					SaveFlashBitmapBuffer(char* lpsFileName);
};
#endif