/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* XAE Sprite : based on hgeSprite
*/
#include <xae.h>
#include <xaeSprite.h>
#include <XStringFunc.h>
#include <xaeResourceMgrObject.h>
#include <assert.h>
#include <CCFGReader.h>

xaeSprite::xaeSprite(void) :
    m_hTex(0),
    m_pSprite(NULL)
{
}

xaeSprite::xaeSprite(const char* path, int x, int y, int w, int h) :
    m_hTex(0),
    m_pSprite(NULL)
{
    if(false == load_resource(path, x, y, w, h))
    {
        xae::Instance().alert("Can't load resource.");
        exit(0);
    }
}

xaeSprite::~xaeSprite(void)
{
    release();
}

bool xaeSprite::load_resource(string path)
{
    string new_path = xaeResourceMgrObject::path_to_upper(path);
    /** 最后4位是.CFG */
    if(new_path.find(".CFG", new_path.length() - 4) != string::npos)
    {
        return load_resource_from_cfg(path);
    }
    else
        /** 不是.CFG */
    {
        return load_resource_from_pic(new_path);
    }

    return true;
}

bool xaeSprite::load_resource(string path, int x, int y, int w, int h)
{
    /** 将路径转换为大写 */
    string new_path = xaeResourceMgrObject::path_to_upper(path);

    /** 最后4位是.CFG */
    if(new_path.find(".CFG", new_path.length() - 4) != string::npos)
    {
        return load_resource_from_cfg(path);
    }
    else
    /** 不是.CFG */
    {
        return load_resource_from_pic(new_path, x, y, w, h);
    }

    return true;
}

bool xaeSprite::load_resource_from_cfg(string path)
{
    path = xaeResourceMgrObject::path_to_upper(path);
    /** 资源管理器中载入cfg文件 */
    xaeResourceBody RB;
    xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
    bool res = pRMO->get_resource_bin(path, RB);
    if(!res)
    {
        /** 载入出错 */
        return false;
    }

    /** 打开CFG文件 */
    CCFGReader reader;
    reader.SetFile((char*)RB.m_pRes, RB.m_nSize);

    /** 参数 */
    string filename = reader.GetString("FILENAME");
    int x = reader.GetInt("X");
    int y = reader.GetInt("Y");
    int w = reader.GetInt("W");
    int h = reader.GetInt("H");
    int hx = reader.GetInt("HOTSPOT_X");
    int hy = reader.GetInt("HOTSPOT_Y");

    if(filename.length() == 0)
    {
        m_pHGE->System_Log("[!] The path of filename to create a particle is NULL.");
        return false;
    }

    /** 相对路径还是什么 */
    if('/' != filename[0] && '\\' != filename[0])
    {
        /** 获取cfg文件的路径 */
        int len = 0;
        for(int i = path.length() - 1; i >= 0; i--)
        {
            if(path[i] == '\\' || path[i] == '/')
            {
                len = i + 1;
                break;
            }
        }

        /** 加上cfg路径 */
        filename = path.substr(0, len) + filename;
    }

    /** 读取 */
    return load_resource_from_pic(filename, x, y, w, h, hx, hy);

    return true;
}

bool xaeSprite::load_resource_from_pic(string path, int x, int y, int w, int h, int hx, int hy)
{
    path = xaeResourceMgrObject::path_to_upper(path);
    /** 从资源管理器中载入 */
    xaeResourceBody RB;
    xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
    bool res = pRMO->get_resource_bin(path, RB);

    /** 载入成功 */
    if(res)
    {
        /** 锁定临界区 */
        xaeResourceObject::LockScope oLock(*this);

        /** 删除先前纹理 */
        SAFEDEL(m_pSprite);
        if(0 != m_hTex)
        {
            m_pHGE->Texture_Free(m_hTex);
            m_hTex = 0;
        }

        m_hTex = m_pHGE->Texture_Load((char*)RB.m_pRes, RB.m_nSize);
        /** 创建纹理失败 */
        if(0 == m_hTex)
        {
            m_pHGE->System_Log("[!] Sprite can't create the texture [%s].", path.c_str());

            /** 跳出临界区 */

            return false;
        }

        if(0 == x && 0 == y && 0 == w && 0 == h)
        {
            m_pSprite = new hgeSprite(m_hTex, 0, 0, m_pHGE->Texture_GetWidth(m_hTex, true), m_pHGE->Texture_GetHeight(m_hTex, true));
        }
        else m_pSprite = new hgeSprite(m_hTex, x, y, w, h);
        m_szPath = path;

        m_pSprite->SetHotSpot(hx, hy);

        return true;
    }
    /** 载入失败 */
    else
    {
        return false;
    }
}

void xaeSprite::render(float x /* = 0.0f */, float y /* = 0.0f */)
{
    xaeResourceObject::LockScope oLock(*this);
    //assert(m_pSprite != NULL);
    if(NULL != m_pSprite) m_pSprite->Render(x, y);
}

void xaeSprite::render_stretch(float x1, float y1, float x2, float y2)
{
    xaeResourceObject::LockScope oLock(*this);
    //assert(m_pSprite != NULL);
    if(NULL != m_pSprite) m_pSprite->RenderStretch(x1, y1, x2, y2);
}

void xaeSprite::render_ex(float x, float y, float rot, float hscale, float vscale)
{
    if(NULL != m_pSprite) m_pSprite->RenderEx(x, y, rot, hscale, vscale);
}

hgeSprite* xaeSprite::get_sprite()
{
    xaeResourceObject::LockScope oLock(*this);
    hgeSprite* spr = m_pSprite;

    return spr;
}

void xaeSprite::lock(bool light_lock)
{
    xaeResourceObject::lock();
    if(!light_lock && 0 != m_hTex) m_pHGE->Texture_Lock(m_hTex, false);
}

void xaeSprite::unlock(bool light_lock)
{
    if(!light_lock && 0 != m_hTex) m_pHGE->Texture_Unlock(m_hTex);
    xaeResourceObject::unlock();
}

void xaeSprite::release()
{
    xaeResourceObject::LockScope oLock(*this);
    SAFEDEL(m_pSprite);
    if(0 != m_hTex)
    {
        m_pHGE->Texture_Free(m_hTex);
        m_hTex = 0;
    }

    xae::Instance().get_resource_mgr()->free_resource(m_szPath);
}
