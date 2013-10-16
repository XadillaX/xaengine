/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Base class
 */
#ifndef IXAEOBJECT_H
#define IXAEOBJECT_H
#pragma once

#ifdef XAEDLL
    #define XAEEXP  __declspec(dllexport)
#else
    #define XAEEXP __declspec(dllimport)
#endif

#define CALL __stdcall

#include "utils.h"

class XAEEXP iXAEObject
{
public:
    iXAEObject() : m_pHGE(NULL) {};
    virtual ~iXAEObject() {};

protected:
    HGE*                        m_pHGE;
};

#endif
