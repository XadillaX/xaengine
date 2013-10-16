#include <gui/xaeGUIStaticImage.h>
#include <xaeSpriteLoader.h>

xaeGUIStaticImage::xaeGUIStaticImage(int id, string key, string path, int x, int y, int w, int h)
{
    m_dwControlID = id;
    m_szControlKey = key;
    m_pImage = NULL;

    /** ÔØÈë×ÊÔ´ */
    int pos;
    if((pos = path.find("*")) != string::npos)
    {
        string type = path.substr(0, pos);
        string name = path.substr(pos + 1, path.length());
        
        if(!w) w = XAE_SPRITE_LDR_VALUE_DEFAULT;
        if(!h) h = XAE_SPRITE_LDR_VALUE_DEFAULT;
        m_pImage = xaeSpriteLoader::Instance().get_sprite(type, name, 0, 0, w, h);
    }
    else
    {
        m_pImage = new xaeSprite(path.c_str(), 0, 0, w, h);
    }

    if(m_pImage && m_pImage->get_sprite())
    {
        w = m_pImage->get_sprite()->GetWidth();
        h = m_pImage->get_sprite()->GetHeight();
    }

    set_size(w, h);
    set_pos(x, y);
}

xaeGUIStaticImage::~xaeGUIStaticImage()
{
    if(m_pImage)
    {
        m_pImage->release();
        SAFEDEL(m_pImage);
    }
}

void xaeGUIStaticImage::on_render(int x, int y)
{
    if(m_pImage && m_pImage->get_sprite())
    {
        m_pImage->render(x + m_tagRect.x1, y + m_tagRect.y1);
    }
}
