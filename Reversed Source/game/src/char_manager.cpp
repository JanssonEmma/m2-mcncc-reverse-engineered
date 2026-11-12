#include "party.h"
#include "config.h"
#include "locale_service.h"
#include "db.h"
#include "stdio.h"
#include "cstdio"
#include "mob_manager.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "questmanager.h"
#include "../../libthecore/include/utils.h"
// Partial old character manager: real lifecycle entry, state registry and basic lookups.
#include "char_manager.h"
#include "char.h"
#include "desc.h"
#include "utils.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
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

// 0818c1d0
void CHARACTER_MANAGER::GracefulShutdown()
{
    for (auto it = m_map_pkPCChr.begin(); it != m_map_pkPCChr.end(); ) {
        CHARACTER* character = (it++)->second;
        character->Disconnect();
    }
}
DWORD CHARACTER_MANAGER::AllocVID() {
    DWORD next=static_cast<unsigned int>(m_iVIDCount)+1U;
    m_iVIDCount=static_cast<int>(next); return next;
}
LPCHARACTER CHARACTER_MANAGER::Find(DWORD vid) {
    std::map<DWORD,LPCHARACTER>::iterator it=m_map_pkChrByVID.find(static_cast<std::uint32_t>(vid));
    return it==m_map_pkChrByVID.end()?0:it->second;
}
LPCHARACTER CHARACTER_MANAGER::FindByPID(DWORD pid) {
    std::map<DWORD,LPCHARACTER>::iterator it=m_map_pkChrByPID.find(pid);
    return it==m_map_pkChrByPID.end()?0:it->second;
}
bool CHARACTER_MANAGER::AddToStateList(LPCHARACTER character) {
    if (!character) {
        assert(character);
        std::abort();
    }
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
// 0818e940
LPCHARACTER CHARACTER_MANAGER::FindSpecifyPC(unsigned jobFlags, long map, LPCHARACTER except,
                                            int minLevel, int maxLevel)
{
    LPCHARACTER found = nullptr;
    int count = 0;
    for (const auto& entry : m_map_pkChrByPID) {
        LPCHARACTER candidate = entry.second;
        if (candidate == except || candidate->GetLevel() < minLevel || candidate->GetLevel() > maxLevel
            || candidate->GetMapIndex() != map)
            continue;
        if (jobFlags) {
            const unsigned job = (candidate->GetSkillGroup() + 3 + candidate->GetJob() * 3) & 31;
            if (!(jobFlags & (1u << job)))
                continue;
        }
        if (!found || number_ex(1, ++count, "char_manager.cpp", 800) == 1)
            found = candidate;
    }
    return found;
}

// 0818eac0
int CHARACTER_MANAGER::GetMobDamageRate(LPCHARACTER)
{
    return m_iMobDamageRate;
}

// 0818ecb0
int CHARACTER_MANAGER::GetUserDamageRate(LPCHARACTER character)
{
    return character && character->GetPremiumRemainSeconds(0) > 0
        ? m_iUserDamageRatePremium : m_iUserDamageRate;
}

void CHARACTER_MANAGER::Update(int pulse)
{
    const bool resetChat = passes_per_sec > 0 && pulse % (passes_per_sec * 5) == 0;
    for (auto it = m_map_pkPCChr.begin(); it != m_map_pkPCChr.end(); ) {
        CHARACTER* character = (it++)->second;
        character->Update();
        if (resetChat)
            character->ResetChatCounter();
    }

    for (auto it = m_set_pkChrState.begin(); it != m_set_pkChrState.end(); ) {
        CHARACTER* character = *it++;
        character->UpdateStateMachine(pulse);
    }
    auto& race = *GetCharacterSetByRaceNum(20031);
    for (auto it = race.begin(); it != race.end(); ) {
        CHARACTER* character = *it++;
        character->UpdateStateMachine(pulse);
    }

    if (passes_per_sec > 0 && pulse % (passes_per_sec * 3600) == 0) {
        for (const auto& entry : m_map_dwMobKillCount)
            DBManager::instance().SendMoneyLog(7, entry.first, entry.second);
        m_map_dwMobKillCount.clear();
    }
    if (test_server && passes_per_sec > 0 && pulse % (passes_per_sec * 60) == 0) {
        sys_log(0, "CHARACTER COUNT byvid %d bypid %d", static_cast<int>(m_map_pkChrByVID.size()),
                static_cast<int>(m_map_pkChrByPID.size()));
    }
}

void CHARACTER_MANAGER::DestroyCharacter(LPCHARACTER character) {
    m_map_pkChrByVID.erase(static_cast<DWORD>(character->GetVID()));
    RemoveFromStateList(character);
    char normalized[25]; NormalizeCharacterName(character->GetName(),normalized);
    NAME_MAP::iterator pc=m_map_pkPCChr.find(normalized);
    if(pc!=m_map_pkPCChr.end()) m_map_pkPCChr.erase(pc);
    if(character->GetPlayerID()) m_map_pkChrByPID.erase(character->GetPlayerID());
    UnregisterRaceNumMap(character);
    m_set_pkChrForDelayedSave.erase(character);
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

// 0818d010
LPCHARACTER CHARACTER_MANAGER::SpawnMob(DWORD vnum,long map,long x,long y,long z,
                                      bool spawnMotion,int rotation,bool show) {
    const CMob* mob=CMobManager::instance().Get(vnum);
    if(!mob) { _sys_err("SpawnMob",0x132,"SpawnMob: no mob data for vnum %lu",vnum);return 0; }
    if(mob->m_table.bType!=1 && mob->m_table.bType!=3) {
        SECTREE* tree=SECTREE_MANAGER::instance().Get(static_cast<DWORD>(map),
                                                   static_cast<DWORD>(x),static_cast<DWORD>(y));
        if(!tree) {
            sys_log(0,"no sectree for spawn at %d %d mobvnum %d mapindex %d",x,y,vnum,map);
            return 0;
        }
        DWORD attributes=tree->GetAttribute(x,y);
        if(attributes & (ATTR_BLOCK | ATTR_OBJECT)) {
            static bool log=quest::CQuestManager::instance().GetEventFlag("spawn_block_log")!=0;
            static std::uint32_t next = static_cast<std::uint32_t>(get_global_time()) + 10000u;
            const auto now = static_cast<std::uint32_t>(get_global_time());
            if(next<now) { next=now;log=quest::CQuestManager::instance().GetEventFlag("spawn_block_log")!=0; }
            if(log) sys_log(0,"SpawnMob: BLOCKED position for spawn %s %lu at %d %d (attr %lu)",
                           mob->m_table.szName,vnum,x,y,attributes);
            return 0;
        }
        if(attributes & ATTR_BANPK) {
            sys_log(0,"SpawnMob: BAN_PK position for mob spawn %s %lu at %d %d",
                    mob->m_table.szName,vnum,x,y);
            return 0;
        }
    }
    CHARACTER* character=CHARACTER_MANAGER::instance().CreateCharacter(mob->m_table.szLocaleName,0);
    if(!character) { sys_log(0,"SpawnMob: cannot create new character");return 0; }
    if(rotation==-1) rotation=number_ex(0,360,"char_manager.cpp",0x167);
    character->SetProto(mob);
    if(mob->m_table.bType==1 && character->GetEmpire()==0)
        character->SetEmpire(SECTREE_MANAGER::instance().GetEmpireFromMapIndex(map));
    character->SetRotation(static_cast<float>(rotation));
    if(!show) return character;
    if(character->Show(map,x,y,z,spawnMotion)) return character;
    CHARACTER_MANAGER::instance().DestroyCharacter(character);
    sys_log(0,"SpawnMob: cannot show monster");
    return 0;
}

void SendNotice(const char* text);

LPCHARACTER CHARACTER_MANAGER::SpawnMobRandomPosition(DWORD vnum, long mapIndex)
{
    if (vnum==5001 && !quest::CQuestManager::instance().GetEventFlag("japan_regen")) {
        sys_log(0,"WAEGU[5001] regen disabled.");
        return 0;
    }
    if (vnum==5002 && !quest::CQuestManager::instance().GetEventFlag("newyear_mob")) {
        sys_log(0,"HAETAE (new-year-mob) [5002] regen disabled.");
        return 0;
    }
    if (vnum==5004 && !quest::CQuestManager::instance().GetEventFlag("independence_day")) {
        sys_log(0,"INDEPENDECE DAY [5004] regen disabled.");
        return 0;
    }
    const CMob* mob=CMobManager::instance().Get(vnum);
    if (!mob) {
        _sys_err("SpawnMobRandomPosition",0xe2,"no mob data for vnum %lu",vnum);
        return 0;
    }
    if (!map_allow_find(mapIndex)) {
        _sys_err("SpawnMobRandomPosition",0xe8,"not allowed map %lu",mapIndex);
        return 0;
    }
    SECTREE_MANAGER& sectors=SECTREE_MANAGER::instance();
    SECTREE_MAP* map=sectors.GetMap(mapIndex);
    const auto randomCoordinate = [](int extent, int base, int line) {
        const auto limit = static_cast<std::int32_t>(static_cast<std::uint32_t>(extent) - 100u);
        const int offset = number_ex(100, limit, "char_manager.cpp", line);
        return static_cast<std::int32_t>(static_cast<std::uint32_t>(base) + offset);
    };
    long x,y;
    int attempt=0;
    for (;attempt<2000;++attempt) {
        x = randomCoordinate(map->m_setting.iWidth, map->m_setting.iBaseX, 0xf1);
        y = randomCoordinate(map->m_setting.iHeight, map->m_setting.iBaseY, 0xf2);
        SECTREE* tree=map->Find(x,y);
        if (tree && !(tree->GetAttribute(x,y) & (ATTR_BLOCK | ATTR_BANPK | ATTR_OBJECT))) break;
    }
    if (attempt==2000) {
        _sys_err("SpawnMobRandomPosition",0x106,"cannot find valid location");
        return 0;
    }
    CHARACTER* ch=CHARACTER_MANAGER::instance().CreateCharacter(mob->m_table.szLocaleName,0);
    if (!ch) {
        _sys_err("SpawnMobRandomPosition",0x10e,"cannot create new character");
        return 0;
    }
    ch->SetProto(mob);
    if (mob->m_table.bType==1 && !ch->GetEmpire()) ch->SetEmpire(sectors.GetEmpireFromMapIndex(mapIndex));
    ch->SetRotation(number_ex(0,360,"char_manager.cpp",0x119));
    if (!ch->Show(mapIndex,x,y,0,false)) {
        CHARACTER_MANAGER::instance().DestroyCharacter(ch);
        _sys_err("SpawnMobRandomPosition",0x11e,"SpawnMobRandomPosition: cannot show monster");
        return 0;
    }
    char notice[1024];
    time_t time=get_global_time();
    std::snprintf(notice,sizeof(notice),"spawn %s[%ld] random position at %ld %ld (time: %d)",
                  ch->GetName(),vnum,x,y,static_cast<int>(time));
    if (test_server) SendNotice(notice);
    sys_log(0,notice);
    return ch;
}

namespace {
void SendMapScript(int mapIndex, const std::string& script, int empire)
{
    if (script.size() > 0xffffU - 6U)
        return;
    const packet_script header = {45, static_cast<WORD>(script.size() + 6), 1,
                                  static_cast<WORD>(script.size())};
    TEMP_BUFFER buffer;
    buffer.write(&header, 6);
    buffer.write(script.data(), static_cast<int>(script.size()));

    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!map)
        return;
    for (const auto& entry : *map) {
        entry.second->ForEachEntity([&](LPENTITY entity) {
            if (!entity->IsType(0) || !entity->GetDesc())
                return;
            if (empire >= 0 && static_cast<LPCHARACTER>(entity)->GetEmpire() != empire)
                return;
            entity->GetDesc()->Packet(buffer.read_peek(), buffer.size());
        });
    }
}
}

// 0818ee90
void CHARACTER_MANAGER::NoticeToMyEmpireInMap(int mapIndex, BYTE empire, const char* text)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!map)
        return;

    const std::string message(text);
    for (const auto& entry : *map) {
        entry.second->ForEachEntity([&](LPENTITY entity) {
            if (!entity->IsType(0) || !entity->GetDesc())
                return;
            auto* character = static_cast<CHARACTER*>(entity);
            if (character->GetEmpire() == empire)
                character->ChatPacket(2, "%s", message.c_str());
        });
    }
}

