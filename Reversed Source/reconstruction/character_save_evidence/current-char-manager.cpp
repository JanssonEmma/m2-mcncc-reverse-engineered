// Partial old character manager: real lifecycle entry, state registry and basic lookups.
#include "char_manager.h"
#include "char.h"
#include "desc.h"
#include "utils.h"
#include <cassert>
#include "../../libthecore/include/log.h"
CHARACTER_MANAGER::CHARACTER_MANAGER() : m_iVIDCount(0),m_map_pkPCChr(100),m_pkChrSelectedStone(0) {
    RegisterRaceNum(9004); RegisterRaceNum(20031); RegisterRaceNum(20032);
    m_iMobItemRate=m_iMobDamageRate=m_iMobGoldAmountRate=m_iMobGoldDropRate=m_iMobExpRate=100;
    m_iMobItemRatePremium=m_iMobGoldAmountRatePremium=m_iMobGoldDropRatePremium=m_iMobExpRatePremium=100;
    m_iUserDamageRate=m_iUserDamageRatePremium=100;
}
CHARACTER_MANAGER::~CHARACTER_MANAGER() { Destroy(); }
//0818c1c0 is an evidenced empty application body, not a reconstruction substitute.
void CHARACTER_MANAGER::Destroy() {}
DWORD CHARACTER_MANAGER::AllocVID() {
    DWORD next=static_cast<unsigned int>(m_iVIDCount)+1U;
    m_iVIDCount=static_cast<int>(next); return next;
}
LPCHARACTER CHARACTER_MANAGER::Find(DWORD vid) {
    std::map<DWORD,LPCHARACTER>::iterator it=m_map_pkChrByVID.find(vid);
    return it==m_map_pkChrByVID.end()?0:it->second;
}
LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD pid) {
    std::map<DWORD,LPCHARACTER>::iterator it=m_map_pkChrByPID.find(pid);
    return it==m_map_pkChrByPID.end()?0:it->second;
}
bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER character) {
    assert(character);
    if(m_set_pkChrState.find(character)!=m_set_pkChrState.end()) return false;
    m_set_pkChrState.insert(character); return true;
}
void CHARACTER_MANAGER::RemoveFromStateList(LPCHARACTER character) { m_set_pkChrState.erase(character); }
void CHARACTER_MANAGER::SelectStone(LPCHARACTER character) { m_pkChrSelectedStone=character; }
void CHARACTER_MANAGER::RegisterRaceNum(DWORD race) { m_set_dwRegisteredRaceNum.insert(race); }

LPCHARACTER CHARACTER_MANAGER::Find(const VID& vid) {
    LPCHARACTER character=Find(static_cast<DWORD>(vid));
    return character && character->GetVID()==vid?character:0;
}

namespace {
//Local expansion of old str_lower08146e30 with the manager's24-byte bound.
void NormalizeCharacterName(const char* name,char (&normalized)[25]) {
    unsigned length=0;
    while(length<24 && name[length]) {
        unsigned char c=static_cast<unsigned char>(name[length]);
        normalized[length]=static_cast<char>(c>='A' && c<='Z'?c+32:c);
        ++length;
    }
    normalized[length]='\0';
}
}
//0818c880.
LPCHARACTER CHARACTER_MANAGER::FindPC(const char* name) {
    char normalized[25]; NormalizeCharacterName(name,normalized);
    NAME_MAP::iterator it=m_map_pkPCChr.find(normalized);
    return it==m_map_pkPCChr.end()?0:it->second;
}
//0818e570: log even duplicate registrations, but only for registered races.
void CHARACTER_MANAGER::RegisterRaceNumMap(LPCHARACTER character) {
    DWORD race=character->GetRaceNum();
    if(m_set_dwRegisteredRaceNum.find(race)==m_set_dwRegisteredRaceNum.end()) return;
    sys_log(0,"RegisterRaceNumMap %s %lu",character->GetName(),race);
    m_map_pkChrByRaceNum[race].insert(character);
}
//0818e7d0: use current race and retain empty map buckets.
void CHARACTER_MANAGER::UnregisterRaceNumMap(LPCHARACTER character) {
    DWORD race=character->GetRaceNum();
    std::map<DWORD,CHARACTER_SET>::iterator it=m_map_pkChrByRaceNum.find(race);
    if(it!=m_map_pkChrByRaceNum.end()) it->second.erase(character);
}
//0818c490: remove by keys, without pointer-identity checks or delayed-save cleanup.
void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER character) {
    m_map_pkChrByVID.erase(static_cast<DWORD>(character->GetVID()));
    RemoveFromStateList(character);
    char normalized[25]; NormalizeCharacterName(character->GetName(),normalized);
    NAME_MAP::iterator pc=m_map_pkPCChr.find(normalized);
    if(pc!=m_map_pkPCChr.end()) m_map_pkPCChr.erase(pc);
    if(character->GetPlayerID()) m_map_pkChrByPID.erase(character->GetPlayerID());
    UnregisterRaceNumMap(character);
    delete character;
}

//0818e250,0818e310,0818e340: unique pointer registries; null is not filtered.
void CHARACTER_MANAGER::DelayedSave(LPCHARACTER character) { m_set_pkChrForDelayedSave.insert(character); }
void CHARACTER_MANAGER::RegisterForMonsterLog(LPCHARACTER character) { m_set_pkChrMonsterLog.insert(character); }
void CHARACTER_MANAGER::UnregisterForMonsterLog(LPCHARACTER character) { m_set_pkChrMonsterLog.erase(character); }
//0818e840: querying an absent race creates an empty bucket, regardless of registration.
CHARACTER_SET* CHARACTER_MANAGER::GetCharacterSetByRaceNum(DWORD race) { return &m_map_pkChrByRaceNum[race]; }

//0818e360: live ordered recipients, no map/observer filter; source-null broadcasts.
void CHARACTER_MANAGER::PacketMonsterLog(LPCHARACTER source,const void* data,int bytes) {
    for(CHARACTER_SET::iterator it=m_set_pkChrMonsterLog.begin();it!=m_set_pkChrMonsterLog.end();++it) {
        LPCHARACTER recipient=*it;
        if(source) {
            int dx=static_cast<int>(static_cast<unsigned int>(recipient->GetX())-static_cast<unsigned int>(source->GetX()));
            int dy=static_cast<int>(static_cast<unsigned int>(recipient->GetY())-static_cast<unsigned int>(source->GetY()));
            if(DISTANCE_APPROX(dx,dy)>6000) continue;
        }
        if(recipient->GetDesc()) recipient->GetDesc()->Packet(data,bytes);
    }
}

//0818c280: new character returned even when unique registry insertion collides.
LPCHARACTER CHARACTER_MANAGER::CreateCharacter(const char* name,DWORD playerID) {
    DWORD vid=AllocVID();
    LPCHARACTER character=new CHARACTER;
    character->Create(name,vid,playerID!=0);
    m_map_pkChrByVID.insert(std::make_pair(vid,character));
    if(playerID) {
        char normalized[25]; NormalizeCharacterName(name,normalized);
        m_map_pkPCChr.insert(NAME_MAP::value_type(normalized,character));
        m_map_pkChrByPID.insert(std::make_pair(playerID,character));
    }
    return character;
}
