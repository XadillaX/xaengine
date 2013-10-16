#include "xaeAVGTextbox.h"

xaeAVGTextbox::xaeAVGTextbox(void) :
    m_pTextbox(NULL),
    m_szDisplayTextBuffer(""),
    m_szDisplayText(""),
    m_szCurrentTalker(""),
    m_pTalkerFont(NULL),
    m_fCharTimer(g_Setting.AVG.m_fTextPerTimer),
    m_pVoice(NULL)
{
    m_pHGE = hgeCreate(HGE_VERSION);

    __loadResource();
}

xaeAVGTextbox::~xaeAVGTextbox(void)
{
    SAFEDEL(m_pTextbox);
    SAFEDEL(m_pTalkerFont);
    SAFEDEL(m_pTextFont);

    SAFEDEL(m_pVoice);

    m_pHGE->Release();
}

void xaeAVGTextbox::__loadResource()
{
    /** �Ի��򱳾� */
    m_pTextbox = xaeSpriteLoader::Instance().get_sprite("AVG", "textbox");

    /** ˵�������� */
    m_pTalkerFont = new xaeFont(g_szDefaultFont.c_str(), g_Setting.AVG.m_nTalkerSize);
    m_pTalkerFont->SetColor(g_Setting.AVG.m_dwTalkerColor);

    /** �������� */
    m_pTextFont = new xaeFont(g_szDefaultFont.c_str(), g_Setting.AVG.m_nTextSize);
    m_pTextFont->SetColor(g_Setting.AVG.m_dwTextColor);
}

void xaeAVGTextbox::setTalker(string szTalker)
{
    m_szCurrentTalker = szTalker;
}

void xaeAVGTextbox::addText(string szText)
{
    m_szDisplayTextBuffer += szText;
    m_fCharTimer = g_Setting.AVG.m_fTextPerTimer;
}

void xaeAVGTextbox::clearText()
{
    m_szDisplayText = m_szDisplayTextBuffer = "";
    m_fCharTimer = g_Setting.AVG.m_fTextPerTimer;
}

void xaeAVGTextbox::setVoice(string szFilename)
{
    clearVoice();

    m_pVoice = new xaeSound(szFilename, false, g_nVolume);
    m_pVoice->play();
}

void xaeAVGTextbox::clearVoice()
{
    if(m_pVoice)
    {
        m_pVoice->stop();
        m_pVoice->release();
        delete m_pVoice;
        m_pVoice = NULL;
    }
}

bool xaeAVGTextbox::hasTextbox()
{
    if(m_szCurrentTalker != "" || m_szDisplayTextBuffer != "") return true;

    return false;
}

string xaeAVGTextbox::formatTextboxString(string szText)
{
    /** Ѱ�Ұٷֺ� */
    string n_content = "";
    for(size_t i = 0; i < szText.length(); i++)
    {
        n_content += szText[i];
        if(szText[i] == '%') n_content += '%';
    }
    szText = n_content;

    vector<string> strs;
    strs.push_back("");
    int line_cnt = 0;
    for(size_t i = 0; i < szText.length(); i++)
    {
        strs[line_cnt] += szText[i];

        /** �����ַ� */
        if(szText[i] < 0)
        {
            strs[line_cnt] += szText[++i];
        }

        if(strs[line_cnt].length() >= g_Setting.AVG.m_nTextMaxPerLine)
        {
            line_cnt++;
            strs.push_back("");
        }
    }

    string szStr = "";
    for(size_t i = 0; i < strs.size(); i++)
    {
        if("" != strs[i])
        {
            szStr += strs[i];
            szStr += "\n";
        }
    }

    return szStr;
}

void xaeAVGTextbox::render()
{
    /** �Ի��� */
    if(m_szCurrentTalker != "" || m_szDisplayTextBuffer != "")
    {
        m_pTextbox->render();
    }

    /** ˵���� */
    if(m_szCurrentTalker != "")
    {
        SIZE m_szTalkerSize = m_pTalkerFont->GetTextSize(C2W(m_szCurrentTalker.c_str()));
        int left = g_Setting.AVG.m_nTalkerLeft + (g_Setting.AVG.m_nTalkerRight - g_Setting.AVG.m_nTalkerLeft - m_szTalkerSize.cx) / 2;
        m_pTalkerFont->Printf(left, g_Setting.AVG.m_nTalkerTop, C2W(m_szCurrentTalker.c_str()));
    }

    /** �ı� */
    if(m_szDisplayText != "")
    {
        m_pTextFont->Printf(g_Setting.AVG.m_nTextLeft, g_Setting.AVG.m_nTextTop, C2W(formatTextboxString(m_szDisplayText).c_str()));
    }
}

XAE_AVG_DISPLAY_STATUS xaeAVGTextbox::update(float fDT)
{
    /** ����û���ֻ���������ʾ��� */
    if(m_szDisplayTextBuffer == "" || m_szDisplayTextBuffer == m_szDisplayText)
    {
        /** ��������û�в����� */
        if(m_pVoice != NULL)
        {
            if(m_pVoice->is_playing()) return XADS_PLAYING_VOICE;
        }

        return XADS_PAUSED;
    }

    /** ����ڿ��ڵ�� */
    float mx, my;
    m_pHGE->Input_GetMousePos(&mx, &my);

    if(
        (my >= g_Setting.AVG.m_nTextboxTop && my <= g_Setting.AVG.m_nTextboxBottom &&
        mx >= g_Setting.AVG.m_nTextboxLeft && mx <= g_Setting.AVG.m_nTextboxRight &&
        xae::Instance().keyup(HGEK_LBUTTON)) ||
        xae::Instance().keyup(HGEK_SPACE) ||
        xae::Instance().keyup(HGEK_ENTER)
      )
    {
        m_fCharTimer = g_Setting.AVG.m_fTextPerTimer;
        m_szDisplayText = m_szDisplayTextBuffer;
        return XADS_PAUSED;
    }

    /** ������ʾ */
    m_fCharTimer -= fDT;
    if(m_fCharTimer < 0.0f)
    {
        m_fCharTimer = g_Setting.AVG.m_fTextPerTimer;

        /** ��ʾ��һ������ */
        m_szDisplayText += m_szDisplayTextBuffer[m_szDisplayText.length()];
        if(m_szDisplayText[m_szDisplayText.length() - 1] < 0)
        {
            m_szDisplayText += m_szDisplayTextBuffer[m_szDisplayText.length()];
        }
    }

    return XADS_SHOWING_TEXT;
}
