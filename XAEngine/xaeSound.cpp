#include "xaeSound.h"
#include <xae.h>
#include <XStringFunc.h>

/** Eff句柄相关全局变量以及函数 */
struct _eff_time
{
    HEFFECT eff;
    int time;
};
map<string, _eff_time> _hEffect;
bool _bEffectInited = false;
::CRITICAL_SECTION _eff_cs;

/** 根据路径获取HEFFECT */
HEFFECT _get_effect(string path)
{
    static HGE* pHGE = xae::Instance().get_core();
    path = xaeResourceMgrObject::path_to_upper(path);

    LockScope oScope(_eff_cs);
    if(_hEffect.find(path) == _hEffect.end())
    {
        /** 从资源管理器中载入 */
        xaeResourceBody RB;
        xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
        bool res = pRMO->get_resource_bin(path, RB);

        /** 载入失败 */
        if(!res)
        {
            return 0;
        }
        _hEffect[path].eff = pHGE->Effect_Load((char*)RB.m_pRes, RB.m_nSize);
        _hEffect[path].time = 0;        
    }
    HEFFECT eff = _hEffect[path].eff;
    _hEffect[path].time++;

    return eff;
}

xaeSound::xaeSound() :
    m_hEffect(0),
    m_hChannel(0),
    m_nVolume(100),
    m_nPan(0),
    m_bLoop(false),
    m_fPitch(1.0f)
{
    if(!_bEffectInited)
    {
        _bEffectInited = true;
        _hEffect.clear();
        ::InitializeCriticalSection(&_eff_cs);
    }
}

xaeSound::xaeSound(string path, bool loop, int volume, int pan, float pitch) :
    m_nVolume(volume),
    m_bLoop(loop),
    m_nPan(pan),
    m_fPitch(pitch)
{
    if(!_bEffectInited)
    {
        _bEffectInited = true;
        _hEffect.clear();
        ::InitializeCriticalSection(&_eff_cs);
    }

    load_resource(path);
}

xaeSound::~xaeSound()
{
    release();
}

bool xaeSound::load_resource(string path)
{
    /** 将路径转换为大写 */
    string new_path = xaeResourceMgrObject::path_to_upper(path);

    return load_resource_from_snd(new_path);
}

bool xaeSound::load_resource_from_snd(string path)
{
    /** 锁定临界区 */
    xaeResourceObject::LockScope oLock(*this);
    path = xaeResourceMgrObject::path_to_upper(path);

    /** 置零 */
    if(0 != m_hChannel) m_pHGE->Channel_Stop(m_hChannel);
    if(0 != m_hEffect) m_pHGE->Effect_Free(m_hEffect);
    m_hChannel = 0;
    m_hEffect = 0;

    m_hEffect = _get_effect(path);
    /** 创建纹理失败 */
    if(0 == m_hEffect)
    {
        m_pHGE->System_Log("[!] Sound [%s] can't be created.", path.c_str());

        /** 跳出临界区 */

        return false;
    }
    m_szPath = path;

    /** 跳出临界区 */

    return true;
}

void xaeSound::play(bool new_channel)
{
    xaeResourceObject::LockScope oLock(*this);
    if(0 == m_hChannel)
    {
        m_hChannel = m_pHGE->Effect_PlayEx(m_hEffect, m_nVolume, m_nPan, m_fPitch, m_bLoop);
    }
    else
    if(new_channel)
    {
        m_pHGE->Channel_Stop(m_hChannel);
        m_hChannel = m_pHGE->Effect_PlayEx(m_hEffect, m_nVolume, m_nPan, m_fPitch, m_bLoop);
    }
    else
    {
        m_pHGE->Channel_Resume(m_hChannel);
    }
}

void xaeSound::stop()
{
    xaeResourceObject::LockScope oLock(*this);
    if(0 != m_hChannel)
    {
        m_pHGE->Channel_Stop(m_hChannel);
        m_hChannel = 0;
    }
}

void xaeSound::pause()
{
    xaeResourceObject::LockScope oLock(*this);
    if(0 != m_hChannel)
    {
        m_pHGE->Channel_Pause(m_hChannel);
    }
}

