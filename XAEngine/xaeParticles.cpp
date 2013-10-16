#include <xaeParticles.h>
#include <xae.h>

xaeParticles::xaeParticles() :
    m_dwStartColor(-1),
    m_dwEndColor(-1),
    m_nBlendMode(PAR_BLEND_DEFAULT),
    m_fScale(1.0f),
    m_pPar(NULL)
{
    m_pPar = new hgeParticleSystem(&m_PSI);
}

xaeParticles::xaeParticles(const char* path) :
    m_dwStartColor(-1),
    m_dwEndColor(-1),
    m_nBlendMode(PAR_BLEND_DEFAULT),
    m_fScale(1.0f),
    m_pPar(NULL)
{
    m_pPar = new hgeParticleSystem(&m_PSI);
    if(false == load_resource(path))
    {
        xae::Instance().alert("Can't load resource.");
        exit(0);
    }
}

xaeParticles::~xaeParticles()
{
    release();
}

void xaeParticles::release()
{
    SAFEDEL(m_pPar);
    xaeSprite::release();
}

bool xaeParticles::load_resource(string path)
{
    string new_path = xaeResourceMgrObject::path_to_upper(path);
    /** ���4λ��.CFG */
    if(new_path.find(".CFG", new_path.length() - 4) != string::npos)
    {
        return load_resource_from_cfg(path);
    }
    else
        /** ����.CFG */
    {
        return load_resource_from_pic(new_path);
    }

    return true;
}

bool xaeParticles::load_resource(string path, int x, int y, int w, int h)
{
    /** ��·��ת��Ϊ��д */
    string new_path = xaeResourceMgrObject::path_to_upper(path);

    /** ���4λ��.CFG */
    if(new_path.find(".CFG", new_path.length() - 4) != string::npos)
    {
        return load_resource_from_cfg(path);
    }
    else
    /** ����.CFG */
    {
        return load_resource_from_pic(new_path, x, y, w, h);
    }
}

bool xaeParticles::load_resource_from_cfg(string path)
{
    path = xaeResourceMgrObject::path_to_upper(path);
    /** ��Դ������������cfg�ļ� */
    xaeResourceBody RB;
    xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
    bool res = pRMO->get_resource_bin(path, RB);
    if(!res)
    {
        /** ������� */
        return false;
    }

    /** ��CFG�ļ� */
    CCFGReader reader;
    reader.SetFile((char*)RB.m_pRes, RB.m_nSize);

    /** ���� */
    string psi_filename = reader.GetString("PSI");
    string tex_filename = reader.GetString("TEXTURE");
    int x = reader.GetInt("X");
    int y = reader.GetInt("Y");
    int w = reader.GetInt("W");
    int h = reader.GetInt("H");

    int startcolor = reader.GetInt("STARTCOLOR");
    int endcolor = reader.GetInt("ENDCOLOR");
    float scale = reader.GetFloat("SCALE");
    string blend = reader.GetString("BLENDMODE");
    set_blend_mode(blend);

    if(tex_filename.length() == 0 || psi_filename.length() == 0)
    {
        m_pHGE->System_Log("[!] The path of filename to create a particle is NULL.");
        return false;
    }

    /** ���·������ʲô */
    if('/' != tex_filename[0] && '\\' != tex_filename[0])
    {
        /** ��ȡcfg�ļ���·�� */
        int len = 0;
        for(int i = path.length() - 1; i >= 0; i--)
        {
            if(path[i] == '\\' || path[i] == '/')
            {
                len = i + 1;
                break;
            }
        }

        /** ����cfg·�� */
        tex_filename = path.substr(0, len) + tex_filename;
    }
    
    if('/' != psi_filename[0] && '\\' != psi_filename[0])
    {
        /** ��ȡcfg�ļ���·�� */
        int len = 0;
        for(int i = path.length() - 1; i >= 0; i--)
        {
            if(path[i] == '\\' || path[i] == '/')
            {
                len = i + 1;
                break;
            }
        }

        /** ����cfg·�� */
        psi_filename = path.substr(0, len) + psi_filename;
    }

    /** ��ȡ */
    return load_resource_from_pic(tex_filename, x, y, w, h) & load_resource_from_psi(psi_filename, startcolor, endcolor, m_nBlendMode, scale);

    return true;
}

bool xaeParticles::load_resource_from_pic(string path, int x, int y, int w, int h)
{
    bool res = xaeSprite::load_resource_from_pic(path, x, y, w, h);
    if(!res) return false;
    else
    {
        m_PSI.sprite = m_pSprite;
        m_pPar->info.sprite = m_pSprite;
        set_blend_mode(m_nBlendMode);
        return true;
    }

    return true;
}

