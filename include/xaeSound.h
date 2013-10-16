/**
 * XadillaX's Engine 1.0
 * Utils: HGE
 * Copyright (C) 2012-2013, XadillaX
 *
 * Description: Sound Object
 */
#ifndef XAESOUND_H
#define XAESOUND_H
#pragma once
#include "xaeResourceObject.h"

class XAEEXP xaeSound : xaeResourceObject
{
public:
    xaeSound();
    xaeSound(string path, bool loop = false, int volume = 100, int pan = 0, float pitch = 1.0f);
    virtual ~xaeSound();

    virtual bool                load_resource(string path);
    virtual bool                load_resource_from_snd(string path);

    /** ���ſ��ƺ��� */
    virtual void                play(bool new_channel = false);
    virtual void                stop();
    virtual void                pause();
    virtual void                slide_to(float time, int volume, int pan = -101, float pitch = -1.0f);
    /** ��ȡ������Ժ��� */
    virtual int                 get_volume();

    virtual bool                is_playing();
    virtual bool                is_sliding();
    virtual int                 length();
    virtual float               get_pos();
    virtual void                set_pos(float fSecond);
    virtual void                set_panning(int pan);
    virtual void                set_volume(int volume);
    virtual void                set_pitch(float pitch);
    virtual void                set_loop(bool bLoop = true);

    virtual void                release();

    static void                 stop_all();
    static void                 pause_all();
    static void                 resume_all();

    /** �ͷ��������֡�ע��������ֻ�����ڳ�����β�� */
    static void                 release_all();

protected:
    HEFFECT                     m_hEffect;
    HCHANNEL                    m_hChannel;

    int                         m_nVolume;
    int                         m_nPan;
    bool                        m_bLoop;
    float                       m_fPitch;
};

#endif