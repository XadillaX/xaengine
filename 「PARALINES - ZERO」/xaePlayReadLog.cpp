#include "xaePlayReadLog.h"
#include <xaeSpriteLoader.h>

#define XAE_PLAY_READ_LOG_LINE_MAX (45 * 2)

xaePlayReadLog::xaePlayReadLog(void) :
    m_nTotPage(0),
    m_nCurrentPage(0),
    m_pBG(NULL)
{
    m_pHGE = hgeCreate(HGE_VERSION);
    m_pPageIdx = new xaeGUIObject();

    m_nTotPage = 1;

    /** 载入背景 */
    m_pBG = xaeSpriteLoader::Instance().get_sprite("UI", "Reader");

    /** 字体 */
    m_pDateFont = new xaeFont("宋体", 14, true, false, false);
    m_pDateFont->SetColor(0xff0000ff);
    m_pDateFont->SetKerningWidth(2);
    m_pIdxFont = new xaeFont("宋体", 12, false, false, false);
    m_pContentFont = new xaeFont("宋体", 14, false, false, false);
    m_pContentFont->SetColor(0xff000000);

    /** 关闭按钮 */
    m_pPageIdx->add_control(new xaeGUIButton(1000, "close", "UI*readerclose%d", ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1) + GAME_REAL_WIDTH - 35, ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + 16, "", "", "media/cursor_point.png"));
    m_pPageIdx->add_control(new xaeGUIButton(1001, "prev", "UI*prev%d", 0, 0, "", "", "media/cursor_point.png"));
    m_pPageIdx->add_control(new xaeGUIButton(1002, "next", "UI*next%d", 0, 0, "", "", "media/cursor_point.png"));

    ReloadIdxGUI();
}

xaePlayReadLog::~xaePlayReadLog(void)
{
    if(m_pBG)
    {
        m_pBG->release();
        SAFEDEL(m_pBG);
    }

    SAFEDEL(m_pDateFont);
    SAFEDEL(m_pIdxFont);
    SAFEDEL(m_pContentFont);
    
    SAFEDEL(m_pPageIdx);

    m_pHGE->Release();
}

void xaePlayReadLog::SetDate(string date, string week, string weather)
{
    m_tagDate.Date = date;
    m_tagDate.Week = week;
    m_tagDate.Weather = weather;
}

void xaePlayReadLog::AddLine(string line)
{
    string str = "";
    for(size_t i = 0; i < line.length(); i++)
    {
        str += line[i];
        if(line[i] < 0) str += line[++i];

        if(str.length() >= XAE_PLAY_READ_LOG_LINE_MAX)
        {
            m_szContent.push_back(str);
            str = "";
        }
    }
    if(!str.empty())
    {
        m_szContent.push_back(str);
        str = "";
    }

    m_nTotPage = ((((m_szContent.size() == 0) ? 1 : m_szContent.size()) - 1) / LINES_PER_PAGE) + 1;

    ToLastPage();
}

void xaePlayReadLog::ReloadIdxGUI()
{
    /** 新建GUI */
    for(int i = 0; i < m_nTotPage; i++)
    {
        if(m_pPageIdx->get_control(i) == NULL)
        {
            char text[16];
            char key[16];
            sprintf(key, "Page%d", i);
            sprintf(text, "[%.2d]", i + 1);
            m_pPageIdx->add_control(new xaeSelectLabel(i, text, 0, 0, 0, 0, "宋体", 12, 0xff000000, 0xffff7800));
        }

        m_pPageIdx->get_control(i)->set_show_mode(false);
    }

    /** 查看当前页面 */
    bool bFind = false;
    for(size_t i = 0; i < m_nShowPageIdx.size(); i++)
    {
        if(m_nShowPageIdx[i] == m_nCurrentPage)
        {
            bFind = true;
            break;
        }
    }

    /** 重置显示页面索引 */
    if(!bFind)
    {
        m_nShowPageIdx.clear();
        for(int i = 0; i < 7 && i < m_nTotPage; i++)
        {
            m_nShowPageIdx.push_back(i);
        }
        m_nCurrentPage = 0;
        SetCurrentPage(0);
    }
    else
    {
        while(m_nShowPageIdx.size() < 7 && m_nTotPage > m_nShowPageIdx.size() && m_nShowPageIdx.back() + 1 < m_nTotPage)
        {
            m_nShowPageIdx.push_back(m_nShowPageIdx.back() + 1);
        }

        while(m_nShowPageIdx.size() < 7 && m_nTotPage > m_nShowPageIdx.size() && m_nShowPageIdx.back() + 1 >= 0)
        {
            m_nShowPageIdx.insert(m_nShowPageIdx.begin(), m_nShowPageIdx.front() - 1);
        }

        SetCurrentPage(m_nCurrentPage);
    }

    /** 重排索引 */
    SIZE sz = m_pIdxFont->GetTextSize(C2W("[01]"));
    int width = (sz.cx * m_nShowPageIdx.size()) + 5 * (m_nShowPageIdx.size() - 1);
    int startx = (GAME_WIDTH - width) >> 1;
    for(size_t i = 0; i < m_nShowPageIdx.size(); i++)
    {
        xaeSelectLabel* pBtn = (xaeSelectLabel*)m_pPageIdx->get_control(m_nShowPageIdx[i]);
        pBtn->set_show_mode(true);
        pBtn->set_pos(
            startx + (sz.cx + 5) * (i - 1),
            ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + 420);
    }

    /** 上一页 & 下一页 */
    m_pPageIdx->get_control("prev")->set_pos(
        startx - 45,
        ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + 420);
    m_pPageIdx->get_control("next")->set_pos(
        startx + width - 20,
        ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1) + 420);
}

