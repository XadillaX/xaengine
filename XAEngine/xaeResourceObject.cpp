#include "xaeResourceObject.h"

xaeResourceObject::xaeResourceObject() :
    m_szPath("")
{
    m_pHGE = hgeCreate(HGE_VERSION);

    ::InitializeCriticalSection(&m_cs);
}

xaeResourceObject::~xaeResourceObject()
{
    m_pHGE->Release();
}

bool xaeResourceObject::load_resource(string path)
{
    m_szPath = path;
    return true;
}

bool xaeResourceObject::load_resource_from_cfg(std::string path)
{
    m_szPath = path;
    return true;
}

string xaeResourceObject::get_path()
{
    return m_szPath;
}

void xaeResourceObject::lock(bool light_lock)
{
    ::EnterCriticalSection(&m_cs);
}

void xaeResourceObject::unlock(bool light_lock)
{
    ::LeaveCriticalSection(&m_cs);
}

void xaeResourceObject::release()
{

}