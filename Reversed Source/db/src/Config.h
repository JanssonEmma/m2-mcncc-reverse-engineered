#ifndef __INC_CONFIG_H__
#define __INC_CONFIG_H__

typedef std::map<std::string, std::string> TValueMap;

class CConfig : public singleton<CConfig>
{
public:
    CConfig();
    ~CConfig();

    bool LoadFile(const char* filename);
    bool GetValue(const char* key, int* dest);
    bool GetValue(const char* key, float* dest);
    bool GetValue(const char* key, DWORD* dest);
    bool GetValue(const char* key, BYTE* dest);
    bool GetValue(const char* key, char* dest, int size);
    bool GetTwoValue(const char* key, DWORD* first, DWORD* second);
    void NextLine(FILE* fp);

private:
    bool GetWord(FILE* fp, std::string& dest);
    bool GetLine(FILE* fp, std::string& dest);
    void Destroy();
    bool GetParam(const char* key, int index, DWORD* dest);
    const char* Get(const char* key);
    std::string* Search(const char* key);

    TValueMap m_valueMap;
};

#endif