void xaePlayReadLog::render()
{
    m_pBG->render((GAME_WIDTH - GAME_REAL_WIDTH) >> 1, (GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1);

    /** 日期 */
    m_pDateFont->Print(
        65 + ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1),
        63 + ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1),
        string(m_tagDate.Date + "  " + m_tagDate.Week + "  天气：" + m_tagDate.Weather).c_str());

    /** 文字 */
    int startline = m_nCurrentPage * LINES_PER_PAGE;
    int starty = 105 + ((GAME_HEIGHT - GAME_REAL_HEIGHT) >> 1);
    for(size_t i = startline, j = 0; i < startline + LINES_PER_PAGE && i < m_szContent.size(); i++, j++)
    {
        m_pContentFont->Print(
            65 + ((GAME_WIDTH - GAME_REAL_WIDTH) >> 1),
            starty + (j * 42),
            m_szContent[i].c_str()
            );
    }

    m_pPageIdx->render(0, 0);
    m_pPageIdx->render_mouse();
}

bool xaePlayReadLog::update(float fDT)
{
    float mx, my;
    m_pHGE->Input_GetMousePos(&mx, &my);
    m_pPageIdx->update(fDT, mx, my);

    if(m_pPageIdx->get_control(1000)->get_mouse_lbutton_clicked())
    {
        m_pPageIdx->get_control(1000)->set_enable_mode(false);
        m_pPageIdx->get_control(1000)->set_enable_mode(true);
        return true;
    }

    for(int i = 0; i < m_nTotPage; i++)
    {
        if(m_pPageIdx->get_control(i)->get_mouse_lbutton_clicked())
        {
            SetCurrentPage(i);
            break;
        }
    }

    /** 上一页 & 下一页 */
    if(m_pPageIdx->get_control("next")->get_mouse_lbutton_clicked())
    {
        if(m_nCurrentPage < m_nTotPage - 1)
        {
            SetCurrentPage(m_nCurrentPage + 1);

            if(m_nCurrentPage > m_nShowPageIdx.back())
            {
                m_nShowPageIdx.erase(m_nShowPageIdx.begin());
                m_nShowPageIdx.push_back(m_nCurrentPage);
            }
        }

        ReloadIdxGUI();
    }
    else
    if(m_pPageIdx->get_control("prev")->get_mouse_lbutton_clicked())
    {
        if(m_nCurrentPage > 0)
        {
            SetCurrentPage(m_nCurrentPage - 1);

            if(m_nCurrentPage < m_nShowPageIdx.front())
            {
                m_nShowPageIdx.pop_back();
                m_nShowPageIdx.insert(m_nShowPageIdx.begin(), m_nCurrentPage);
            }
        }

        ReloadIdxGUI();
    }

    return false;
}

void xaePlayReadLog::SetCurrentPage(int nPage)
{
    m_nCurrentPage = nPage;
    for(size_t i = 0; i < m_nShowPageIdx.size(); i++)
    {
        if(nPage != m_nShowPageIdx[i])
        {
            m_pPageIdx->get_control(m_nShowPageIdx[i])->set_enable_mode(true);
        }
        else m_pPageIdx->get_control(m_nShowPageIdx[i])->set_enable_mode(false);

        ((xaeSelectLabel*)m_pPageIdx->get_control(m_nShowPageIdx[i]))->set_color(0xff000000, 0xffff7800);
    }

    ((xaeSelectLabel*)m_pPageIdx->get_control(nPage))->set_color(0xffff0000, 0xffff0099);
}

void xaePlayReadLog::ToLastPage()
{
    for(int i = 0; i < m_nTotPage; i++)
    {
        if(NULL != m_pPageIdx->get_control(i))
        {
            m_pPageIdx->get_control(i)->set_show_mode(false);
        }
    }
    m_nCurrentPage = m_nTotPage - 1;
    
    m_nShowPageIdx.clear();

    for(int i = (m_nTotPage - 7 >= 0) ? (m_nTotPage - 7) : 0; i < m_nTotPage; i++)
    {
        m_nShowPageIdx.push_back(i);
    }

    ReloadIdxGUI();
}