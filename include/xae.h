/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * System layer API
 */
#ifndef XAE_H
#define XAE_H

#define TINYXML_USE_STL

#include "utils.h"
#include "ixaeobject.h"
#include "singleton.h"
#include "xaeResourceMgrObject.h"
#pragma once

class XAEEXP xae : public iXAEObject, public Singleton<xae>
{
public:
    xae();
    ~xae();

/** һЩͨ�ú��� */
public:
    void                        alert(const char* msg, const char* title = "XAE��Ϣ", int icon = MB_ICONINFORMATION, bool console_only = false);

/** ����HGE������ */
public:
    /** ����HGE�ں� */
    HGE*                        create_core();
    /** ��ȡHGE�ں� */
    HGE*                        get_core();
    /** ����HGE�ں� */
    void                        set_core(
                                    const char* title = "XAEngine",
                                    float width = 800.0f,
                                    float height = 600.0f,
                                    int fps = 100,
                                    int bpp = 32,
                                    bool windowed = true,
                                    /** bool hidemouse = false, */
                                    const char* logfile = "",
                                    bool splash = false
                                    );
    /** ��ʼ��HGE�ں� */
    bool                        init_core();
    /** ����HGE�ں� */
    bool                        start_core();
    /** �ر�HGE�ں� */
    void                        shutdown_core();
    /** �ͷ�HGE�ں� */
    void                        release_core();
    /** ��ʱ���ٸ�����Ⱦ */
    void                        set_no_more_render(bool bNoMore = true);
    bool                        is_no_more_render();
    
/** ������Դ���ĺ��� */
public:
    /** ������Դ�������� */
    void                        set_resource_mgr(xaeResourceMgrObject* mgr);
    /** ��ȡ��Դ�������� */
    xaeResourceMgrObject*       get_resource_mgr();

/** �������뺯�� */
public:
    bool                        keystate(int keycode);
    bool                        keydown(int keycode);
    bool                        keyup(int keycode);

/** ���ϵͳ���� */
public:
    bool                        create_temp_folder(string folder_name);
    void                        delete_temp_folder(string folder_name);
    void                        delete_all_temp_folders();

/** ˽�г�Ա */
private:
    xaeResourceMgrObject*       m_pResMgr;
    vector<string>              m_szTmpFolders;
    bool                        m_bNoMoreRender;
};

#endif