bool xaeParticles::load_resource_from_psi(string path, DWORD startcolor, DWORD endcolor, int blend, float scale)
{
    /** һЩ���� */
    m_dwStartColor = startcolor;
    m_dwEndColor = endcolor;
    m_nBlendMode = blend;
    m_fScale = scale;

    path = xaeResourceMgrObject::path_to_upper(path);
    /** ����Դ������������ */
    xaeResourceBody RB;
    xaeResourceMgrObject* pRMO = xae::Instance().get_resource_mgr();
    bool res = pRMO->get_resource_bin(path, RB);

    /** ����ɹ� */
    if(res)
    {
        /** �����ٽ��� */
        xaeResourceObject::LockScope oLock(*this);

        /** ɾ����ǰ���� */
        //SAFEDEL(m_pPar);
        
        /** �½����� */
        //char* pMem = new char[sizeof(hgeParticleSystem)];
        memcpy(&(m_pPar->info), RB.m_pRes, RB.m_nSize);
        //m_pPar.in = (hgeParticleSystem*)pMem;

        /** ���Ӳ��� */
        if(-1 != m_dwStartColor) m_pPar->info.colColorStart = m_dwStartColor;
        if(-1 != m_dwEndColor) m_pPar->info.colColorEnd = m_dwEndColor;
        m_pPar->SetScale(m_fScale);
        set_blend_mode(m_nBlendMode);

        /** �������Ӿ��� */
        if(NULL != m_pSprite) m_PSI.sprite = m_pPar->info.sprite = m_pSprite;

        return true;
    }
    else
    /** ����ʧ�� */
    {
        return false;
    }

    return true;
}

void xaeParticles::set_blend_mode(int blend_mode)
{
    m_nBlendMode = blend_mode;
    if(NULL != m_pSprite) m_pSprite->SetBlendMode(m_nBlendMode);
}

void xaeParticles::set_blend_mode(string blend_mode)
{
    /** �ָ��ַ��� */
    XStringFunc xstr(blend_mode);
    string mode[3];
    xstr.ToUpper();
    int cnt = xstr.Split("|", mode, 3);

    int nMode = 0;
    /** ��ʼ���� */
    for(int i = 0; i < cnt; i++)
    {
        if(mode[i] == "COLORMUL") nMode |= BLEND_COLORMUL;
        else
        if(mode[i] == "COLORADD") nMode |= BLEND_COLORADD;
        else
        if(mode[i] == "ALPHABLEND") nMode |= BLEND_ALPHABLEND;
        else
        if(mode[i] == "ALPHAADD") nMode |= BLEND_ALPHAADD;
        else
        if(mode[i] == "ZWRITE") nMode |= BLEND_ZWRITE;
        else
        if(mode[i] == "NOZWRITE") nMode |= BLEND_NOZWRITE;
    }

    set_blend_mode(nMode);
}

void xaeParticles::fire()
{
    /** �����ٽ��� */
    xaeResourceObject::LockScope oLock(*this);

    if(NULL != m_pPar) m_pPar->Fire();
    else
    {
        m_pHGE->System_Log("[!] A particle be fired is not completed.");
    }
}

void xaeParticles::fire_at(int x, int y)
{
    /** �����ٽ��� */
    xaeResourceObject::LockScope oLock(*this);

    if(NULL != m_pPar) m_pPar->FireAt(x, y);
    else
    {
        m_pHGE->System_Log("[!] A particle be fired is not completed.");
    }
}

void xaeParticles::move_to(int x, int y)
{
    /** �����ٽ��� */
    xaeResourceObject::LockScope oLock(*this);

    if(NULL != m_pPar) m_pPar->MoveTo(x, y);
    else
    {
        m_pHGE->System_Log("[!] A particle be moved is not completed.");
    }
}

void xaeParticles::get_pos(int* x, int* y)
{
    /** �����ٽ��� */
    xaeResourceObject::LockScope oLock(*this);
    *x = *y = 0;

    if(NULL != m_pPar)
    {
        float fx, fy;
        m_pPar->GetPosition(&fx, &fy);
        *x = fx, *y = fy;
    }
    else
    {
        m_pHGE->System_Log("[!] A particle be got position is not completed.");
    }
}

void xaeParticles::update(float fDT)
{
    /** �����ٽ��� */
    xaeResourceObject::LockScope oLock(*this);

    if(NULL != m_pPar) m_pPar->Update(fDT);
    else
    {
        m_pHGE->System_Log("[!] A particle be updated is not completed.");
    }
}

void xaeParticles::render()
{
    /** �����ٽ��� */
    xaeResourceObject::LockScope oLock(*this);

    if(NULL != m_pPar) m_pPar->Render();
    else
    {
        m_pHGE->System_Log("[!] A particle be rendered is not completed.");
    }
}
