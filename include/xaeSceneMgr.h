/**
* XadillaX's Engine 1.0
* Utils: HGE
* Copyright (C) 2012-2013, XadillaX
*
* Scene Manager
*/
#pragma once
#ifndef XAESCENEMGR_H
#define XAESCENEMGR_H
#include <map>
#include <string>
using namespace std;

#include "utils.h"
#include "ixaeobject.h"
#include "xaeSceneObject.h"

typedef map<string, xaeSceneObject*> scene_obj_map;
typedef ObjectFactory<xaeSceneObject> xaeSceneFactory;
extern xaeSceneFactory* g_pSceneFactory;

class XAEEXP xaeSceneMgr : public iXAEObject, public Singleton<xaeSceneMgr>
{
public:
    xaeSceneMgr();
    virtual ~xaeSceneMgr();

public:
    /** ��ȡ���� */
    xaeSceneObject*             get_scene(string name);
    scene_obj_map::iterator     get_scene_iterator(string name);
    /** �������� */
    bool                        add_scene(string name, xaeSceneObject* obj);
    /** ɾ������ */
    bool                        del_scene(string name);
    /** ��ȡ��ǰ���� */
    xaeSceneObject*             get_current_scene();
    xaeSceneObject*             get_prev_scene();
    xaeSceneObject*             get_next_scene();
    /** ���õ�ǰ���� */
    bool                        set_current_scene(string name, bool bAutoDelPrev = true);
    /** ����ԭ�ȶ��õ�˳��ı䳡�� */
    void                        turn_scene_by_order();
    /** �ͷų��� */
    void                        release();

    /** �½����� */
    xaeSceneObject*             create_scene(string tn, string scenename = "default");
    /** ע�᳡������ */
    bool                        register_scene(char* tn, xaeSceneFactory::tCreator fn);
    /** ����Ĭ����� */
    void                        set_default_mouse(char* szMouse, bool bWinDefault = false);
    void                        set_default_mouse(string szMouse);
    /** ��ȡ���õ�WindowsĬ����� */
    LPSTR                       get_default_mouse_win();
    /** ��ȡ���õľ�����Ĭ����� */
    string                      get_default_mouse_path();
    /** ��ȡĬ���������: trueΪWindows��꣬����Ϊ��������� */
    bool                        get_default_mouse_type();

private:
    scene_obj_map               m_pSceneArray;
    xaeSceneObject*             m_pCurrentScene;                ///< ��ǰ����
    xaeSceneObject*             m_pPreviousScene;               ///< ��һ������
    xaeSceneObject*             m_pNextScene;                   ///< ��һ������

    string                      m_szMouse;
    LPSTR                       m_lpszMouse;
    bool                        m_bWinMouse;
};

#endif