void xaeSound::slide_to(float time, int volume, int pan /* = -101 */, float pitch /* = -1.0f */)
{
    xaeResourceObject::LockScope oLock(*this);
    if(volume >= 0 && volume <= 100) m_nVolume = volume;
    if(-101 != pan) m_nPan = pan;
    if(-1.0f != pitch) m_fPitch = pitch;

    if(m_hChannel != 0)
    {
        m_pHGE->Channel_SlideTo(m_hChannel, time, m_nVolume, m_nPan, m_fPitch);
    }
}
bool xaeSound::is_sliding()
{
    xaeResourceObject::LockScope oLock(*this);
    if(m_hChannel != 0)
    {
        return m_pHGE->Channel_IsSliding(m_hChannel);
    }
    return false;
}

int xaeSound::get_volume()
{
    return m_nVolume;
}

void xaeSound::set_panning(int pan)
{
    xaeResourceObject::LockScope oLock(*this);
    if(m_hChannel != 0)
    {
        m_pHGE->Channel_SetPanning(m_hChannel, pan);
    }
}

void xaeSound::set_volume(int volume)
{
    xaeResourceObject::LockScope oLock(*this);
    if(m_hChannel != 0)
    {
        m_pHGE->Channel_SetVolume(m_hChannel, volume);
    }
}

void xaeSound::set_pitch(float pitch)
{
    xaeResourceObject::LockScope oLock(*this);
    if(m_hChannel != 0)
    {
        m_pHGE->Channel_SetPitch(m_hChannel, pitch);
    }
}

void xaeSound::set_loop(bool bLoop)
{
    xaeResourceObject::LockScope oLock(*this);
    m_bLoop = bLoop;
}

bool xaeSound::is_playing()
{
    bool ans = true;
    xaeResourceObject::LockScope oLock(*this);
    /** 未播放 */
    if(0 == m_hChannel) ans = false;
    else
    /** 暂停 */
    if(!m_pHGE->Channel_IsPlaying(m_hChannel)) ans = false;

    return ans;
}

int xaeSound::length()
{
    int len = 0;
    xaeResourceObject::LockScope oLock(*this);
    if(0 == m_hChannel) len = -1;
    else len = m_pHGE->Channel_GetLength(m_hChannel);

    return len;
}

float xaeSound::get_pos()
{
    float pos = 0;
    xaeResourceObject::LockScope oLock(*this);
    if(0 == m_hChannel) pos = -1.f;
    else pos = m_pHGE->Channel_GetPos(m_hChannel);

    return pos;
}

void xaeSound::set_pos(float fSecond)
{
    xaeResourceObject::LockScope oLock(*this);
    if(0 != m_hChannel)
    {
        m_pHGE->Channel_SetPos(m_hChannel, fSecond);
    }
}

void xaeSound::stop_all()
{
    HGE* m_pHGE = xae::Instance().get_core();
    m_pHGE->Channel_StopAll();
}

void xaeSound::pause_all()
{
    HGE* m_pHGE = xae::Instance().get_core();
    m_pHGE->Channel_PauseAll();
}

void xaeSound::resume_all()
{
    HGE* m_pHGE = xae::Instance().get_core();
    m_pHGE->Channel_ResumeAll();
}

void xaeSound::release()
{
    xaeResourceObject::LockScope oLock(*this);
    ::LockScope oScope(_eff_cs);
    if(0 != m_hChannel) m_pHGE->Channel_Stop(m_hChannel);
    map<string, _eff_time>::iterator it;
    for(it = _hEffect.begin(); it != _hEffect.end(); it++)
    {
        if(it->second.eff == m_hEffect)
        {
            it->second.time--;
            if(it->second.time == 0)
            {
                if(0 != m_hEffect)
                {
                    /** 释放音效句柄 */
                    m_pHGE->Effect_Free(m_hEffect);

                    /** 释放内存资源 */
                    xae::Instance().get_resource_mgr()->free_resource(it->first);
                }
                _hEffect.erase(it);
            }
            break;
        }
    }
    m_hChannel = 0;
    m_hEffect = 0;
}

void xaeSound::release_all()
{
    xaeSound::stop_all();
    HGE* m_pHGE = xae::Instance().get_core();

    ::LockScope oScope(_eff_cs);
    map<string, _eff_time>::iterator it;
    for(it = _hEffect.begin(); it != _hEffect.end(); it++)
    {
        if(0 != it->second.eff)
        {
            m_pHGE->Effect_Free(it->second.eff);
            xae::Instance().get_resource_mgr()->free_resource(it->first);
        }
    }
    _hEffect.clear();
}
