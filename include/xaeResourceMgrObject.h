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

    /** 文件是否存在 */
    bool                        file_exists(const char* filename);

    /** 获取资源: 二进制 */
    virtual bool                get_resource_bin(string path, xaeResourceBody& body);
    /** 释放资源 */
    virtual bool                free_resource(string path);

    /** 格式化路径 */
    static string               path_to_upper(string path);

    /** 将内存数据写入文件 */
    static string              save_to_file(xaeResourceBody Res, string path, string org_filename = "");

protected:
    vector<xaeResourceBody>     m_Res;
};

#endif
