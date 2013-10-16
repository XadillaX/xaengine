#ifndef XAESCENETEST_H
#define XAESCENETEST_H
#pragma once

#include "vld.h"
#include <xae.h>
#include <xaesceneobject.h>
#include <xaeSprite.h>
#include "CXadillaXPack.h"
#include <string>
#include <xaeSound.h>
#include <xaeFont.h>
#include <xaeParticles.h>
#include <gui/xaeGUIButton.h>
#include <xaeSceneMgr.h>
#include <xaeFlashSprite.h>
using namespace std;

class xaeResMgr : public xaeResourceMgrObject
{
public:
    xaeResMgr()
    {
        pack.SetPackage("data.dat");
        pack.SetKey("XadillaX");
    }

    virtual bool get_resource_bin(string path, xaeResourceBody& body)
    {
        tagXadillaXPackFileData PFD;
        PFD = pack.GetFile(path.c_str());
        body.m_szPath = xaeResourceMgrObject::path_to_upper(path);
        body.m_nSize = PFD.m_dwFileSize;
        body.m_pRes = PFD.m_pBuf;
        if(NULL == body.m_pRes) return xaeResourceMgrObject::get_resource_bin(path, body);
        return true;
    }

    ~xaeResMgr()
    {
        pack.ClearCache();
    }

private:
    CXadillaXPack pack;
};

#define LEAVE_TIME 0.5f

#include <xaeSpriteLoader.h>

class xaeSceneTest : public xaeSceneObject
{
private:
    /** �������֣� �ɫ���� */
    xaeSound* snd;
    /** ���� */
    xaeFont* font;

    /** ܽ����¶�����ʯ���꼰���� */
    int diacnt;
    int diax[16], diay[16];
    /** ܽ����¶���ϻ��� */
    xaeParticles* par;
    /** ܽ����¶�����ʯ */
    xaeParticles* dia[16];
    /** ܽ����¶�۾� */
    xaeParticles* eyes[2];
    /** �������� */
    xaeParticles* starpar[10];

    xaeSprite* blood;
    xaeGUIObject* gui;

public:
    xaeSceneTest(void) : first(true), leave_time(LEAVE_TIME), enter_time(LEAVE_TIME)
    {
        /** һЩ�ز� */
        snd = new xaeSound("�ɫ���� ��D�ξ� 1st Anniv nayuta Sacred Factor.mp3", true);
        blood = new xaeSprite("Ѫ��.png");
        font = new xaeFont("����", 12, false, false, false);

        /** ������Ⱦ�� */
        add_sprite_to_pool("ܽ����¶������", xaeSpriteLoader::Instance().get_sprite("BG", "Flandre"), 0, 0);
        add_sprite_to_pool("�ĵ�", xaeSpriteLoader::Instance().get_sprite("Texture", "�ĵ�"), 20, 0);
        add_sprite_to_pool("����", xaeSpriteLoader::Instance().get_sprite("Texture", "Logo"), 0, 10);

        /** ܽ����¶���ϵĻ��� */
        par = new xaeParticles("fire.cfg");
        par->fire_at(280, 390);

        /** ܽ����¶�ĳ�� */
        init_wings();
        
        /** ܽ����¶�۾� */
        eyes[0] = new xaeParticles("eyes.cfg");
        eyes[0]->fire_at(400, 148);
        eyes[1] = new xaeParticles("eyes.cfg");
        eyes[1]->fire_at(448, 143);

        /** �������� */
        for(int i = 0; i < 10; i++)
        {
            starpar[i] = new xaeParticles("star.cfg");
            starpar[i]->fire_at(rand() % 920, rand() % 600);
        }

        /** GUI */
        gui = new xaeGUIObject();
        gui->set_pos(20, 0);
        gui->add_control(new xaeGUIButton(0, "start", "GUIButton*start%d", 50, 250, "gui/���.wav", "gui/���.wav", "Cursor*Sword"));
        gui->add_control(new xaeGUIButton(1, "load", "GUIButton*load%d", 50, 250 + 45, "gui/���.wav", "gui/���.wav", "Cursor*Sword"));
        gui->add_control(new xaeGUIButton(2, "special", "GUIButton*special%d", 50, 250 + 45 * 2, "gui/���.wav", "gui/���.wav", "Cursor*Sword"));
        gui->add_control(new xaeGUIButton(3, "end", "GUIButton*end%d", 50, 250 + 45 * 3, "gui/���.wav", "gui/���.wav", "Cursor*Sword"));
    }