// 0818f050
void CHARACTER_MANAGER::SayInMap(int mapIndex, const std::string& script)
{
    SendMapScript(mapIndex, script, -1);
}

// 0818f1d0
void CHARACTER_MANAGER::SayInMapinMyEmpire(int mapIndex, BYTE empire, const std::string& script)
{
    SendMapScript(mapIndex, script, empire);
}

void CHARACTER_MANAGER::ProcessDelayedSave()
{
    CHARACTER_SET::iterator it = m_set_pkChrForDelayedSave.begin();
    while (it != m_set_pkChrForDelayedSave.end()) {
        LPCHARACTER character = *it++;
        character->SaveReal();
    }
    m_set_pkChrForDelayedSave.clear();
}

bool CHARACTER_MANAGER::FlushDelayedSave(LPCHARACTER character)
{
    CHARACTER_SET::iterator it=m_set_pkChrForDelayedSave.find(character);
    if (it==m_set_pkChrForDelayedSave.end()) return false;
    m_set_pkChrForDelayedSave.erase(it);
    character->SaveReal();
    return true;
}

LPCHARACTER CHARACTER_MANAGER::SpawnMobRange(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                                             bool, bool spawnMotion, bool aggressive)
{
    const CMob* mob=CMobManager::instance().Get(vnum);
    if (!mob) return 0;
    if (mob->m_table.bType==2) spawnMotion=true;
    for (int attempt=0;attempt<16;++attempt) {
        int x=number_ex(sx,ex,"char_manager.cpp",0x18a);
        int y=number_ex(sy,ey,"char_manager.cpp",0x18b);
        CHARACTER* ch=SpawnMob(vnum,map,x,y,0,spawnMotion,-1,true);
        if (!ch) continue;
        DWORD vid=ch->GetVID();
        long placedY=ch->GetY();
        long placedX=ch->GetX();
        sys_log(1,"MOB_SPAWN: %s(%d) %dx%d",ch->GetName(),vid,placedX,placedY);
        if (aggressive) ch->SetAggressive();
        return ch;
    }
    return 0;
}

