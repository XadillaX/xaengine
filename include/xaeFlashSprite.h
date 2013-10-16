//////////////////////////////////////////////////////////////////////////////////////
// Flash¾«Áé
//////////////////////////////////////////////////////////////////////////////////////

#ifndef XAEFLASHSPRITE_H
#define XAEFLASHSPRITE_H
#include "utils.h"
#include "xae.h"
#include "xaeResourceObject.h"

class FlashPlayer;
class XAEEXP hgeSpriteEx : public hgeSprite
{
public:
	hgeSpriteEx(HTEXTURE tex, float x, float y, float w, float h):hgeSprite(tex,x,y,w,h) {};
	~hgeSpriteEx(void) {};


	void GetQuad(hgeQuad& _quad)
	{
		_quad = quad;
	}
};

class XAEEXP xaeFlashSprite : public xaeResourceObject
{
public:
	xaeFlashSprite(void);
	~xaeFlashSprite(void);

protected:
	FlashPlayer*	m_pFlashPlayer;
	hgeQuad			m_FlashQuad;
	float			m_nWidth;
	float			m_nHeight;

    bool            m_bFromTmpFile;
    string          m_szFilename;

public:
	static double			GetFlashVersion();

	BOOL					StartFlash(char* lpsFlash,LONG nWidth,LONG nHeight);
	void					SetQuality(BYTE byQuality);

	BOOL					IsPlaying();	
	void					Pause();
	void					Unpause();
	void					Back();
	void					Rewind();
	void					Forward();	
	void					GotoFrame(int nFrame);

	int						GetCurrentFrame();
	int						GetTotalFrames();

	BOOL					GetLoopPlay();
	void					SetLoopPlay(BOOL bLoop);

	BOOL					Update(float dt);
	BOOL					Render(float x,float y);

    virtual void            release();
};//class FlashSprite

#endif