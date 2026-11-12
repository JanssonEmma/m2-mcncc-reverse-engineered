// Complete old application bodies; no newer length overloads.
#include "banword.h"
#include "config.h"
#include <cstdio>
#include <cstring>
extern void SendLog(const char*);
CBanwordManager::CBanwordManager() {}
CBanwordManager::~CBanwordManager() {}
bool CBanwordManager::Initialize(TBanwordTable* p,WORD wSize)
{
    m_hashmap_words.clear();
    for(WORD i=0;i<wSize;++i,++p) m_hashmap_words[p->szWord]=true;
    char szBuf[257];
    std::snprintf(szBuf,256,"Banword reloaded! (total %d banwords)",
                  static_cast<int>(m_hashmap_words.size()));
    SendLog(szBuf);
    return true;
}
bool CBanwordManager::Find(const char* c_pszString)
{
    return m_hashmap_words.end()!=m_hashmap_words.find(c_pszString);
}
bool CBanwordManager::CheckString(const char* c_pszString)
{
    if(m_hashmap_words.empty()) return false;
    for(TBanwordHashmap::iterator it=m_hashmap_words.begin();it!=m_hashmap_words.end();++it) {
        const std::string& r=it->first;
        const char* tmp=c_pszString;
        while(*tmp) {
            if(is_twobyte(tmp)) {
                if(!std::strncmp(tmp,r.c_str(),r.size())) return true;
                tmp+=2;
            } else {
                if(!std::strncmp(tmp,r.c_str(),r.size())) return true;
                ++tmp;
            }
        }
    }
    return false;
}
void CBanwordManager::ConvertString(char* c_pszString)
{
    for(TBanwordHashmap::iterator it=m_hashmap_words.begin();it!=m_hashmap_words.end();++it) {
        const std::string& r=it->first;
        char* tmp=c_pszString;
        while(*tmp) {
            if(is_twobyte(tmp)) {
                if(!std::strncmp(tmp,r.c_str(),r.size())) {
                    std::memset(tmp,'*',r.size());tmp+=r.size();
                } else tmp+=2;
            } else {
                if(*tmp=='*') { ++tmp;continue; }
                if(!std::strncmp(tmp,r.c_str(),r.size())) {
                    std::memset(tmp,'*',r.size());tmp+=r.size();
                } else ++tmp;
            }
        }
    }
}