    virtual ~xaeSceneTest(void)
    {
        SAFEDEL(snd);
        SAFEDEL(blood);
        SAFEDEL(font);
        SAFEDEL(par);
        SAFEDEL(eyes[0]);
        SAFEDEL(eyes[1]);
        for(int i = 0; i < 10; i++)
        {
            SAFEDEL(starpar[i]);
        }
        SAFEDEL(gui);
    }

    void init_wings()
    {
        diacnt = 16;
        diax[0] = 280, diay[0] = 520;
        diax[1] = 149, diay[1] = 510;
        diax[2] = 36, diay[2] = 440;
        diax[3] = 103, diay[3] = 378;
        diax[4] = 105, diay[4] = 256;
        diax[5] = 135, diay[5] = 134;
        diax[6] = 229, diay[6] = 180;
        diax[7] = 312, diay[7] = 246;
        diax[8] = 522, diay[8] = 207;
        diax[9] = 594, diay[9] = 125;
        diax[10] = 637, diay[10] = 208;
        diax[11] = 702, diay[11] = 219;
        diax[12] = 750, diay[12] = 258;
        diax[13] = 802, diay[13] = 269;
        diax[14] = 846, diay[14] = 343;
        diax[15] = 884, diay[15] = 415;

        for(int i = 0; i < diacnt; i++)
        {
            dia[i] = new xaeParticles("diamond.cfg");
            dia[i]->fire_at(diax[i], diay[i]);
            dia[i]->get_particle_core()->info.colColorStart.a = 0.3;
            dia[i]->get_particle_core()->info.colColorEnd.a = 0.3;
        }
    }

public:
    virtual bool render()
    {
        render_sprite_pool();

        /** ��Ⱦ���� */
        par->render();

        /** ��Ⱦ�����ʯ */
        for(int i = 0; i < diacnt; i++) dia[i]->render();
        
        /** ��Ⱦ�۾� */
        eyes[0]->render();
        eyes[1]->render();
        
        /** ��Ⱦ�������� */
        for(int i = 0; i < 10; i++) starpar[i]->render();

        /** ��ȾGUI */
        gui->render(0, 0);

        /** ���� */
        font->SetColor(0xffffffff);
        font->Print(630, 580, "�� Demo Ϊ XAE��Ϸ���� ������, ����: XadillaX");
        font->Print(10, 10, "FPS: %d", m_pHGE->Timer_GetFPS());

        /** ��Ⱦ��� */
        xaeGUIObject::render_mouse();

        return false;
    }

