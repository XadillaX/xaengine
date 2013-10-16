#ifndef XSTRINGFUNC_H
#define XSTRINGFUNC_H

#include <ixaeobject.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
using namespace std;
#pragma once

class XAEEXP XStringFunc
{
public:
    XStringFunc();
    XStringFunc(string szStr);
    XStringFunc(const char* szStr);

    /** 去空格 */
    void                        LTrim();
    void                        RTrim();
    void                        LRTrim();
    void                        AllTrim();
    
    /** 分割 */
    int                         Split(string szCHR, string* szArr, int nMaxCount = 0);

    /** 类型转换I */
    static int                  ToInt(string szStr);
    static void                 ToInt(string* szArr, int* nArr, int nCount);
    static double               ToDouble(string szStr);
    static void                 ToDouble(string* szArr, double* fArr, int nCount);

    /** 类型转换II */
    static string               IntToString(int a);

    /** 获取结果 */
    void                        SetString(string szStr);
    void                        SetString(const char* szStr);
    string                      GetString();

    /** 大小写 */
    void                        ToUpper();
    void                        ToLower();
private:
    string                      m_szStr;
};

#endif
