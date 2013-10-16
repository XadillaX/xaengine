#ifndef CCFGREADER_H
#define CCFGREADER_H

#include <ixaeobject.h>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

#pragma once
class XAEEXP CCFGReader
{
public:
    CCFGReader(void);
    CCFGReader(const char* filename);
    virtual ~CCFGReader(void);

    void                            SetFile(const char* filename, int size = 0);

    string                          GetString(const char* key);
    int                             GetString(const char* key, const char* split, string out[], int maxcount = 0);
    int                             GetInt(const char* key);
    int                             GetInt(const char* key, const char* split, int out[], int maxcount = 0);
    float                           GetFloat(const char* key);
    float                           GetFloat(const char* key, const char* split, float out[], int maxcount = 0);

#ifdef _DEBUG
    void                            _ShowList()
    {
        for(map<string, string>::iterator it = dict.begin(); it != dict.end(); it++)
        {
            printf("%15s = %s\n", it->first.c_str(), it->second.c_str());
        }
    }
#endif


private:
    void                            _BuildNodeFromMemo(const char* memo, DWORD size);
    void                            _BuildNode(const char* filename);

private:
    map<string, string>             dict;
};

#endif