LPCHARACTER CHARACTER_MANAGER::SpawnGroup(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                                          regen* record, bool aggressive, CDungeon* dungeon)
{
    CMobGroup* group=CMobManager::instance().GetGroup(vnum);
    if (!group) {
        _sys_err("SpawnGroup",0x1f1,"NOT_EXIST_GROUP_VNUM(%u)",vnum);
        return 0;
    }
    const bool fromStone=m_pkChrSelectedStone!=0;
    if (fromStone && m_pkChrSelectedStone->GetDungeon()) aggressive=true;
    const std::vector<DWORD>& members=group->GetMemberVector();
    CHARACTER* leader=0;
    CHARACTER* master=0;
    CParty* party=0;
    for (size_t i=0;i<members.size();++i) {
        CHARACTER* ch=SpawnMobRange(members[i],map,sx,sy,ex,ey,true,fromStone,false);
        if (!ch) {
            if (i==0) return 0;
            continue;
        }
        if (i==0) leader=ch;
        ch->SetDungeon(dungeon);
        long x=ch->GetX();
        sx=x-number_ex(300,500,"char_manager.cpp",0x218);
        long y=ch->GetY();
        sy=y-number_ex(300,500,"char_manager.cpp",0x219);
        x=ch->GetX();
        ex=x+number_ex(300,500,"char_manager.cpp",0x21a);
        y=ch->GetY();
        ey=y+number_ex(300,500,"char_manager.cpp",0x21b);
        if (m_pkChrSelectedStone) ch->SetStone(m_pkChrSelectedStone);
        else if (party) {
            party->Join(ch->GetVID());
            party->Link(ch);
        } else if (!master) {
            ch->SetRegen(record);
            party=CPartyManager::instance().CreateParty(ch);
            master=ch;
        }
        if (aggressive) ch->SetAggressive();
    }
    return leader;
}

