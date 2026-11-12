// Complete old CBanwordManager logical class (banword.h:3), target old size24.
#ifndef BANWORD_MANAGER_H_
#define BANWORD_MANAGER_H_
#include <string>
#include <cstddef>
#include <ext/hash_map>
#include "../../common/singleton.h"
#include "../../common/length.h"
#include "../../common/tables.h"
class CBanwordManager : public singleton<CBanwordManager>
{
public:
    CBanwordManager();
    virtual ~CBanwordManager();
    bool Initialize(TBanwordTable* p,WORD wSize);
    bool Find(const char* c_pszString);
    bool CheckString(const char* c_pszString);
    void ConvertString(char* c_pszString);
protected:
    struct stringhash {
        std::size_t operator()(const std::string& value) const {
            unsigned int h=0;
            for(std::size_t i=0;i<value.size();++i)
                h=h*16777619U ^ static_cast<unsigned char>(value[i]);
            return h;
        }
    };
    typedef __gnu_cxx::hash_map<std::string,bool,stringhash> TBanwordHashmap;
    TBanwordHashmap m_hashmap_words;
};
#endif
