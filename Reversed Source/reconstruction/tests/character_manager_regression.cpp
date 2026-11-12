#include <map>
#include <set>
#include <string>
#include <ext/hash_map>
#define private public
#include "../../game/src/char_manager.h"
#undef private
#include <cassert>
#include <climits>
#include <cstdio>
int main() {
    VID empty; const VID first_identity(0xffffffffUL,0x80000000UL),second_identity(0xffffffffUL,0x80000001UL);
    assert(static_cast<DWORD>(empty)==0 && empty==VID(0,0));
    assert(first_identity!=second_identity && !(first_identity==second_identity) && static_cast<DWORD>(first_identity)==0xffffffffUL);
    VID copied(first_identity),assigned;
    assert(copied==first_identity && &assigned.operator=(copied)==&assigned && assigned==first_identity);
    assigned=assigned; assert(assigned==first_identity);
    copied.Reset(); assert(copied==empty && assigned==first_identity);
    VID chained; chained=assigned=second_identity; assert(chained==second_identity && assigned==second_identity);

    int one=1,two=2;
    LPCHARACTER first=reinterpret_cast<LPCHARACTER>(&one),second=reinterpret_cast<LPCHARACTER>(&two); // opaque values, never dereferenced
    assert(!CHARACTER_MANAGER::instance_ptr());
    {
        CHARACTER_MANAGER manager; assert(CHARACTER_MANAGER::instance_ptr()==&manager);
        assert(manager.m_iVIDCount==0 && !manager.m_pkChrSelectedStone);
        const int rates[]={manager.m_iMobItemRate,manager.m_iMobDamageRate,manager.m_iMobGoldAmountRate,manager.m_iMobGoldDropRate,manager.m_iMobExpRate,
            manager.m_iMobItemRatePremium,manager.m_iMobGoldAmountRatePremium,manager.m_iMobGoldDropRatePremium,manager.m_iMobExpRatePremium,
            manager.m_iUserDamageRate,manager.m_iUserDamageRatePremium};
        for(unsigned i=0;i<sizeof rates/sizeof *rates;++i) assert(rates[i]==100);
        assert(manager.m_set_dwRegisteredRaceNum.size()==3 && manager.m_set_dwRegisteredRaceNum.count(9004) && manager.m_set_dwRegisteredRaceNum.count(20031) && manager.m_set_dwRegisteredRaceNum.count(20032));
        manager.RegisterRaceNum(9004); manager.RegisterRaceNum(999); assert(manager.m_set_dwRegisteredRaceNum.size()==4);
        assert(manager.AllocVID()==1 && manager.AllocVID()==2);
        manager.m_iVIDCount=INT_MAX; assert(manager.AllocVID()==0x80000000UL && manager.m_iVIDCount==INT_MIN);
        manager.m_iVIDCount=-1; assert(manager.AllocVID()==0 && manager.AllocVID()==1);
        assert(!manager.FindPC("missing") && !manager.FindPC(""));
        manager.m_map_pkPCChr["alice"]=first;
        manager.m_map_pkPCChr["abcdefghijklmnopqrstuvwx"]=second;
        manager.m_map_pkPCChr[""]=second;
        assert(manager.FindPC("ALIce")==first && manager.FindPC("alice")==first);
        assert(manager.FindPC("ABCDEFGHIJKLMNOPQRSTUVWXextra")==second);
        assert(manager.FindPC("")==second && !manager.FindPC("alice "));
        const char high_key[]={static_cast<char>(0xc0),'a',0};
        const char high_query[]={static_cast<char>(0xc0),'A',0};
        manager.m_map_pkPCChr[high_key]=first;
        assert(manager.FindPC(high_query)==first);
        assert(!manager.Find(1) && !manager.FindByPID(1));
        manager.m_map_pkChrByVID[0xffffffffUL]=first; manager.m_map_pkChrByPID[0x80000000UL]=second;
        assert(manager.Find(0xffffffffUL)==first && manager.FindByPID(0x80000000UL)==second);
        assert(manager.AddToStateList(first) && !manager.AddToStateList(first) && manager.AddToStateList(second));
        manager.RemoveFromStateList(0); assert(manager.m_set_pkChrState.size()==2);
        manager.RemoveFromStateList(first); manager.RemoveFromStateList(first);
        assert(manager.m_set_pkChrState.size()==1 && manager.m_set_pkChrState.count(second));
        manager.SelectStone(first); assert(manager.m_pkChrSelectedStone==first);
        manager.SelectStone(0); assert(!manager.m_pkChrSelectedStone);
        manager.DelayedSave(first); manager.DelayedSave(first); manager.DelayedSave(0);
        assert(manager.m_set_pkChrForDelayedSave.size()==2 && manager.m_set_pkChrForDelayedSave.count(0));
        manager.RegisterForMonsterLog(first); manager.RegisterForMonsterLog(first); manager.RegisterForMonsterLog(0);
        manager.UnregisterForMonsterLog(second); assert(manager.m_set_pkChrMonsterLog.size()==2);
        manager.UnregisterForMonsterLog(first); manager.UnregisterForMonsterLog(first);
        manager.UnregisterForMonsterLog(0); assert(manager.m_set_pkChrMonsterLog.empty());
        CHARACTER_SET* race_set=manager.GetCharacterSetByRaceNum(0xffffffffUL);
        assert(race_set && race_set->empty() && manager.m_map_pkChrByRaceNum.size()==1);
        race_set->insert(second); assert(manager.GetCharacterSetByRaceNum(0xffffffffUL)==race_set);
        assert(manager.GetCharacterSetByRaceNum(0xffffffffUL)->count(second));
        manager.GetCharacterSetByRaceNum(0); assert(manager.GetCharacterSetByRaceNum(0xffffffffUL)==race_set);
        manager.Destroy(); // Actual target body leaves registries intact.
        assert(manager.Find(0xffffffffUL)==first && manager.m_set_pkChrState.count(second) && manager.m_set_dwRegisteredRaceNum.size()==4);
    }
    assert(!CHARACTER_MANAGER::instance_ptr());
    std::puts("character manager passed: old initial rates/races, VID wrap, registries, unique state membership and empty Destroy");
}