    bool first;
    virtual bool update(float fDT)
    {
        /** ���� */
        par->update(fDT);
        
        /** �۾� */
        eyes[0]->update(fDT);
        eyes[1]->update(fDT);

        /** ��� */
        for(int i = 0; i < diacnt; i++) dia[i]->update(fDT);

        /** ���� */
        for(int i = 0; i < 10; i++)
        {
            int dx = rand() % 51 - 25, dy = rand() % 51 - 25;
            int x, y;
            starpar[i]->get_pos(&x, &y);
            x += dx, y += dy;
            if(x < 0 || x > 920) x -= (2 * dx);
            if(y < 0 || y > 600) y -= (2 * dy);
            starpar[i]->move_to(x, y);
            starpar[i]->update(fDT);
        }  

        /** ������Ϣ */
        if(xae::Instance().keyup(HGEK_ENTER))
        {
            if(!snd->is_playing()) snd->play();
            else snd->pause();
        }
        if(xae::Instance().keyup(HGEK_UP))
        {
            snd->slide_to(0.1f, snd->get_volume() + 5);
        }
        if(xae::Instance().keyup(HGEK_DOWN))
        {
            snd->slide_to(0.1f, snd->get_volume() - 5);
        }

        float mousex, mousey;
        m_pHGE->Input_GetMousePos(&mousex, &mousey);

        gui->update(fDT, mousex, mousey);

        /** GUI�ж� */
        if(gui->get_control("start")->get_mouse_lbutton_clicked())
        {
            xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("blank", "blank");
            if(NULL != scene) xaeSceneMgr::Instance().add_scene("blank", scene);
            xaeSceneMgr::Instance().set_current_scene("blank");
        }
        if(gui->get_control("load")->get_mouse_lbutton_clicked()) xae::Instance().alert("����ǰ��", "����ǰ��", MB_ICONINFORMATION);
        if(gui->get_control("special")->get_mouse_lbutton_clicked()) xae::Instance().alert("�Թ�����", "�Թ�����", MB_ICONINFORMATION);
        if(gui->get_control("end")->get_mouse_lbutton_clicked()) return true;

        /** ����Ƿ���� */
        if(xae::Instance().keystate(HGEK_LBUTTON))
        {
            int out = true;
            for(int i = 0; i < 4; i++)
            {
                if(gui->get_control(i)->get_mouse_inside())
                {
                    out = false;
                    break;
                }
            }
            if(out)
            {
                xaeGUIObject::set_mouse(string("Cursor*Noway"));
            }
        }
        else
        {
            int out = true;
            for(int i = 0; i < 4; i++)
            {
                if(gui->get_control(i)->get_mouse_inside())
                {
                    out = false;
                    break;
                }
            }
            if(out)
            {
                xaeGUIObject::set_mouse(xaeSceneMgr::Instance().get_default_mouse_path());
            }
        }

        return false;
    }

    virtual void release()
    {
        xaeGUIObject::release_mouse();
        snd->release();
        par->release();
        blood->release();

        SAFEDEL(par);
        SAFEDEL(snd);
        SAFEDEL(font);
        SAFEDEL(gui);
        SAFEDEL(blood);
        
        eyes[0]->release(); SAFEDEL(eyes[0]);
        eyes[1]->release(); SAFEDEL(eyes[1]);
        for(int i = 0; i < diacnt; i++) { dia[i]->release(); SAFEDEL(dia[i]); }
        for(int i = 0; i < 10; i++) { starpar[i]->release(); SAFEDEL(starpar[i]); }

        xaeSceneObject::release();
    }

    float leave_time;
    float enter_time;
    virtual bool on_leave_update(float fDT)
    {
        leave_time -= fDT;
        if(leave_time < 0.0f)
        {
            leave_time = LEAVE_TIME;
            set_leave_state((XAE_SCENE_LEAVE_STATE)(get_leave_state() + 2));
            return false;
        }
        return false;
    }

    virtual bool on_leave_render()
    {
        render();
        hgeColor clr(1.0f, 1.0f, 1.0f, 1.0f - (leave_time / LEAVE_TIME));
        blood->get_sprite()->SetBlendMode(PAR_BLEND_DEFAULT);
        blood->get_sprite()->SetColor(clr.GetHWColor());
        blood->render_stretch(0, 0, 920, 600);

        xaeGUIObject::render_mouse();

        return false;
    }

    virtual bool on_enter_update(float fDT)
    {
        if(first)
        {
            first = false;
            snd->play();
        }

        enter_time -= fDT;
        if(enter_time < 0.0f)
        {
            enter_time = LEAVE_TIME;
            set_leave_state(SLS_NO_LEAVE);
            return false;
        }
        return false;
    }

    virtual bool on_enter_render()
    {
        render();
        hgeColor clr(1.0f, 1.0f, 1.0f, enter_time / LEAVE_TIME);
        blood->get_sprite()->SetBlendMode(PAR_BLEND_DEFAULT);
        blood->get_sprite()->SetColor(clr.GetHWColor());
        blood->render_stretch(0, 0, 920, 600);

        xaeGUIObject::render_mouse();

        return false;
    }
};

#define NAME_X 165
#define NAME_Y 440
#define CONTENT_X 165
#define CONTENT_Y 470
class BlankScene : public xaeSceneObject
{
private:
    xaeSprite* pTextBox;
    xaeSprite* pBG;
    xaeSprite* pFLDL;

    xaeSound* pSaya;
    bool first;

