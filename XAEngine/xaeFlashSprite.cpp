#include "xaeFlashSprite.h"
#include "Flash\FlashPlayer.h"

double xaeFlashSprite::GetFlashVersion()
{
	return FlashPlayer::GetFlashVersion();
}

xaeFlashSprite::xaeFlashSprite(void) :
    m_bFromTmpFile(false),
    m_szFilename("")
{
	m_pFlashPlayer = 0;
	memset(&m_FlashQuad,0,sizeof(m_FlashQuad));
}

xaeFlashSprite::~xaeFlashSprite(void)
{
	release();
}

void xaeFlashSprite::release()
{
    if (m_FlashQuad.tex) m_pHGE->Texture_Free(m_FlashQuad.tex);
    memset(&m_FlashQuad,0,sizeof(m_FlashQuad));

    SAFEDEL(m_pFlashPlayer);

    if(m_bFromTmpFile)
    {
        DeleteFile(m_szFilename.c_str());
    }

    xaeResourceObject::release();
}

BOOL xaeFlashSprite::StartFlash(char* lpsFlash,LONG nWidth,LONG nHeight)
{
    /** 打开数据: XadillaX+ */
    string filename;
    xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
    xaeResourceBody tagRB;
    bool res = pRMO->get_resource_bin(lpsFlash, tagRB);
    /** 若已经从包裹管理器中获取了 */
    bool bPreFromTmpFile = m_bFromTmpFile;
    string szPreFilename = m_szFilename;
    if(res)
    {
        xae::Instance().create_temp_folder(XAE_FLASH_TMP_FOLDER);
        filename = xaeResourceMgrObject::save_to_file(tagRB, XAE_FLASH_TMP_FOLDER);
        if(filename == "") return false;

        m_bFromTmpFile = true;
    }
    else
    {
        m_bFromTmpFile = false;
        filename = lpsFlash;
    }

	// 释放数据
	if (m_FlashQuad.tex) m_pHGE->Texture_Free(m_FlashQuad.tex);
	memset(&m_FlashQuad,0,sizeof(m_FlashQuad));

	// 创建纹理
	HTEXTURE hTexture = m_pHGE->Texture_Create(nWidth,nHeight);
	if (hTexture == NULL)
		return FALSE;

	m_nWidth = float(nWidth);
	m_nHeight = float(nHeight);

	// 获取FLASH纹理的QUAD结构
	hgeSpriteEx spr(hTexture,0,0,m_nWidth,m_nHeight);
	spr.GetQuad(m_FlashQuad);

	// 创建FLASH播放器
	if (m_pFlashPlayer == NULL)
	{
		m_pFlashPlayer = new FlashPlayer;
		if (m_pFlashPlayer == NULL)
			return FALSE;
	}

	char sAppPath[MAX_PATH];

	if (filename.length()>=2&& filename[1]==':')
	{
		strcpy(sAppPath,filename.c_str());
	}
	else
	{
		GetModuleFileName(GetModuleHandle(NULL), sAppPath, sizeof(sAppPath));
        int i;
		for( i = (int)strlen(sAppPath) - 1; i > 0; i--)
			if(sAppPath[i]=='\\') break;
		sAppPath[i+1]=0;
		strcat(sAppPath,filename.c_str());
	}


	m_pFlashPlayer->StartAnimation(sAppPath,nWidth,nHeight,NULL);
	m_pFlashPlayer->SetQuality(FlashPlayer::QUALITY_HIGH);
	m_pFlashPlayer->GotoFrame(0);

    m_szFilename = sAppPath;

    /** 删除先去文件 */
    if(bPreFromTmpFile)
    {
        DeleteFile(szPreFilename.c_str());
    }

	return TRUE;
}
void xaeFlashSprite::SetQuality(BYTE byQuality)
{
	if (m_pFlashPlayer)
	{
		if (byQuality == 0)
			m_pFlashPlayer->SetQuality(FlashPlayer::QUALITY_LOW);
		else if (byQuality == 1)
			m_pFlashPlayer->SetQuality(FlashPlayer::QUALITY_MEDIUM);
		else
			m_pFlashPlayer->SetQuality(FlashPlayer::QUALITY_HIGH);
	}
}

BOOL xaeFlashSprite::IsPlaying()
{
	if (m_pFlashPlayer)
		return m_pFlashPlayer->IsPlaying() == true;
	return FALSE;
}
void xaeFlashSprite::Pause()
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->Pause();
}
void xaeFlashSprite::Unpause()
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->Unpause();

}
void xaeFlashSprite::Back()
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->Back();

}
void xaeFlashSprite::Rewind()
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->Rewind();

}
void xaeFlashSprite::Forward()
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->Forward();

}
void xaeFlashSprite::GotoFrame(int nFrame)
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->GotoFrame(nFrame);

}
int xaeFlashSprite::GetCurrentFrame()
{
	if (m_pFlashPlayer)
		return m_pFlashPlayer->GetCurrentFrame();
	return -1;
}
int xaeFlashSprite::GetTotalFrames()
{
	if (m_pFlashPlayer)
		return m_pFlashPlayer->GetTotalFrames();
	return -1;
}
BOOL xaeFlashSprite::GetLoopPlay()
{
	if (m_pFlashPlayer)
		return m_pFlashPlayer->GetLoopPlay();
	return FALSE;
}
void xaeFlashSprite::SetLoopPlay(BOOL bLoop)
{
	if (m_pFlashPlayer)
		m_pFlashPlayer->SetLoopPlay(bLoop);
}
BOOL xaeFlashSprite::Update(float dt)
{
	if (m_pFlashPlayer)
	{
		m_pFlashPlayer->Update();
		if(m_pFlashPlayer->Render())
		{
			int tex_w = m_pHGE->Texture_GetWidth(m_FlashQuad.tex);

			int nWidth = m_pFlashPlayer->GetWidth();
			int nHeight = m_pFlashPlayer->GetHeight();

			DWORD* dwFlashs = (DWORD*)m_pFlashPlayer->GetFlashFrameBuffer();
			DWORD* pixels = m_pHGE->Texture_Lock(m_FlashQuad.tex,FALSE,0,0,nWidth,nHeight);

			for (int y=0;y<nHeight;y++) 
				memcpy(&pixels[y*tex_w],&dwFlashs[y*nWidth],sizeof(DWORD)*nWidth);

			m_pHGE->Texture_Unlock(m_FlashQuad.tex);
			return TRUE;
		}
	}
	return FALSE;
}
BOOL xaeFlashSprite::Render(float x,float y)
{
	register float tempx1, tempy1, tempx2, tempy2;

	tempx1 = x;
	tempy1 = y;
	tempx2 = x+m_nWidth;
	tempy2 = y+m_nHeight;

	m_FlashQuad.v[0].x = tempx1; m_FlashQuad.v[0].y = tempy1;
	m_FlashQuad.v[1].x = tempx2; m_FlashQuad.v[1].y = tempy1;
	m_FlashQuad.v[2].x = tempx2; m_FlashQuad.v[2].y = tempy2;
	m_FlashQuad.v[3].x = tempx1; m_FlashQuad.v[3].y = tempy2;

	m_pHGE->Gfx_RenderQuad(&m_FlashQuad);
	return TRUE;
}
