#include <CCFGReader.h>
#include <cstdlib>
#include <XStringFunc.h>
#include <xae.h>

CCFGReader::CCFGReader(void)
{
}

CCFGReader::CCFGReader(const char* filename)
{
    _BuildNode(filename);
}

CCFGReader::~CCFGReader(void)
{
}

void CCFGReader::SetFile(const char* filename, int size)
{
    /** ���ļ��� */
    if(size == 0) _BuildNode(filename);
    else
    /** ���ڴ�� */
    {
        _BuildNodeFromMemo(filename, size);
    }
}

void CCFGReader::_BuildNodeFromMemo(const char* memo, DWORD size)
{
    /** �����һ������ */
    dict.clear();

    string buf;
    int pos = 0;
    for(size_t i = 0; i < size; i++)
    {
        if(memo[i] != '\r' && memo[i] != '\n')
        {
            buf += memo[i];
            continue;
        }
        else
        {
            if(buf.empty()) continue;

            /** Ѱ��"#"��ɾ��������� */
            int pos = buf.find("#");
            if(pos != string::npos)
            {
                buf = buf.substr(0, pos);
            }

            /** Ѱ��"="����ȡ�������� */
            string left, right;
            pos = buf.find("=");
            if(pos == string::npos) continue;
            left = buf.substr(0, pos);
            right = buf.substr(pos + 1, buf.length());

            /** ��ʽ�� */
            XStringFunc l, r;
            l.SetString(left);
            r.SetString(right);
            l.LRTrim();
            r.LRTrim();
            l.ToUpper();
            if(l.GetString().length() == 0) continue;

            /** д��dict */
            dict[l.GetString()] = r.GetString();

            buf = "";
        }
    }
}

void CCFGReader::_BuildNode(const char* filename)
{
    /** ���ļ� */
    string fn = filename;
    while(fn[0] == '\\' || fn[0] == '/') fn.erase(fn.begin());

    ifstream ifs;
    ifs.open(fn.c_str(), ios::in);
    if(!ifs.is_open())
    {
        //printf("[ERROR] ����CFG�ļ� \"%s\" ����\n", filename);
        xae::Instance().alert("Can't load [%s].", "Error", MB_ICONERROR);
        xae::Instance().get_core()->System_Log("[x] Can't load [%s].", filename);
        exit(0);
    }

    /** ����ϴ����� */
    dict.clear();

    /** ��ȡ�ļ� */
    string buf;
    while(!ifs.eof())
    {
        /** �����ַ��� */
        getline(ifs, buf);

        /** Ѱ��"#"��ɾ��������� */
        int pos = buf.find("#");
        if(pos != string::npos)
        {
            buf = buf.substr(0, pos);
        }

        /** Ѱ��"="����ȡ�������� */
        string left, right;
        pos = buf.find("=");
        if(pos == string::npos) continue;
        left = buf.substr(0, pos);
        right = buf.substr(pos + 1, buf.length());

        /** ��ʽ�� */
        XStringFunc l, r;
        l.SetString(left);
        r.SetString(right);
        l.LRTrim();
        r.LRTrim();
        l.ToUpper();
        if(l.GetString().length() == 0) continue;

        /** д��dict */
        dict[l.GetString()] = r.GetString();
    }

    ifs.close();
}

string CCFGReader::GetString(const char* key)
{
    XStringFunc sf(key);
    sf.ToUpper();

    if(dict.find(sf.GetString()) == dict.end()) return "";
    return dict[key];
}

int CCFGReader::GetString(const char* key, const char* split, string out[], int maxcount)
{
    string value = GetString(key);
    XStringFunc sf(value);
    return sf.Split(split, out, maxcount);
}

int CCFGReader::GetInt(const char* key)
{
    string value = GetString(key);
    return XStringFunc::ToInt(value);
}

int CCFGReader::GetInt(const char* key, const char* split, int out[], int maxcount)
{
    string arr[1000];
    XStringFunc sf(GetString(key));
    int count = sf.Split(split, arr, maxcount);
    sf.ToInt(arr, out, maxcount);

    return count;
}

float CCFGReader::GetFloat(const char* key)
{
    string value = GetString(key);
    return XStringFunc::ToDouble(value);
}

float CCFGReader::GetFloat(const char* key, const char* split, float out[], int maxcount)
{
    string arr[1000];
    double darr[1000];
    XStringFunc sf(GetString(key));
    int count = sf.Split(split, arr, maxcount);
    sf.ToDouble(arr, darr, maxcount);
    for(int i = 0; i < count; i++) out[i] = darr[i];

    return count;
}
