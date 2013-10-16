/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Resource Manager Object
*/
#ifndef XAERESOURCEMGROBJECT_H
#define XAERESOURCEMGROBJECT_H
#pragma once
#include "ixaeobject.h"
#include "utils.h"
#include <string>
#include <vector>
using namespace std;

struct xaeResourceBody
{
    string                      m_szPath;
    unsigned long               m_nSize;
    unsigned char*              m_pRes;
};

class XAEEXP xaeResourceMgrObject : public iXAEObject
{
public:
    xaeResourceMgrObject(void);
    virtual ~xaeResourceMgrObject(void);

    /** �ļ��Ƿ���� */
    bool                        file_exists(const char* filename);

    /** ��ȡ��Դ: ������ */
    virtual bool                get_resource_bin(string path, xaeResourceBody& body);
    /** �ͷ���Դ */
    virtual bool                free_resource(string path);

    /** ��ʽ��·�� */
    static string               path_to_upper(string path);

    /** ���ڴ�����д���ļ� */
    static string              save_to_file(xaeResourceBody Res, string path, string org_filename = "");

protected:
    vector<xaeResourceBody>     m_Res;
};

#endif
