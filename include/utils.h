/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Utils' wrapper
 */
#ifndef UTILS_H
#define UTILS_H
#pragma once

#include <assert.h>
#include <stdexcept>
#include "hge/hgecolor.h"
#include "hge/hge.h"
#include "hge/hgesprite.h"
#include "hge/hgeanim.h"
#include "hge/hgegui.h"
#include "hge/hgeguictrls.h"
#include "Singleton.h"
#include "ObjectFactory.h"
#include "XStringFunc.h"
#include "CCFGReader.h"
#include <winbase.h>

#ifndef SAFEDEL
#define SAFEDEL(p) { if(NULL != (p)) { delete (p); (p) = NULL; } }
#endif

/** XAEµ„¿‡–Õ */
struct xaePoint
{
    int x;
    int y;
};
typedef hgeRect xaeRect;
#define XAE_FLASH_TMP_FOLDER ("tmp.flash.xae")

struct LockScope
{
    bool m_bLock;
    CRITICAL_SECTION &m_cs;

    LockScope(CRITICAL_SECTION &cs) : m_cs(cs), m_bLock(false)
    {
        ::EnterCriticalSection(&m_cs);
        m_bLock = true;
    }
    void Unlock()
    {
        if (m_bLock)
        {
            m_bLock = false;
            ::LeaveCriticalSection(&m_cs);
        }
    }
    ~LockScope()
    {
        Unlock();
    }

};


#endif