bool CHARACTER_MANAGER::SpawnMoveGroup(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                                         int tx, int ty, regen* record, bool aggressive)
{
    CMobGroup* group = CMobManager::instance().GetGroup(vnum);
    if (!group) {
        _sys_err("SpawnMoveGroup", 0x1aa, "NOT_EXIST_GROUP_VNUM(%u) Map(%u) ",
                 static_cast<unsigned>(vnum), static_cast<unsigned>(map));
        return false;
    }

    const bool fromStone = m_pkChrSelectedStone != nullptr;
    if (fromStone && m_pkChrSelectedStone->GetDungeon())
        aggressive = true;
    CParty* party = nullptr;
    CHARACTER* master = nullptr;
    const auto& members = group->GetMemberVector();
    for (std::size_t index = 0; index < members.size(); ++index) {
        CHARACTER* character = SpawnMobRange(members[index], map, sx, sy, ex, ey, true, fromStone, false);
        if (!character) {
            if (!index)
                return false;
            continue;
        }

        sx = character->GetX() - number_ex(300, 500, const_cast<char*>("char_manager.cpp"), 0x1c9);
        sy = character->GetY() - number_ex(300, 500, const_cast<char*>("char_manager.cpp"), 0x1ca);
        ex = character->GetX() + number_ex(300, 500, const_cast<char*>("char_manager.cpp"), 0x1cb);
        ey = character->GetY() + number_ex(300, 500, const_cast<char*>("char_manager.cpp"), 0x1cc);
        if (m_pkChrSelectedStone) {
            character->SetStone(m_pkChrSelectedStone);
        } else if (party) {
            party->Join(character->GetVID());
            party->Link(character);
        } else if (!master) {
            character->SetRegen(record);
            party = CPartyManager::instance().CreateParty(character);
            master = character;
        }
        if (aggressive)
            character->SetAggressive();
        if (character->Goto(tx, ty))
            character->SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    }
    return true;
}

