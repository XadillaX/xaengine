#ifndef XAEPACKAGE_H
#define XAEPACKAGE_H
#pragma once
#include <xaeResourceMgrObject.h>
#include "CXadillaXPack.h"

class xaePackage : public xaeResourceMgrObject
{
public:
    xaePackage(void);
    virtual ~xaePackage(void);

    virtual bool get_resource_bin(string path, xaeResourceBody& body);

    void clear_cache();

private:
    CXadillaXPack pack;
};
#endif