    xaeFlashSprite* pFlashPlayer;
    bool bFlashPlaying;

public:
    BlankScene() : leave_time(LEAVE_TIME), enter_time(LEAVE_TIME), first(true), bFlashPlaying(true)
    {
        fnt = new xaeFont("΢���ź�", 16, true, false);
        blood = new xaeSprite("Ѫ��.png");

        ///////////////////////////////////////////////////
        pTextBox = new xaeSprite("textbox.png");
        pBG = new xaeSprite("bg1.jpg");
        pFLDL = new xaeSprite("ܽ����¶������.png");
        pSaya = new xaeSound("saya.ogg", true);

        pFlashPlayer = new xaeFlashSprite();
        pFlashPlayer->StartFlash("fla.swf", 920, 600);
    }

    virtual ~BlankScene()
    {
        delete fnt;
        blood->release();
        delete blood;
        pTextBox->release();
        delete pTextBox;
        pBG->release();
        delete pBG;
        pFLDL->release();
        delete pFLDL;
        pSaya->release();
        delete pSaya;

        SAFEDEL(pFlashPlayer);
    }

    virtual bool render()
    {
        if(bFlashPlaying)
        {
            pFlashPlayer->Render(0, 0);
        }
        else
        {
            pBG->render();
            pFLDL->render(920 - 500, 0);
            pTextBox->render();

            fnt->SetColor(0xffa900c8);
            fnt->Print(NAME_X, NAME_Y, "ܽ����¶");
            fnt->SetColor(0xff000000);
            fnt->Print(CONTENT_X, CONTENT_Y, "�त���त���त���त���ʤ����ʤ����ʤ����ʤ�����");
        }

        xaeGUIObject::render_mouse();

        return false;
    }

    virtual bool update(float fDT)
    {
        if(first)
        {
            first = false;
            pSaya->play(true);
        }

        /** ����FLASH */
        if(bFlashPlaying && pFlashPlayer->IsPlaying())
        {
            pFlashPlayer->Update(fDT);
        }
        else
        if(bFlashPlaying)
        {
            bFlashPlaying = false;
            SAFEDEL(pFlashPlayer);
        }

        if(!bFlashPlaying)
        {
            if(xae::Instance().keystate(HGEK_SPACE) || xae::Instance().keystate(HGEK_LBUTTON) || xae::Instance().keystate(HGEK_ENTER))
            {
                xaeSceneObject* scene = xaeSceneMgr::Instance().create_scene("test", "test");
                xaeSceneMgr::Instance().add_scene("test", scene);
                xaeSceneMgr::Instance().set_current_scene("test");
            }
        }
        
        return false;
    }

    float leave_time;
    float enter_time;
    virtual bool on_leave_update(float fDT)
    {
        leave_time -= fDT;
        if(leave_time < 0.0f)
        {
            leave_time = LEAVE_TIME;
            set_leave_state((XAE_SCENE_LEAVE_STATE)(get_leave_state() + 2));
            return false;
        }
        return false;
    }

    virtual bool on_leave_render()
    {
        render();
        hgeColor clr(1.0f, 1.0f, 1.0f, 1.0f - (leave_time / LEAVE_TIME));
        blood->get_sprite()->SetBlendMode(PAR_BLEND_DEFAULT);
        blood->get_sprite()->SetColor(clr.GetHWColor());
        blood->render_stretch(0, 0, 920, 600);

        xaeGUIObject::render_mouse();

        return false;
    }

    virtual bool on_enter_update(float fDT)
    {
        enter_time -= fDT;
        if(enter_time < 0.0f)
        {
            enter_time = LEAVE_TIME;
            set_leave_state(SLS_NO_LEAVE);
            return false;
        }
        return false;
    }

    virtual bool on_enter_render()
    {
        render();
        hgeColor clr(1.0f, 1.0f, 1.0f, enter_time / LEAVE_TIME);
        blood->get_sprite()->SetBlendMode(PAR_BLEND_DEFAULT);
        blood->get_sprite()->SetColor(clr.GetHWColor());
        blood->render_stretch(0, 0, 920, 600);

        xaeGUIObject::render_mouse();

        return false;
    }

private:
    xaeFont* fnt;
    xaeSprite* blood;
};
#endif
