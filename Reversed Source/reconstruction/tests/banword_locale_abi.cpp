#include <string>
#include <map>
#include <set>
#include <list>
#include <vector>
#include <queue>
#include <ext/hash_map>
#define protected public
#define private public
#include "../../game/src/banword.h"
#include "../../game/src/desc.h"
#undef private
#undef protected
#include "../../game/src/utils.h"
#include "../../game/src/locale_service.h"
#include "../../game/src/cmd_gm.cpp"
#include <cstddef>
#include "banword_locale_api_checks.h"
#define BL_ASSERT(n,e) typedef char bl_assert_##n[(e)?1:-1]
BL_ASSERT(int32,sizeof(int)==4);
BL_ASSERT(pointer32,sizeof(void*)==4);
BL_ASSERT(size32,sizeof(std::size_t)==4);
BL_ASSERT(word16,sizeof(WORD)==2);
BL_ASSERT(banword25,sizeof(SBanwordTable)==25 && sizeof(TBanwordTable)==25);
BL_ASSERT(word0,offsetof(SBanwordTable,szWord)==0);
BL_ASSERT(word25,sizeof(((SBanwordTable*)0)->szWord)==25);
BL_ASSERT(hash_empty,sizeof(CBanwordManager::stringhash)==1);
BL_ASSERT(base4,sizeof(singleton<CBanwordManager>)==4);
// Full logical field and exact container/hash/allocator type; old GNU STL
// object size24 is evidence, not a padding claim on modern libstdc++.
typedef __gnu_cxx::hash_map<std::string,bool,CBanwordManager::stringhash,
    std::equal_to<std::string>,std::allocator<bool> > ExpectedBanwords;
static ExpectedBanwords CBanwordManager::*const member=&CBanwordManager::m_hashmap_words;
BL_ASSERT(no_extra_tail,sizeof(CBanwordManager)==offsetof(CBanwordManager,m_hashmap_words)+sizeof(ExpectedBanwords));
BL_ASSERT(words_after_base,offsetof(CBanwordManager,m_hashmap_words)>=sizeof(singleton<CBanwordManager>));
BL_ASSERT(log_record4,sizeof(log_packet_func)==4);
BL_ASSERT(log_pointer0,offsetof(log_packet_func,m_str)==0);
static std::size_t(CBanwordManager::stringhash::*const hashApi)(const std::string&)const=
    &CBanwordManager::stringhash::operator();
void banword_locale_abi_signatures(){BanwordLocaleApiChecks();(void)member;(void)hashApi;}
