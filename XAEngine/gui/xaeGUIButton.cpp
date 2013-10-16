#include <gui/xaeGUIButton.h>
#include <xaeSpriteLoader.h>
#include <xaeSoundLoader.h>

xaeGUIButton::xaeGUIButton(int id, string key, string path_format, int x, int y, string sound_hover, string sound_click, string enter_mouse) :
    m_pSoundHover(NULL),
    m_pSoundClick(NULL),
    m_szEnterMouse(enter_mouse)
{
    char* path = new char[path_format.length() + 11];
    for(int i = 0; i < 3; i++)
    {
        sprintf(path, path_format.c_str(), i);

        /** 查找'*' */
        string szPath = path;
        int pos = szPath.find('*');
        if(string::npos != pos)
        {
            string type = szPath.substr(0, pos);
            string name = szPath.substr(pos + 1, szPath.length());
            m_pSprite[i] = xaeSpriteLoader::Instance().get_sprite(type, name);
        }
        else m_pSprite[i] = new xaeSprite(path);
    }

    delete []path;

    set_pos(x, y);
    set_size(m_pSprite[0]->get_sprite()->GetWidth(), m_pSprite[0]->get_sprite()->GetHeight());

    m_dwControlID = id;
    m_szControlKey = key;

    /** 声音 */
    if(sound_hover != "")
    {
        /** 查找'*' */
        string szPath = sound_hover;
        int pos = szPath.find('*');
        if(string::npos != pos)
        {
            string type = szPath.substr(0, pos);
            string name = szPath.substr(pos + 1, szPath.length());
            m_pSoundHover = xaeSoundLoader::Instance().get_sound(type, name);
        }
        else m_pSoundHover = new xaeSound(sound_hover);
    }
    if(sound_click != "")
    {
        /** 查找'*' */
        string szPath = sound_click;
        int pos = szPath.find('*');
        if(string::npos != pos)
        {
            string type = szPath.substr(0, pos);
            string name = szPath.substr(pos + 1, szPath.length());
            m_pSoundHover = xaeSoundLoader::Instance().get_sound(type, name);
        }
        else m_pSoundHover = new xaeSound(sound_click);
    }
}

xaeGUIButton::~xaeGUIButton()
{
    for(int i = 0; i < 3; i++)
    {
        m_pSprite[i]->release();
        SAFEDEL(m_pSprite[i]);
    }

    SAFEDEL(m_pSoundHover);
    SAFEDEL(m_pSoundClick);
}

void xaeGUIButton::on_render(int x, int y)
{
    if(!m_bMouseEnter)
    {
        m_pSprite[0]->render(x + m_tagRect.x1, y + m_tagRect.y1);
    }
    else
    {
        if(!m_bMouseLButtonDown)
        {
            m_pSprite[1]->render(x + m_tagRect.x1, y + m_tagRect.y1);
        }
        else m_pSprite[2]->render(x + m_tagRect.x1, y + m_tagRect.y1);
    }

    //m_pHGE->Gfx_RenderLine(x + m_tagRect.x1, y + m_tagRect.y1, x + m_tagRect.x2, y + m_tagRect.y1);
    //m_pHGE->Gfx_RenderLine(x + m_tagRect.x2, y + m_tagRect.y1, x + m_tagRect.x2, y + m_tagRect.y2);
    //m_pHGE->Gfx_RenderLine(x + m_tagRect.x2, y + m_tagRect.y2, x + m_tagRect.x1, y + m_tagRect.y2);
    //m_pHGE->Gfx_RenderLine(x + m_tagRect.x1, y + m_tagRect.y2, x + m_tagRect.x1, y + m_tagRect.y1);
}

void xaeGUIButton::on_update(float fDT, int nMX, int nMY)
{
}

void xaeGUIButton::on_mouse_enter()
{
    if(NULL != m_pSoundHover)
    {
        m_pSoundHover->play(true);
    }

    /** 设置鼠标 */
    m_tagPreMouse = xaeGUIObject::get_mouse();
    (m_szEnterMouse == "") ? set_mouse(IDC_HAND) : set_mouse(m_szEnterMouse);
}

void xaeGUIButton::on_mouse_leave()
{
    if(m_tagPreMouse.bSprite) set_mouse(m_tagPreMouse.szPath);
    else set_mouse(m_tagPreMouse.uMouse.nMouse);
}

void xaeGUIButton::on_mouse_lbutton_clicked()
{
    if(NULL != m_pSoundClick)
    {
        m_pSoundClick->play(true);
    }
}