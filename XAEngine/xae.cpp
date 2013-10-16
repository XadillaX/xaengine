/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * System layer API: IMPL
 */
#include <xae.h>
#include <xaeSceneMgr.h>

extern bool __framefunc();
extern bool __renderfunc();

xae::xae()
{
    m_pResMgr = new xaeResourceMgrObject();
    m_szTmpFolders.clear();
    m_bNoMoreRender = false;
}

xae::~xae()
{
    delete_all_temp_folders();
    if(NULL != m_pHGE) m_pHGE->Release();
    SAFEDEL(m_pResMgr);
}

void xae::alert(const char* msg, const char* title /* = "XAE消息" */, int icon /* = MB_ICONINFORMATION */, bool console_only /* = false */)
{
    printf("[%d][\"%s\"]\n[%s]\n", icon, title, msg);
    if(!console_only)
    {
        assert(NULL != m_pHGE);
        MessageBox(m_pHGE->System_GetState(HGE_HWND), msg, title, icon);
    }
}

HGE* xae::create_core()
{
    m_pHGE = hgeCreate(HGE_VERSION);
    assert(NULL != m_pHGE);

    m_pHGE->System_SetState(HGE_FRAMEFUNC, __framefunc);
    m_pHGE->System_SetState(HGE_RENDERFUNC, __renderfunc);

    return m_pHGE;
}

HGE* xae::get_core()
{
    if(NULL == m_pHGE) create_core();
    return m_pHGE;
}

void xae::set_core(const char* title /* = "XAEngine" */, float width /* = 800.0f */, float height /* = 600.0f */, int fps /* = 100 */, int bpp /* = 32 */, bool windowed /* = true *//**, bool hidemouse /* = false */, const char* logfile /* = "" */, bool splash /* = false */)
{
    if(NULL == m_pHGE) create_core();
    m_pHGE->System_SetState(HGE_TITLE, title);
    m_pHGE->System_SetState(HGE_SCREENWIDTH, width);
    m_pHGE->System_SetState(HGE_SCREENHEIGHT, height);
    m_pHGE->System_SetState(HGE_FPS, fps);
    m_pHGE->System_SetState(HGE_SCREENBPP, bpp);
    m_pHGE->System_SetState(HGE_WINDOWED, windowed);
    m_pHGE->System_SetState(HGE_HIDEMOUSE, false);
    m_pHGE->System_SetState(HGE_SHOWSPLASH, splash);
    m_pHGE->System_SetState(HGE_DONTSUSPEND, true);
    m_pHGE->System_SetState(HGE_USESOUND, true);

    if(strcmp(logfile, ""))
    {
        m_pHGE->System_SetState(HGE_LOGFILE, logfile);
    }
}

bool xae::init_core()
{
    if(NULL == m_pHGE) create_core();
    return m_pHGE->System_Initiate();
}

bool xae::start_core()
{
    if(NULL == m_pHGE) create_core();
    return m_pHGE->System_Start();
}

void xae::shutdown_core()
{
    if(NULL == m_pHGE) create_core();

    /** 场景管理器释放 */
    xaeSceneMgr::Instance().release();
    /** 资源管理器释放 */
    SAFEDEL(m_pResMgr);

    m_pHGE->System_Shutdown();
}

void xae::release_core()
{
    if(NULL == m_pHGE) create_core();

    m_pHGE->Release();
    m_pHGE = 0;
}

void xae::set_resource_mgr(xaeResourceMgrObject* mgr)
{
    SAFEDEL(m_pResMgr);
    m_pResMgr = mgr;
}

xaeResourceMgrObject* xae::get_resource_mgr()
{
    return m_pResMgr;
}

bool xae::keystate(int keycode)
{
    return m_pHGE->Input_GetKeyState(keycode);
}

bool xae::keydown(int keycode)
{
    return m_pHGE->Input_KeyDown(keycode);
}

bool xae::keyup(int keycode)
{
    return m_pHGE->Input_KeyUp(keycode);
}

bool xae::create_temp_folder(string folder_name)
{
    XStringFunc xstr(folder_name);
    xstr.ToUpper();
    m_szTmpFolders.push_back(xstr.GetString());

    if(CreateDirectory(folder_name.c_str(), NULL))
    {
        SetFileAttributes(folder_name.c_str(), FILE_ATTRIBUTE_HIDDEN);
        return true;
    }
    else return false;

    return true;
}

/** 有误 */
void xae::delete_temp_folder(string folder_name)
{
    std::string folder = folder_name;
    SHFILEOPSTRUCT FileOp;
    ZeroMemory((void*)&FileOp,sizeof(SHFILEOPSTRUCT));

    FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;
    FileOp.hNameMappings = NULL;
    FileOp.hwnd = NULL;
    FileOp.lpszProgressTitle = NULL;
    FileOp.pFrom = folder.c_str();
    FileOp.pTo = NULL;
    FileOp.wFunc = FO_DELETE;

    SHFileOperation(&FileOp);

    XStringFunc xstr(folder_name);
    xstr.ToUpper();
    folder_name = xstr.GetString();

    for(vector<string>::iterator it = m_szTmpFolders.begin(); it != m_szTmpFolders.end(); it++)
    {
        if((*it) == folder_name)
        {
            m_szTmpFolders.erase(it);
            return;
        }
    }

    return;
}

void xae::delete_all_temp_folders()
{
    while(!m_szTmpFolders.empty())
    {
        delete_temp_folder(m_szTmpFolders[0]);
    }
}

void xae::set_no_more_render(bool bNoMore /* = true */)
{
    m_bNoMoreRender = bNoMore;
}

bool xae::is_no_more_render()
{
    return m_bNoMoreRender;
}