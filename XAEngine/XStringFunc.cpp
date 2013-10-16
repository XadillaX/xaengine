#include <XStringFunc.h>

XStringFunc::XStringFunc()
{
    m_szStr = "";
}

XStringFunc::XStringFunc(string szStr)
{
    m_szStr = szStr;
}

XStringFunc::XStringFunc(const char* szStr)
{
    m_szStr = szStr;
}

void XStringFunc::SetString(string szStr)
{
    m_szStr = szStr;
}

void XStringFunc::SetString(const char* szStr)
{
    m_szStr = szStr;
}

string XStringFunc::GetString()
{
    return m_szStr;
}

void XStringFunc::LTrim()
{
    while(m_szStr.length())
    {
        if(m_szStr[0] == ' ' || m_szStr[0] == '\t') m_szStr.erase(0, 1);
        else break;
    }
}

void XStringFunc::RTrim()
{
    while(m_szStr.length())
    {
        if(m_szStr[m_szStr.length() - 1] == ' ' || m_szStr[m_szStr.length() - 1] == '\t') m_szStr.erase(m_szStr.length() - 1, 1);
        else break;
    }
}

void XStringFunc::LRTrim()
{
    LTrim();
    RTrim();
}

void XStringFunc::AllTrim()
{
    for(size_t i = 0; i < m_szStr.length(); i++)
    {
        if(m_szStr[i] == ' ' || m_szStr[i] == '\t')
        {
            m_szStr.erase(i, 1);
            i--;
        }
    }
}

int XStringFunc::Split(string szCHR, string* szArr, int nMaxCount)
{
    string szTMP = m_szStr;
    int nRst = 0, pos = 0;
    LRTrim();

    /** 未超出限制数量 */
    while(nMaxCount == 0 || nMaxCount > nRst)
    {
        /** 寻找分隔符 */
        pos = szTMP.find(szCHR, 0);

        /** 若分隔符是第一个则继续 */
        if(pos == 0)
        {
            szTMP.erase(0, szCHR.length());
            continue;
        }

        /** 若找不到分隔符则跳出 */
        if(pos == string::npos) break;

        /** 将字符串分割 */
        szArr[nRst++] = szTMP.substr(0, pos);
        szTMP.erase(0, pos + szCHR.length());
    }

    /** 如果有尾巴则将尾巴补上 */
    if(szTMP.length() && (nMaxCount == 0 || nMaxCount > nRst)) szArr[nRst++] = szTMP;
    
    return nRst;
}

int XStringFunc::ToInt(string szStr)
{
    return atoi(szStr.c_str());
}

void XStringFunc::ToInt(string* szArr, int* nArr, int nCount)
{
    for(int i = 0; i < nCount; i++)
    {
        nArr[i] = atoi(szArr[i].c_str());
    }
}

double XStringFunc::ToDouble(string szStr)
{
    return atof(szStr.c_str());
}

void XStringFunc::ToDouble(string* szArr, double* fArr, int nCount)
{
    for(int i = 0; i < nCount; i++)
    {
        fArr[i] = atof(szArr[i].c_str());
    }
}

void XStringFunc::ToUpper()
{
    for(size_t i = 0; i < m_szStr.length(); i++)
    {
        m_szStr[i] = toupper(m_szStr[i]);
    }
}

void XStringFunc::ToLower()
{
    for(size_t i = 0; i < m_szStr.length(); i++)
    {
        m_szStr[i] = tolower(m_szStr[i]);
    }
}

string XStringFunc::IntToString(int a)
{
    char buf[20];
    sprintf(buf, "%d", a);

    return string(buf);
}
