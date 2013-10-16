#include "xaePackage.h"

xaePackage::xaePackage(void)
{
    pack.SetPackage("data.pz");
    pack.SetKey("XadillaX");
}

xaePackage::~xaePackage(void)
{
    clear_cache();
}

bool xaePackage::get_resource_bin(string path, xaeResourceBody& body)
{
    tagXadillaXPackFileData PFD;
    PFD = pack.GetFile(path.c_str());
    body.m_szPath = xaeResourceMgrObject::path_to_upper(path);
    body.m_nSize = PFD.m_dwFileSize;
    body.m_pRes = PFD.m_pBuf;
    if(NULL == body.m_pRes) return xaeResourceMgrObject::get_resource_bin(path, body);

    return true;
}

void xaePackage::clear_cache()
{
    pack.ClearCache();
}
