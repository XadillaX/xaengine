/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Resource Manager Object
*/
#include <xaeResourceMgrObject.h>
#include <io.h>

xaeResourceMgrObject::xaeResourceMgrObject(void)
{
    m_pHGE = hgeCreate(HGE_VERSION);
}

xaeResourceMgrObject::~xaeResourceMgrObject(void)
{
    for(size_t i = 0; i < m_Res.size(); i++)
    {
        m_pHGE->Resource_Free(m_Res[i].m_pRes);
    }
    m_Res.clear();
    m_pHGE->Release();
}

bool xaeResourceMgrObject::file_exists(const char* filename)
{
    return (0 == access(filename, 0));
}

bool xaeResourceMgrObject::get_resource_bin(string path, xaeResourceBody& body)
{
    /** 载入资源 */
    DWORD size;
    unsigned char* res;
    res = (unsigned char*)(m_pHGE->Resource_Load(path.c_str(), &size));

    /** 载入失败 */
    if(NULL == res)
    {
        m_pHGE->System_Log("[!] Resource [%s] can't be loaded.", path.c_str());
        body.m_szPath = xaeResourceMgrObject::path_to_upper(path);
        body.m_nSize = 0;
        body.m_pRes = 0;
        return false;
    }

    /** 加入数组 */
    body.m_szPath = xaeResourceMgrObject::path_to_upper(path);
    body.m_nSize = size;
    body.m_pRes = res;
    m_Res.push_back(body);

    return true;
}

bool xaeResourceMgrObject::free_resource(string path)
{
    /** 格式化路径 */
    path = xaeResourceMgrObject::path_to_upper(path);

    /** 遍历资源数组 */
    vector<xaeResourceBody>::iterator it;
    for(it = m_Res.begin(); it != m_Res.end(); it++)
    {
        /** 若匹配则释放并且返回true */
        if((*it).m_szPath == path)
        {
            m_pHGE->Resource_Free((void*)(*it).m_pRes);
            m_Res.erase(it);
            return true;
        }
    }

    /** 未找到 */
    return false;
}

string xaeResourceMgrObject::path_to_upper(string path)
{
    XStringFunc xstr(path);
    xstr.ToUpper();
    string str = xstr.GetString();

    for(size_t i = 0; i < str.length(); i++)
    {
        if(path[i] == '/') path[i] = '\\';
    }

    return xstr.GetString();
}

#pragma comment(lib,"ole32.lib")
string xaeResourceMgrObject::save_to_file(xaeResourceBody Res, string path, string org_filename)
{
    /** 获取扩展名 */
    string ext = "";
    if("" == org_filename) org_filename = Res.m_szPath;
    for(int i = org_filename.length() - 1; i >= 0 && org_filename[i] != '.'; i--)
    {
        ext = org_filename[i] + ext;
    }
    if(ext.length() == org_filename.length()) ext = "";
    else ext = '.' + ext;

    /** 创建唯一文件名 */
    GUID guid;
    CoCreateGuid(&guid);
    static char szGuid[128] = {0};
    sprintf(
        szGuid,
        "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1],
        guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5],
        guid.Data4[6], guid.Data4[7]);
    
    string new_filename = path + "\\" + szGuid + ext;

    /** 打开文件 */
    FILE* fp = fopen(new_filename.c_str(), "wb+");
    if(NULL == fp) return "";

    fwrite(Res.m_pRes, Res.m_nSize, 1, fp);

    fclose(fp);

    return new_filename;
}