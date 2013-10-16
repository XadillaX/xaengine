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

/** 一些通用函数 */
public:
    void                        alert(const char* msg, const char* title = "XAE消息", int icon = MB_ICONINFORMATION, bool console_only = false);

/** 关于HGE的设置 */
public:
    /** 创建HGE内核 */
    HGE*                        create_core();
    /** 获取HGE内核 */
    HGE*                        get_core();
    /** 设置HGE内核 */
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
    /** 初始化HGE内核 */
    bool                        init_core();
    /** 启动HGE内核 */
    bool                        start_core();
    /** 关闭HGE内核 */
    void                        shutdown_core();
    /** 释放HGE内核 */
    void                        release_core();
    /** 暂时不再更新渲染 */
    void                        set_no_more_render(bool bNoMore = true);
    bool                        is_no_more_render();
    
/** 关于资源包的函数 */
public:
    /** 设置资源包管理器 */
    void                        set_resource_mgr(xaeResourceMgrObject* mgr);
    /** 获取资源包管理器 */
    xaeResourceMgrObject*       get_resource_mgr();

/** 关于输入函数 */
public:
    bool                        keystate(int keycode);
    bool                        keydown(int keycode);
    bool                        keyup(int keycode);

/** 外界系统函数 */
public:
    bool                        create_temp_folder(string folder_name);
    void                        delete_temp_folder(string folder_name);
    void                        delete_all_temp_folders();

/** 私有成员 */
private:
    xaeResourceMgrObject*       m_pResMgr;
    vector<string>              m_szTmpFolders;
    bool                        m_bNoMoreRender;
};

#endif
