// Complete 2009 blend module, including original parser failure behavior.
#include "blend_item.h"
#include "item.h"
#include "locale_service.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
struct BLEND_ITEM_INFO {
    DWORD item_vnum;
    int apply_type;
    int apply_value[MAX_BLEND_ITEM_VALUE];
    int apply_duration[MAX_BLEND_ITEM_VALUE];
};
typedef std::vector<BLEND_ITEM_INFO*> T_BLEND_ITEM_INFO;
T_BLEND_ITEM_INFO s_blend_info;

typedef struct SValueName {
    const char* c_pszName;
    long lValue;
} TValueName;
// Exact old c_aApplyTypeNames616 bytes, ordered77 entries including sentinel.
TValueName c_aApplyTypeNames[77] = {
    {"STR", 5},
    {"DEX", 6},
    {"CON", 3},
    {"INT", 4},
    {"MAX_HP", 1},
    {"MAX_SP", 2},
    {"MAX_STAMINA", 58},
    {"POISON_REDUCE", 41},
    {"EXP_DOUBLE_BONUS", 43},
    {"GOLD_DOUBLE_BONUS", 44},
    {"ITEM_DROP_BONUS", 45},
    {"HP_REGEN", 10},
    {"SP_REGEN", 11},
    {"ATTACK_SPEED", 7},
    {"MOVE_SPEED", 8},
    {"CAST_SPEED", 9},
    {"ATT_BONUS", 53},
    {"DEF_BONUS", 54},
    {"MAGIC_ATT_GRADE", 55},
    {"MAGIC_DEF_GRADE", 56},
    {"SKILL", 51},
    {"ATTBONUS_ANIMAL", 18},
    {"ATTBONUS_UNDEAD", 21},
    {"ATTBONUS_DEVIL", 22},
    {"ATTBONUS_HUMAN", 17},
    {"ADD_BOW_DISTANCE", 52},
    {"DODGE", 28},
    {"BLOCK", 27},
    {"RESIST_SWORD", 29},
    {"RESIST_TWOHAND", 30},
    {"RESIST_DAGGER", 31},
    {"RESIST_BELL", 32},
    {"RESIST_FAN", 33},
    {"RESIST_BOW", 34},
    {"RESIST_FIRE", 35},
    {"RESIST_ELEC", 36},
    {"RESIST_MAGIC", 37},
    {"RESIST_WIND", 38},
    {"POISON", 12},
    {"SLOW", 14},
    {"STUN", 13},
    {"STEAL_HP", 23},
    {"STEAL_SP", 24},
    {"MANA_BURN_PCT", 25},
    {"CRITICAL", 15},
    {"PENETRATE", 16},
    {"KILL_SP_RECOVER", 42},
    {"KILL_HP_RECOVER", 47},
    {"PENETRATE_PCT", 16},
    {"CRITICAL_PCT", 15},
    {"POISON_PCT", 12},
    {"STUN_PCT", 13},
    {"ATT_BONUS_TO_WARRIOR", 59},
    {"ATT_BONUS_TO_ASSASSIN", 60},
    {"ATT_BONUS_TO_SURA", 61},
    {"ATT_BONUS_TO_SHAMAN", 62},
    {"ATT_BONUS_TO_MONSTER", 63},
    {"ATT_BONUS_TO_MOB", 63},
    {"MALL_ATTBONUS", 64},
    {"MALL_EXPBONUS", 66},
    {"MALL_DEFBONUS", 65},
    {"MALL_ITEMBONUS", 67},
    {"MALL_GOLDBONUS", 68},
    {"MAX_HP_PCT", 69},
    {"MAX_SP_PCT", 70},
    {"SKILL_DAMAGE_BONUS", 71},
    {"NORMAL_HIT_DAMAGE_BONUS", 72},
    {"SKILL_DEFEND_BONUS", 73},
    {"NORMAL_HIT_DEFEND_BONUS", 74},
    {"PCBANG_EXP_BONUS", 75},
    {"PCBANG_DROP_BONUS", 76},
    {"RESIST_WARRIOR", 78},
    {"RESIST_ASSASSIN", 79},
    {"RESIST_SURA", 80},
    {"RESIST_SHAMAN", 81},
    {"INFINITE_AFFECT_DURATION", 536870911},
    {0, 0},
};

