/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: Resource Object
 */
#ifndef XAERESOURCEOBJECT_H
#define XAERESOURCEOBJECT_H
#pragma once

#include "utils.h"
#include "ixaeobject.h"
#include <Windows.h>
#include <string>
using namespace std;

class XAEEXP xaeResourceObject : public iXAEObject
{
public:
    xaeResourceObject(void);
    virtual ~xaeResourceObject(void);

    /** ������Դ */
    virtual bool                load_resource(string path);
    /** ��CFG�ļ�������Դ */
    virtual bool                load_resource_from_cfg(string path);
    /** ��ȡ��Դ·�� */
    virtual string              get_path();

    /** ��������� */
    virtual void                lock(bool light_lock = true);
    virtual void                unlock(bool light_lock = true);

    virtual void                release();

    struct LockScope
    {
        LockScope(xaeResourceObject &oRes, bool bLightLock = true) : m_oRes(oRes), m_bLightLock(bLightLock)
        {
            m_oRes.lock(m_bLightLock);
        }
        ~LockScope()
        {
            m_oRes.unlock(m_bLightLock);
        }
        xaeResourceObject &m_oRes;
        bool m_bLightLock;
    };
protected:
    CRITICAL_SECTION            m_cs;
    string                      m_szPath;
};

#endif