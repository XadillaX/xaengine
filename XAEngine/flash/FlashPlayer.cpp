#include "flash/FlashPlayer.h"

//////////////////////////////////////////////////////////////////////////
// FlashSink - Receives flash events   
class FlashSink : public ShockwaveFlashObjects::_IShockwaveFlashEvents
{
public:	
	LPCONNECTIONPOINT		m_ConnectionPoint;	
	DWORD					m_dwCookie;
	int						m_nRefCount;
	FlashPlayer*			m_pFlashWidget;

public:
	FlashSink()
	{		
		m_dwCookie = 0;
		m_ConnectionPoint = NULL;
		m_nRefCount = 0;
		m_pFlashWidget = NULL;		
	}

	virtual ~FlashSink()
	{

	}

	HRESULT Init(FlashPlayer* theFlashWidget)
	{
		m_pFlashWidget = theFlashWidget;
		m_pFlashWidget->m_nCOMCount++;

		HRESULT aResult = NOERROR;
		LPCONNECTIONPOINTCONTAINER aConnectionPoint = NULL;

		if ((m_pFlashWidget->m_pFlashInterface->QueryInterface(IID_IConnectionPointContainer, (void**) &aConnectionPoint) == S_OK) &&
			(aConnectionPoint->FindConnectionPoint(__uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents), &m_ConnectionPoint) == S_OK))			
		{
			IDispatch* aDispatch = NULL;
			QueryInterface(__uuidof(IDispatch), (void**) &aDispatch);
			if (aDispatch != NULL)
			{
				aResult = m_ConnectionPoint->Advise((LPUNKNOWN)aDispatch, &m_dwCookie);
				aDispatch->Release();
			}
		}

		if (aConnectionPoint != NULL) 
			aConnectionPoint->Release();