// 080e6010: original debug CU is blend_item.cpp, not newer constants.cpp.
static long FN_get_apply_type(char* apply_type_string) {
    for(TValueName* value_name=c_aApplyTypeNames;value_name->c_pszName;++value_name)
        if(!::strcasecmp(value_name->c_pszName,apply_type_string)) return value_name->lValue;
    return 0;
}
// 080e6060. Original sprintf/file_name[256] precondition retained.
bool Blend_Item_init() {
    char file_name[256];
    std::sprintf(file_name,"%s/blend.txt",LocaleService_GetBasePath().c_str());
    sys_log(0,"Blend_Item_init %s ",file_name);
    for(T_BLEND_ITEM_INFO::iterator it=s_blend_info.begin();it!=s_blend_info.end();++it) delete *it;
    s_blend_info.clear();
    bool result=Blend_Item_load(file_name);
    if(!result) _sys_err("Blend_Item_init",156,"<Blend_Item_init> fail");
    return result;
}
// 080e6130. Missing tokens intentionally return WITHOUT fclose/record cleanup.
// Original grammar assumes section precedes fields/end; pointer is not initialized.
bool Blend_Item_load(char* file) {
    if(!file || !*file) return false;
    FILE* fp=std::fopen(file,"r");
    if(!fp) return false;
    char one_line[256];
    BLEND_ITEM_INFO* blend_item_info;
    const char* delim=" \t\r\n";
    while(std::fgets(one_line,256,fp)) {
        if(one_line[0]=='#') continue;
        char* token_string=std::strtok(one_line,delim);
        if(!token_string) continue;
        if(!::strcasecmp(token_string,"section")) {
            blend_item_info=new BLEND_ITEM_INFO;
            std::memset(blend_item_info,0,sizeof(BLEND_ITEM_INFO));
        } else if(!::strcasecmp(token_string,"item_vnum")) {
            char* v=std::strtok(0,delim);if(!v) return false;
            blend_item_info->item_vnum=std::atoi(v);
        } else if(!::strcasecmp(token_string,"apply_type")) {
            char* v=std::strtok(0,delim);if(!v) return false;
            blend_item_info->apply_type=FN_get_apply_type(v);
        } else if(!::strcasecmp(token_string,"apply_value")) {
            for(int i=0;i<MAX_BLEND_ITEM_VALUE;++i) {
                char* v=std::strtok(0,delim);if(!v) return false;
                blend_item_info->apply_value[i]=std::atoi(v);
            }
        } else if(!::strcasecmp(token_string,"apply_duration")) {
            for(int i=0;i<MAX_BLEND_ITEM_VALUE;++i) {
                char* v=std::strtok(0,delim);if(!v) return false;
                blend_item_info->apply_duration[i]=std::atoi(v);
            }
        } else if(!::strcasecmp(token_string,"end")) {
            s_blend_info.push_back(blend_item_info);
        }
    }
    std::fclose(fp);
    return true;
}
// 080e63c0
static int FN_random_index() {
    int percent=number_ex(1,100,const_cast<char*>("blend_item.cpp"),235);
    if(percent<=10) return 0;
    if(percent<=30) return 1;
    if(percent<=70) return 2;
    if(percent<=90) return 3;
    return 4;
}
// 080e6430. Two independent random indices; every VNUM uses same distribution.
bool Blend_Item_set_value(CItem* item) {
    for(T_BLEND_ITEM_INFO::iterator it=s_blend_info.begin();it!=s_blend_info.end();++it) {
        BLEND_ITEM_INFO* blend_info=*it;
        if(blend_info->item_vnum==item->GetVnum()) {
            int apply_type=blend_info->apply_type;
            int apply_value=blend_info->apply_value[FN_random_index()];
            int apply_duration=blend_info->apply_duration[FN_random_index()];
            item->SetSocket(0,apply_type);
            item->SetSocket(1,apply_value);
            item->SetSocket(2,apply_duration);
            return true;
        }
    }
    return false;
}
// 080e64e0
bool Blend_Item_find(DWORD item_vnum) {
    for(T_BLEND_ITEM_INFO::iterator it=s_blend_info.begin();it!=s_blend_info.end();++it)
        if((*it)->item_vnum==item_vnum) return true;
    return false;
}