bool CHARACTER_MANAGER::SpawnGroupGroup(DWORD vnum, long map, int sx, int sy, int ex, int ey,
                                         regen* record, bool aggressive, CDungeon* dungeon)
{
    DWORD group=CMobManager::instance().GetGroupFromGroupGroup(vnum);
    return SpawnGroup(group,map,sx,sy,ex,ey,record,aggressive,dungeon)!=0;
}

int CHARACTER_MANAGER::GetMobItemRate(LPCHARACTER character)
{
    const e_overtime overtime = character && LC_IsNewCIBN() ? character->GetOverTime() : OT_NONE;
    if (overtime == OT_5HOUR)
        return 0;
    const int rate = character && character->GetPremiumRemainSeconds(1) > 0
        ? m_iMobItemRatePremium : m_iMobItemRate;
    return overtime == OT_3HOUR ? rate / 2 : rate;
}

namespace {
int MobGoldRate(CHARACTER* character, int base, int premium)
{
    const e_overtime overtime = character && LC_IsNewCIBN() ? character->GetOverTime() : OT_NONE;
    if (overtime == OT_5HOUR)
        return 0;
    const int rate = character && character->GetPremiumRemainSeconds(6) > 0 ? premium : base;
    return overtime == OT_3HOUR ? rate / 2 : rate;
}
}

int CHARACTER_MANAGER::GetMobGoldAmountRate(LPCHARACTER character)
{
    return MobGoldRate(character, m_iMobGoldAmountRate, m_iMobGoldAmountRatePremium);
}

int CHARACTER_MANAGER::GetMobGoldDropRate(LPCHARACTER character)
{
    return MobGoldRate(character, m_iMobGoldDropRate, m_iMobGoldDropRatePremium);
}

void CHARACTER_MANAGER::KillLog(DWORD vnum)
{
    DWORD& count = m_map_dwMobKillCount[vnum];
    if (++count > 10000) {
        DBManager::instance().SendMoneyLog(7, vnum, count);
        m_map_dwMobKillCount.erase(vnum);
    }
}

int CHARACTER_MANAGER::GetMobExpRate(LPCHARACTER character)
{
    const e_overtime overtime = character && LC_IsNewCIBN() ? character->GetOverTime() : OT_NONE;
    if (overtime == OT_5HOUR)
        return 0;
    const int rate = character && character->GetPremiumRemainSeconds(0) > 0
        ? m_iMobExpRatePremium : m_iMobExpRate;
    return overtime == OT_3HOUR ? rate / 2 : rate;
}