		return aResult;
	}

	HRESULT Shutdown()
	{
		HRESULT aResult = S_OK;

		if (m_ConnectionPoint)
		{
			if (m_dwCookie)
			{
				aResult = m_ConnectionPoint->Unadvise(m_dwCookie);
				m_dwCookie = 0;
			}

			m_ConnectionPoint->Release();
			m_ConnectionPoint = NULL;
		}

		return aResult;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		*ppv = NULL;

		if (riid == IID_IUnknown)
		{
			*ppv = (LPUNKNOWN)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IDispatch)
		{
			*ppv = (IDispatch*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents))
		{
			*ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
			AddRef();
			return S_OK;
		}
		else
		{   
			return E_NOTIMPL;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{  
		return ++m_nRefCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{  
		int aRefCount = --m_nRefCount;
		if (aRefCount == 0)		
			delete this;		

		return aRefCount;
	}

	// IDispatch method
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(UINT* pctinfo)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo** ppTInfo)
	{
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(REFIID riid, LPOLESTR* rgszNames,
		UINT cNames, LCID lcid,DISPID* rgDispId)
	{
		return E_NOTIMPL; 
	}

	virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid,
		WORD wFlags, ::DISPPARAMS __RPC_FAR *pDispParams, VARIANT __RPC_FAR *pVarResult,
		::EXCEPINFO __RPC_FAR *pExcepInfo, UINT __RPC_FAR *puArgErr)
	{
		switch(dispIdMember)
		{
		case 0x7a6:			
			break;
		case 0x96:			
			if ((pDispParams->cArgs == 2) &&
				(pDispParams->rgvarg[0].vt == VT_BSTR) &&
				(pDispParams->rgvarg[1].vt == VT_BSTR))
			{
				FSCommand(pDispParams->rgvarg[1].bstrVal, pDispParams->rgvarg[0].bstrVal);
			}
			break;
		case DISPID_READYSTATECHANGE:					
			break;
		default:			
			return DISP_E_MEMBERNOTFOUND;
		} 

		return NOERROR;
	}

	HRESULT OnReadyStateChange (long newState)
	{	
		return S_OK;
	}

	HRESULT OnProgress(long percentDone )
	{		
		return S_OK;
	}

	HRESULT FSCommand (_bstr_t command, _bstr_t args)
	{
		if (m_pFlashWidget != NULL)
			m_pFlashWidget->FlashCommand((char*) command, (char*) args);
		return S_OK;
	}	
};

//////////////////////////////////////////////////////////////////////////
// ControlSite - container that holds the flash control

class ControlSite : 	
	public IOleInPlaceSiteWindowless, 
	public IOleClientSite	
{
public:
	int						m_nRefCount;
	FlashPlayer*			m_pFlashWidget;

public:
	ControlSite()
	{		
		m_nRefCount = 0;		
		m_pFlashWidget = NULL;
	}	

	virtual ~ControlSite()
	{
		if (m_pFlashWidget != NULL)
			m_pFlashWidget->m_nCOMCount--;
	}

	void Init(FlashPlayer* theFlashWidget)
	{
		m_pFlashWidget = theFlashWidget;
		m_pFlashWidget->m_nCOMCount++;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, LPVOID* ppv)
	{
		*ppv = NULL;

		if (riid == IID_IUnknown)
		{
			*ppv = (IUnknown*) (IOleWindow*) this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleWindow)
		{
			*ppv = (IOleWindow*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleInPlaceSite)
		{
			*ppv = (IOleInPlaceSite*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleInPlaceSiteEx)
		{
			*ppv = (IOleInPlaceSiteEx*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleInPlaceSiteWindowless)
		{
			*ppv = (IOleInPlaceSiteWindowless*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == IID_IOleClientSite)
		{
			*ppv = (IOleClientSite*)this;
			AddRef();
			return S_OK;
		}
		else if (riid == __uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents))
		{
			*ppv = (ShockwaveFlashObjects::_IShockwaveFlashEvents*) this;
			AddRef();
			return S_OK;
		}
		else
		{   
			return E_NOTIMPL;
		}
	}

	ULONG STDMETHODCALLTYPE AddRef()
	{  
		return ++m_nRefCount;
	}

	ULONG STDMETHODCALLTYPE Release()
	{ 
		int aRefCount = --m_nRefCount;
		if (aRefCount == 0)		
			delete this;		

		return aRefCount;
	}

	//////////////////////////////////////////////////////////////////////////	

	virtual HRESULT  STDMETHODCALLTYPE SaveObject(void)
	{
		return S_OK;
	}

	virtual HRESULT  STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,IMoniker** ppmk )
	{
		*ppmk = NULL;
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer ** theContainerP)
	{
		//return QueryInterface(__uuidof(IOleContainer), (void**) theContainerP);				
		return E_NOINTERFACE;
	}

	virtual HRESULT  STDMETHODCALLTYPE ShowObject(void)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT  STDMETHODCALLTYPE OnShowWindow(BOOL)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT  STDMETHODCALLTYPE RequestNewObjectLayout(void)
	{
		return E_NOTIMPL;
	}

	HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(/* [in] */ BOOL fEnterMode)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetWindow(/* [out] */ HWND __RPC_FAR* theWnndow)
	{
		return E_FAIL;
	}

	HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)
	{		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnUIActivate(void)
	{		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetWindowContext(/* [out] */ IOleInPlaceFrame __RPC_FAR *__RPC_FAR *ppFrame, /* [out] */ IOleInPlaceUIWindow __RPC_FAR *__RPC_FAR *ppDoc, /* [out] */ LPRECT lprcPosRect, /* [out] */ LPRECT lprcClipRect, /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo)
	{
		RECT aRect = m_pFlashWidget->m_rcFlashDirty;

		*lprcPosRect = aRect;
		*lprcClipRect = aRect;

		*ppFrame = NULL;
		QueryInterface(__uuidof(IOleInPlaceFrame), (void**) ppFrame);		
		*ppDoc = NULL;

		lpFrameInfo->fMDIApp = FALSE;
		lpFrameInfo->hwndFrame = NULL;
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Scroll(/* [in] */ SIZE scrollExtant)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnUIDeactivate(/* [in] */ BOOL fUndoable)
	{		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)
	{	
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE DiscardUndoState(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnPosRectChange(/* [in] */ LPCRECT lprcPosRect)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnInPlaceActivateEx(/* [out] */ BOOL __RPC_FAR *pfNoRedraw, /* [in] */ DWORD dwFlags)
	{		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnInPlaceDeactivateEx(/* [in] */ BOOL fNoRedraw)
	{		
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE RequestUIActivate(void)
	{
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE CanWindowlessActivate(void)
	{
		// Allow windowless activation?
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetCapture(void)
	{
		// TODO capture the mouse for the object
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE SetCapture(/* [in] */ BOOL fCapture)
	{
		// TODO capture the mouse for the object
		return S_FALSE;
	}

	HRESULT STDMETHODCALLTYPE GetFocus(void)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE SetFocus(/* [in] */ BOOL fFocus)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE GetDC(/* [in] */ LPCRECT pRect, /* [in] */ DWORD grfFlags, /* [out] */ HDC __RPC_FAR *phDC)
	{		
		return E_INVALIDARG;		
	}

	HRESULT STDMETHODCALLTYPE ReleaseDC(/* [in] */ HDC hDC)
	{
		return E_INVALIDARG;
	}

	HRESULT STDMETHODCALLTYPE InvalidateRect(/* [in] */ LPCRECT pRect, /* [in] */ BOOL fErase)
	{	
		if (pRect == NULL)
		{
			m_pFlashWidget->m_bFlashDirty = true;
		}
		else if (!m_pFlashWidget->m_bFlashDirty)
		{
			m_pFlashWidget->m_bFlashDirty = true;
		}
		else
		{			
			
		}		

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE InvalidateRgn(/* [in] */ HRGN hRGN, /* [in] */ BOOL fErase)
	{	
		m_pFlashWidget->m_bFlashDirty = true;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ScrollRect(/* [in] */ INT dx, /* [in] */ INT dy, /* [in] */ LPCRECT pRectScroll, /* [in] */ LPCRECT pRectClip)
	{
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE AdjustRect(/* [out][in] */ LPRECT prc)
	{
		if (prc == NULL)
		{
			return E_INVALIDARG;
		}

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE OnDefWindowMessage(/* [in] */ UINT msg, /* [in] */ WPARAM wParam, /* [in] */ LPARAM lParam, /* [out] */ LRESULT __RPC_FAR *plResult)
	{
		return S_FALSE;
	}
};

//////////////////////////////////////////////////////////////////////////
FlashPlayer::FlashPlayer()
{
	HRESULT aResult;

	m_State = STATE_IDLE;

	m_hFlashLibHandle = NULL;

	m_pControlSite = NULL;
	m_pFlashSink = NULL;
	m_pFlashInterface = NULL;
	m_pOleObject = NULL;
	m_pWindowlessObject = NULL;

	m_nCOMCount = 0;
	m_nPauseCount = 0;
	m_bFlashDirty = TRUE;	

	m_nWidth = 0;
	m_nHeight = 0;

	m_hFlashDC = NULL;
	m_hBitmap = NULL;
	m_pFrameBuffer = NULL;

	CoInitialize(NULL);

	m_pControlSite = new ControlSite();
	m_pControlSite->AddRef();	
	m_pControlSite->Init(this);

	m_hFlashLibHandle = LoadLibraryA("flash.ocx");
	if (m_hFlashLibHandle != NULL)
	{
		IClassFactory* aClassFactory = NULL;
		DllGetClassObjectFunc aDllGetClassObjectFunc = (DllGetClassObjectFunc) GetProcAddress(m_hFlashLibHandle, "DllGetClassObject");
		aResult = aDllGetClassObjectFunc(CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&aClassFactory);
		aClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_pOleObject);
		aClassFactory->Release();	
	}
	else
	{
		CoCreateInstance(CLSID_ShockwaveFlash, NULL, 
			CLSCTX_INPROC_SERVER,
			IID_IOleObject,
			(void**)&m_pOleObject);
	}

	IOleClientSite* aClientSite = NULL;
	m_pControlSite->QueryInterface(__uuidof(IOleClientSite), (void**) &aClientSite);
	m_pOleObject->SetClientSite(aClientSite);	

	m_pOleObject->QueryInterface(__uuidof(IShockwaveFlash), (LPVOID*) &m_pFlashInterface);
	_bstr_t aTrans = "Transparent";
	m_pFlashInterface->put_WMode(aTrans);

	aResult = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, aClientSite, 0, NULL, NULL);
	aClientSite->Release();	

	m_pOleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (LPVOID*) &m_pWindowlessObject);

	m_pFlashSink = new FlashSink();
	m_pFlashSink->AddRef();	
	m_pFlashSink->Init(this);
}

FlashPlayer::~FlashPlayer()
{
	CleanupBuffer();

	if (m_pWindowlessObject != NULL)
		m_pWindowlessObject->Release();
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->Release();	

	if (m_pFlashSink != NULL)
	{
		m_pFlashSink->Shutdown();
		m_pFlashSink->Release();
	}

	m_pOleObject->Close(OLECLOSE_NOSAVE);

	if (m_pOleObject != NULL)
		m_pOleObject->Release();	

	if (m_pControlSite != NULL)
		m_pControlSite->Release();

	// Make sure all our COM objects were actually destroyed
	//DBG_ASSERTE(m_nCOMCount == 0);

	if (m_hFlashLibHandle != NULL)
		FreeLibrary(m_hFlashLibHandle);	
}

double FlashPlayer::GetFlashVersion()
{
	CoInitialize(NULL);

	IOleObject* anOleObject = NULL;
	if (FAILED(CoCreateInstance(CLSID_ShockwaveFlash, NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IOleObject,
		(void**) &anOleObject)))
		return 0.0;	

	IShockwaveFlash* aFlashInterface = NULL;
	if (FAILED(anOleObject->QueryInterface(__uuidof(IShockwaveFlash), (LPVOID*) &aFlashInterface)))
		return 0.0;

	long aVersion = 0;
	aFlashInterface->FlashVersion(&aVersion);

	aFlashInterface->Release();
	anOleObject->Release();

	return aVersion / 65536.0;
}

void FlashPlayer::CleanupBuffer()
{
	if (m_hBitmap != NULL)
		DeleteObject(m_hBitmap);

	if (m_hFlashDC != NULL)
		DeleteDC(m_hFlashDC);
}

void FlashPlayer::RebuildBuffer()
{
	CleanupBuffer();

	HDC desktop_dc = ::GetDC(m_hWnd);
	m_hFlashDC = ::CreateCompatibleDC(desktop_dc);

	BITMAPINFO bmpinfo;
	memset(&bmpinfo,0, sizeof(bmpinfo));
	bmpinfo.bmiHeader.biSize = sizeof(bmpinfo);
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 32;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biWidth = m_nWidth;
	bmpinfo.bmiHeader.biHeight = -m_nHeight;

	m_hBitmap = CreateDIBSection(m_hFlashDC, &bmpinfo, DIB_RGB_COLORS, &m_pFrameBuffer, NULL, 0);
	SelectObject(m_hFlashDC, m_hBitmap);

	// vietdoor's code start here
	::SetMapMode(m_hFlashDC, MM_TEXT);
	// vietdoor's code end here

	::SetBkColor(m_hFlashDC,RGB(255,255,255));
	::SetBkMode(m_hFlashDC,TRANSPARENT);
}

LONG FlashPlayer::GetWidth()
{
	return m_nWidth;
}

LONG FlashPlayer::GetHeight()
{
	return m_nHeight;
}

HDC FlashPlayer::GetFlashDC()
{
	return m_hFlashDC;
}

HBITMAP FlashPlayer::GetFlashBitmap()
{
	return m_hBitmap;
}

void* FlashPlayer::GetFlashFrameBuffer()
{
	return m_pFrameBuffer;
}

bool FlashPlayer::StartAnimation(const std::string& theFileName,LONG nWidth,LONG nHeight,HWND hWnd)
{
	//std::string aFullPath = GetFullPath(theFileName);
	std::string aFullPath = theFileName;

	m_hWnd = hWnd;

	_bstr_t bstr((char*) aFullPath.c_str());		
	m_pFlashInterface->put_Movie(bstr); // you have to change the path here	
	if ((m_nPauseCount == 0) && (m_pFlashInterface->Play() != S_OK))
		return false;

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	SetRect(&m_rcFlashDirty,0,0,m_nWidth,m_nHeight);

	RebuildBuffer();


	IOleInPlaceObject* anInPlaceObject = NULL;	
	m_pOleObject->QueryInterface(__uuidof(IOleInPlaceObject), (LPVOID*) &anInPlaceObject);			

	if (anInPlaceObject != NULL)
	{
		anInPlaceObject->SetObjectRects(&m_rcFlashDirty, &m_rcFlashDirty);
		anInPlaceObject->Release();
	}

	m_State = STATE_PLAYING;
	m_nLastFrame = -1;
	return true;
}

void FlashPlayer::SetQuality(FLASHQUALITY theQuality)
{
	static char* aQualityNames[3] = {"Low", "Medium", "High"};

	_bstr_t aNewStr = aQualityNames[theQuality];
	m_pFlashInterface->put_Quality2(aNewStr);
}

bool FlashPlayer::IsPlaying()
{
	VARIANT_BOOL aBool = 0;
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->IsPlaying(&aBool);
	return aBool != 0;
}

void FlashPlayer::Pause()
{
	m_nPauseCount++;

	if (m_State != STATE_STOPPED)
		m_State = STATE_IDLE;

	if ((m_nPauseCount == 1) && (m_pFlashInterface != NULL) && (m_State != STATE_STOPPED))
		m_pFlashInterface->StopPlay();
}

void FlashPlayer::Unpause()
{
	m_nPauseCount--;
	if ((m_nPauseCount == 0) && (m_pFlashInterface != NULL) && (m_State != STATE_STOPPED))
	{
		m_State = STATE_PLAYING;
		m_pFlashInterface->Play();
	}
}

void FlashPlayer::Back()
{
	if (m_pFlashInterface != NULL)
	{
		m_pFlashInterface->Back();
		m_pFlashInterface->Play();
	}
}

void FlashPlayer::Rewind()
{
	if (m_pFlashInterface != NULL)
	{
		m_pFlashInterface->Rewind();
		m_pFlashInterface->Play();
	}
}

void FlashPlayer::Forward()
{
	if (m_pFlashInterface != NULL)
	{
		m_pFlashInterface->Forward();
		m_pFlashInterface->Play();
	}
}

void FlashPlayer::GotoFrame(int theFrameNum)
{
	if (m_pFlashInterface != NULL)
	{
		m_pFlashInterface->GotoFrame(theFrameNum);
		m_pFlashInterface->Play();
	}
}

int FlashPlayer::GetCurrentFrame()
{	
	long aCurrentFrame = -1;
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->CurrentFrame(&aCurrentFrame);
	return aCurrentFrame;
}

int FlashPlayer::GetTotalFrames()
{
	long aTotalFrame = -1;
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->get_TotalFrames(&aTotalFrame);
	return aTotalFrame;
}

std::string FlashPlayer::GetCurrentLabel(const std::string& theTimeline)
{
	BSTR aBStr = L"";
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->TCurrentLabel(_bstr_t(theTimeline.c_str()), &aBStr);
	return (const char*) _bstr_t(aBStr);
}

LONG FlashPlayer::GetBackgroundColor()
{
	long aBackgroundColor = 0;
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->get_BackgroundColor(&aBackgroundColor);
	return aBackgroundColor;

}
void FlashPlayer::SetBackgroundColor(LONG theColor)
{
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->put_BackgroundColor(theColor);
}

BOOL FlashPlayer::GetLoopPlay()
{
	short aLoop = 0;
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->get_Loop(&aLoop);
	return aLoop==1;
}

void FlashPlayer::SetLoopPlay(BOOL bLoop)
{
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->put_Loop(bLoop==TRUE);
}

void FlashPlayer::CallFrame(const std::string& theTimeline, int theFrameNum)
{
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->TCallFrame(_bstr_t(theTimeline.c_str()), theFrameNum);
}

void FlashPlayer::CallLabel(const std::string& theTimeline, const std::string& theLabel)
{
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->TCallLabel(_bstr_t(theTimeline.c_str()), _bstr_t(theLabel.c_str()));
}

std::string FlashPlayer::GetVariable(const std::string& theName)
{
	BSTR aBStr = L"";
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->GetVariable(_bstr_t(theName.c_str()), &aBStr);
	return (const char*) _bstr_t(aBStr);
}

void FlashPlayer::SetVariable(const std::string& theName, const std::string& theValue)
{
	if (m_pFlashInterface != NULL)
		m_pFlashInterface->SetVariable(_bstr_t(theName.c_str()), _bstr_t(theValue.c_str()));
}

BOOL FlashPlayer::Update()
{
	if (m_State == STATE_PLAYING)
	{
		VARIANT_BOOL isPlaying = 0;
		m_pFlashInterface->IsPlaying(&isPlaying);	
		if (!isPlaying)
		{
			m_State = STATE_STOPPED;
			if (m_nPauseCount == 0)
				FlashAnimEnded();
		}
	}
	return TRUE;
}

BOOL FlashPlayer::Render()
{
	if (m_bFlashDirty && Render(m_hFlashDC))
	{
		m_bFlashDirty = FALSE;
		return TRUE;
	}

	return FALSE;
}

BOOL FlashPlayer::Render(HDC hDC)
{
	IViewObject* aViewObject = NULL;
	m_pFlashInterface->QueryInterface(IID_IViewObject, (LPVOID*) &aViewObject);
	if (aViewObject != NULL)
	{
		RECT aFill = {0, 0, m_nWidth, m_nHeight};
		FillRect(m_hFlashDC,&aFill,(HBRUSH)GetStockObject(WHITE_BRUSH));

		RECTL aRect = {0, 0, m_nWidth, m_nHeight};

		HRESULT aResult = aViewObject->Draw(DVASPECT_CONTENT, 1, NULL, NULL, NULL, hDC, &aRect, NULL, NULL, NULL);

		aViewObject->Release();

		return aResult == S_OK;
	}
	return FALSE;
}

void FlashPlayer::MouseMove(int x, int y)
{	
	LRESULT aResult;
	m_pWindowlessObject->OnWindowMessage(WM_MOUSEMOVE, 0, MAKELPARAM(x, y), &aResult);
}

void FlashPlayer::MouseLButtonDown(int x, int y)
{
	LRESULT aResult;
	m_pWindowlessObject->OnWindowMessage(WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(x, y), &aResult);
}

void FlashPlayer::MouseLButtonUp(int x, int y)
{
	LRESULT aResult;
	m_pWindowlessObject->OnWindowMessage(WM_LBUTTONUP, 0, MAKELPARAM(x, y), &aResult);
}

void FlashPlayer::SaveFlashBitmapBuffer(char* lpsFileName)
{
	if (m_pFrameBuffer == NULL)
		return ;

	int size = 32 / 8 * m_nWidth * m_nHeight;

	int fhsize = sizeof(BITMAPFILEHEADER);
	int bisize = sizeof(BITMAPINFOHEADER);

	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER info;

	memset ( &bmfh, 0, sizeof ( BITMAPFILEHEADER ) );
	memset ( &info, 0, sizeof ( BITMAPINFOHEADER ) );

	// First we fill the file header with data
	bmfh.bfType				= 0x4d42; // 0x4d42 = 'BM'
	bmfh.bfReserved1		= 0;
	bmfh.bfReserved2		= 0;
	bmfh.bfSize				= sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + size;
	bmfh.bfOffBits			= 0x36;  // (54) size of headers

	info.biSize				= sizeof ( BITMAPINFOHEADER );
	info.biWidth			= m_nWidth;
	info.biHeight			= m_nHeight;
	info.biPlanes			= 1;
	info.biBitCount			= 32;
	info.biCompression		= BI_RGB;
	info.biSizeImage		= 0;
	info.biXPelsPerMeter	= 0x0ec4;    
	info.biYPelsPerMeter	= 0x0ec4;
	info.biClrUsed			= 0;
	info.biClrImportant		= 0;

	BYTE* pBitmapFlashBuffer = new BYTE[fhsize + bisize + size];
	if (pBitmapFlashBuffer)
	{
		memcpy(pBitmapFlashBuffer, &bmfh, fhsize);
		memcpy(pBitmapFlashBuffer + fhsize, &info, bisize);
		memcpy(pBitmapFlashBuffer + fhsize + bisize, m_pFrameBuffer, size);

		FILE *pFile = fopen(lpsFileName,"wb");
		if (pFile)
		{
			fwrite(pBitmapFlashBuffer,1,fhsize + bisize + size,pFile);
			fclose(pFile);
		}

		delete [] pBitmapFlashBuffer;
	}
}