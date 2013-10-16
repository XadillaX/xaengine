//////////////////////////////////////////////////////////////////////////
//
//	��Դ����
//
//	Program by ����(XadillaX) (admin@xcoder.in)
//
//////////////////////////////////////////////////////////////////////////
#ifndef CXADILLAXPACK_H
#define CXADILLAXPACK_H
#pragma once

#include <wtypes.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <map>
#include <list>
#include "CXadillaXEncryptFile.h"
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////
//  _____________________________________________
// |   HEAD   |   DATA   |  INDEX  | EMPTY INDEX |
// |__________|__________|_________|_____________| --> PACKAGE BY XADILLAX
// TO SAVE EASILY, IT WILL NOT TRULY DELETE ANYTHING BUT MAKE A EMPTY PLACE FOR OTHERS
//////////////////////////////////////////////////////////////////////////////////////

#define PACK_MAGIC_NUM                  0x49444158              ///< XADI

/** �ļ�ͷ */
struct tagXadillaXPackHeader {
    DWORD                   m_dwMagic;                          ///< Magic�ַ�
    DWORD                   m_dwIdxStartPos;                    ///< ������ʼλ��
    DWORD                   m_dwEmptyIdxStartPos;               ///< ���ļ�������ʼλ��
    DWORD                   m_dwFileCount;                      ///< �ļ�����
    DWORD                   m_dwEmptyCount;                     ///< ���ļ�����
    DWORD                   m_dwVersion;                        ///< �����汾
};

/** �ļ���Ϣ(����) */
struct tagXadillaXPackFileInfo {
    char                    m_szFileName[128];
    DWORD                   m_dwRelativePos;
    DWORD                   m_dwFileSize;
};

/** �ļ���Ϣ */
struct tagXadillaXPackFileData {
    DWORD                   m_dwFileSize;
    UCHAR*                  m_pBuf;
};

class CXadillaXPack
{
public:
    CXadillaXPack();
    virtual ~CXadillaXPack();

    bool                            IsOpened() { return m_bOpened; }
    bool                            SetPackage(const char* filename);
    bool                            SetKey(const char* szKey);

    bool                            DelFile(const char* filename);
    bool                            AddFile(const char* filename, char* newfilename);
    tagXadillaXPackFileData         GetFile(const char* filename);

    void                            ClearCache();


    void                            FormatFilename(char* filename);
    void                            ToLowerCase(string &szStr);
    void                            ToUpperCase(string &szStr);

    DWORD                           GetPackageVersion();
    bool                            SetPackageVersion(DWORD dwVersion);

private:
    void                            __CloseFile();
    bool                            __CheckHeader();
    void                            __UpdateIdx();
    void                            __UpdateCache(const char* filename);

private:
    char                            m_szFilename[512];
    FILE*                           m_pFile;
    CXadillaXEncryptFile*           m_pEncrypt;
    size_t                          m_FileSize;

    tagXadillaXPackHeader           m_Header;
    map<string, tagXadillaXPackFileInfo> m_FileInfoMap;
    list<tagXadillaXPackFileInfo>   m_EmptyList;
    map<string, UCHAR*>             m_FileData;

    bool                            m_bOpened;
};

#endif
