#include <map>
#include <set>
#include <list>
#include <vector>
#include <string>
#include <ext/hash_map>
#define private public
#define protected public
#include "../../game/src/char.h"
#include "../../game/src/questmanager.h"
#undef protected
#undef private
#include "../../libthecore/include/main.h"
#include <cassert>
#include <cstring>
#include <cstdio>
extern DWORD isolated_dword_clock,isolated_dword_step;
extern unsigned isolated_dword_reads,isolated_float_reads,isolated_pulse_reads;
extern float isolated_float_clock;
extern int test_server,passes_per_sec;
int main() {
    quest::CQuestManager quests;
    quests.m_mapEventFlag["arena_potion_limit_count"]=12;
    quests.m_mapEventFlag["threeway_war_dead_count"]=-7;
    CHARACTER character;
    character.m_vid=VID(123,456); character.m_stName="keep identity";
    character.m_stMobile="mobile"; character.m_stShopSign="sign";
    character.m_strNewName="clear this";
    character.m_bAddChrState=0xff; character.m_bEmpire=91; character.m_ArenaObserver=true;
    character.m_dwWalkStartTime=1234; character.m_iRefineAdditionalCell=13;
    character.m_bPolyMaintainStat=true;
    character.m_posStart.x=11; character.m_posStart.y=12; character.m_posStart.z=13;
    character.m_posDest.x=21; character.m_posDest.y=22; character.m_posDest.z=23;
    character.m_posSafeboxOpen.z=33;
    std::memset(character.m_szMobileAuth,'z',sizeof character.m_szMobileAuth);
    std::memset(&character.m_points,0xa5,sizeof character.m_points);
    std::memset(&character.m_pointsInstant,0xa5,sizeof character.m_pointsInstant);
    std::memset(character.m_quickslot,0xa5,sizeof character.m_quickslot);
    character.m_afAffectFlag.bits[0]=character.m_afAffectFlag.bits[1]=0xffffffffUL;
    character.m_setExceptChainLighting.insert(&character);
    character.m_SkillDamageBonus[4]=9;
    character.m_vec_dwFlyTargets.push_back(77);
    character.m_set_pkChrTargetedBy.insert(&character);
    //Initialize drops event handles before standing-state cancellation; it is not cleanup.
    character.m_pkDeadEvent=reinterpret_cast<LPEVENT>(1);
    character.m_pkStunEvent=reinterpret_cast<LPEVENT>(2);
    test_server=0; passes_per_sec=25; thecore_heart->pulse=700;
    isolated_dword_clock=5000; isolated_dword_step=17;
    isolated_dword_reads=0; isolated_float_clock=100.5f; isolated_float_reads=0; isolated_pulse_reads=0;
    CState* prior_state=character.m_pCurrentState;
    character.Initialize();
    assert(isolated_dword_reads==8 && isolated_float_reads==1 && isolated_pulse_reads==5);
    assert(character.GetType()==0 && !character.GetDesc() && !character.GetSectree());
    assert(character.m_pCurrentState==prior_state && character.m_pNewState==&character.m_stateIdle);
    assert(character.m_dwStateDuration==1 && character.m_pointsInstant.position==7);
    assert(character.m_fSyncTime==97.5f && character.m_fSpeedHackMultipler==1.0f);
    assert(character.m_dwLastMoveTime==5000 && character.m_dwPlayStartTime==5000);
    assert(character.m_dwLastAttackTime==5017UL-20000UL);
    assert(character.m_dwLastDeadTime==5034UL-180000UL);
    assert(character.m_dwLastChangeAttackPositionTime==5051UL-10000UL);
    assert(character.m_dwUnderGuildWarInfoMessageTime==5068UL-60000UL);
    assert(character.GetStopTime()==5085 && character.m_dwLastVictimSetTime==5102UL-3000UL);
    assert(character.m_dwLastSkillTime==5119 && character.m_nPotionLimit==12 && character.m_iKillCount==-7);
    assert(character.m_dwPlayerID==0 && character.m_bCharType==0 && character.m_bAddChrState==0);
    CHARACTER_POINT points; std::memset(&points,0,sizeof points);
    CHARACTER_POINT_INSTANT instant; std::memset(&instant,0,sizeof instant); instant.position=7;
    assert(!std::memcmp(&character.m_points,&points,sizeof points));
    assert(!std::memcmp(&character.m_pointsInstant,&instant,sizeof instant));
    unsigned char zero_slots[sizeof character.m_quickslot]={0};
    assert(!std::memcmp(character.m_quickslot,zero_slots,sizeof zero_slots));
    assert(!character.m_afAffectFlag.bits[0] && !character.m_afAffectFlag.bits[1]);
    assert(character.m_bNoOpenedShop && !character.m_bOpeningSafebox && !character.m_isOpenSafebox);
    assert(!character.m_pkMobData && !character.m_pkMobInst && !character.m_pkParty && !character.m_pGuild);
    assert(!character.m_pkShop && !character.m_pkMyShop && !character.m_pkChrShopOwner && !character.m_pkExchange);
    assert(!character.m_pkDeadEvent && !character.m_pkStunEvent && !character.m_pkSaveEvent && !character.m_pkRecoveryEvent);
    assert(!character.m_pkTimedEvent && !character.m_pkFishingEvent && !character.m_pkAffectEvent);
    assert(!character.m_pkPoisonEvent && !character.m_pkFireEvent && !character.m_pkWarpNPCEvent && !character.m_pkMiningEvent);
    assert(!character.m_pkWarpEvent && !character.m_pkCheckSpeedHackEvent && !character.m_pkMuyeongEvent);
    assert(!character.m_pkPartyRequestEvent && !character.m_pkSafebox && !character.m_pkMall && !character.m_pSkillLevels);
    assert(character.m_iSafeboxSize==-1 && character.m_iSafeboxLoadTime==0 && character.m_iMallLoadTime==0);
    assert(character.m_posRegen.x==0 && character.m_posRegen.y==0 && character.m_posRegen.z==0);
    assert(character.m_posStart.x==0 && character.m_posStart.y==0 && character.m_posStart.z==13);
    assert(character.m_posDest.x==0 && character.m_posDest.y==0 && character.m_posDest.z==23);
    assert(!character.m_posWarp.x && !character.m_posWarp.y && !character.m_posWarp.z);
    assert(!character.m_posExit.x && !character.m_posExit.y && !character.m_posExit.z);
    assert(character.m_posSafeboxOpen.x==-1000 && character.m_posSafeboxOpen.y==-1000 && character.m_posSafeboxOpen.z==33);
    assert(character.m_vid==VID(123,456) && character.m_stName=="keep identity");
    assert(character.m_stMobile=="mobile" && character.m_stShopSign=="sign" && character.m_strNewName.empty());
    assert(character.m_bEmpire==91 && character.m_ArenaObserver && character.m_bPolyMaintainStat);
    assert(character.m_iRefineAdditionalCell==13 && character.m_dwWalkStartTime==1234);
    assert(!character.m_szMobileAuth[0] && character.m_szMobileAuth[1]=='z' && character.m_szMobileAuth[4]=='z');
    assert(character.m_setExceptChainLighting.empty() && character.m_SkillDamageBonus[4]==9);
    assert(character.m_vec_dwFlyTargets.size()==1 && character.m_vec_dwFlyTargets[0]==77);
    assert(character.m_set_pkChrTargetedBy.count(&character));
    assert(character.m_iMaxAggro==-100 && !character.m_pkDungeon && !character.m_pArena && !character.m_pWeddingMap);
    assert(!character.m_pWarMap && !character.m_pkChrMarried && !character.m_chHorse && !character.m_chRider);
    for(unsigned i=0;i<5;++i) assert(character.m_adwMobSkillCooltime[i]==0);
    assert(character.GetMCL(MI_HEAL)==250 && character.GetMC(MI_HEAL)==700UL-6250UL);
    assert(character.m_eOverTime==OT_NONE && !character.m_deposit_pulse && !character.m_iKillingCount);
    //A second reset reads current flags and clocks, while still retaining identity.
    quests.m_mapEventFlag["arena_potion_limit_count"]=-3;
    quests.m_mapEventFlag.erase("threeway_war_dead_count");
    isolated_dword_clock=0xffffffffUL; isolated_dword_step=0;
    character.Initialize();
    assert(character.m_nPotionLimit==-3 && character.m_iKillCount==0 && character.m_dwLastSkillTime==0xffffffffUL);
    assert(character.m_vid==VID(123,456) && character.m_posStart.z==13);
    isolated_dword_step=0;
    std::puts("character initialization passed: reset scope, retained fields, clock order/wrap, FSM queue and real quest flags");
}
