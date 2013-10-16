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

    /** ȥ�ո� */
    void                        LTrim();
    void                        RTrim();
    void                        LRTrim();
    void                        AllTrim();
    
    /** �ָ� */
    int                         Split(string szCHR, string* szArr, int nMaxCount = 0);

    /** ����ת��I */
    static int                  ToInt(string szStr);
    static void                 ToInt(string* szArr, int* nArr, int nCount);
    static double               ToDouble(string szStr);
    static void                 ToDouble(string* szArr, double* fArr, int nCount);

    /** ����ת��II */
    static string               IntToString(int a);

    /** ��ȡ��� */
    void                        SetString(string szStr);
    void                        SetString(const char* szStr);
    string                      GetString();

    /** ��Сд */
    void                        ToUpper();
    void                        ToLower();
private:
    string                      m_szStr;
};

#endif
