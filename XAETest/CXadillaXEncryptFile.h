//////////////////////////////////////////////////////////////////////////
//
//	资源加密类
//
//	Program by 死月(XadillaX) (admin@xcoder.in)
//
//////////////////////////////////////////////////////////////////////////
#ifndef CXADILLAXENCRYPTFILE_H
#define CXADILLAXENCRYPTFILE_H
#pragma once

#include <wtypes.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define MAX_KEY_LENGTH                      63

class CXadillaXEncryptFile
{
public:
    CXadillaXEncryptFile(void);
    virtual ~CXadillaXEncryptFile(void);

    int                     GetShuffleCount();
    bool                    SetEncryptKey(const char* szKey);
    bool                    SetFile(const char* szFilename, int nShuffleID = 0);
    bool                    SetFileInPackage(FILE* pFile, DWORD dwStartPos, DWORD dwSize, int nShuffleID = 0);
    size_t                  GetNextEncryptBuffer(unsigned char* &pBuffer, int nBufLength = 512);
    size_t                  GetNextDecryptBuffer(unsigned char* &pBuffer, int nBufLength = 512);
    size_t                  GetFileSize(const char* szFilename);

    int                     GetShuffleIDByString(const char* szStr);

private:
    unsigned char*          m_pBuffer;
    char                    m_szEncryptKey[MAX_KEY_LENGTH];
    int                     m_nEncryptKeyLength;
    FILE*                   m_pFile;
    int                     m_nShuffleID;
    int                     m_nKeyIdx;
    size_t                  m_nFileSize;
    size_t                  m_nCurFileSize;

    int                     m_pAntiShuffle[256];
    bool                    m_bPackage;
};

#endif
