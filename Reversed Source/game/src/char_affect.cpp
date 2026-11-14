#include "battle.h"
#include "arena.h"
#include "char_manager.h"
#include <cstdint>
#include <limits>
#include "locale_service.h"
#include "utils.h"
#include "../../libthecore/include/utils.h"
#include "config.h"
#include "locale.hpp"
#include "../../libthecore/include/log.h"
#include "guild.h"
// Old affect lookup; mutation/stat application remains under recovery.
#include "char.h"
#include "affect.h"
CAffect* CHARACTER::FindAffect(DWORD type,BYTE apply) {
    const auto affectType = static_cast<std::uint32_t>(type);
    for (auto it = m_list_pkAffect.begin(); it != m_list_pkAffect.end();) {
        CAffect* affect = *it++;
        if (static_cast<std::uint32_t>(affect->dwType) == affectType && (!apply || affect->bApplyOn == apply))
            return affect;
    }
    return nullptr;
}

#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include "../../common/tables.h"
void SendAffectRemovePacket(LPDESC d,DWORD pid,DWORD type,BYTE point)
{
    TPacketGCAffectRemove ptoc;
    ptoc.bApplyOn=point;ptoc.dwType=type;ptoc.bHeader=127;
    d->Packet(&ptoc,6);
    TPacketGDRemoveAffect ptod;
    ptod.dwType=type;ptod.dwPID=pid;ptod.bApplyOn=point;
    db_clientdesc->DBPacket(33,0,&ptod,9);
}
bool CHARACTER::IsAffectFlag(DWORD flag)
{
    return m_afAffectFlag.IsSet(static_cast<std::int32_t>(flag));
}

#include <cstring>
namespace mount_polymorph_detail {
// Explicit target32 arithmetic, with C++03 spelling and no signed overflow.
inline long Signed(unsigned int bits) {
    int value; std::memcpy(&value,&bits,sizeof(value)); return value;
}
inline long Add(long a,long b) { return Signed(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline long Sub(long a,long b) { return Signed(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline long Mul(long a,long b) { return Signed(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
}

// Full saved auxiliary body, STILL SCC-dependent; no app substitute.
void CHARACTER::ComputeAffect(CAffect* affect,bool add) {
    using namespace mount_polymorph_detail;
    if(add && affect->dwType>=151 && affect->dwType<=162 &&
       (!m_pGuild || !m_pGuild->UnderAnyWar(3))) return;
    const auto flag = static_cast<std::int32_t>(affect->dwFlag);
    if (add)
        m_afAffectFlag.Set(flag);
    else
        m_afAffectFlag.Reset(flag);
    int amount=static_cast<int>(add?Signed(static_cast<unsigned int>(affect->lApplyValue)):
                                     Sub(0,affect->lApplyValue));
    PointChange(affect->bApplyOn,amount,false,false);
    if(affect->dwType==78) {
        if(add) StartMuyeongEvent();else StopMuyeongEvent();
    }
}

// Full saved auxiliary body, STILL SCC-dependent; no app substitute.
void CHARACTER::RefreshAffect() {
    const DWORD oldHigh = m_afAffectFlag.bits[1];
    std::list<CAffect*>::iterator it=m_list_pkAffect.begin();
    const TAffectFlag oldFlags(m_afAffectFlag.bits[0], oldHigh);
    long move=GetPoint(19);
    long attack=GetPoint(17);
    while(it!=m_list_pkAffect.end()) {
        CAffect* affect=*it++;
        ComputeAffect(affect,true);
    }
    if(oldFlags != m_afAffectFlag ||
       move!=GetPoint(19) || attack!=GetPoint(17)) UpdatePacket();
}

bool CHARACTER::RemoveAffect(CAffect* affect)
{
    if (!affect) return false;
    m_list_pkAffect.remove(affect);
    ComputeAffect(affect,false);
    if (affect->dwFlag || affect->bApplyOn==19 || affect->bApplyOn==17) UpdatePacket();
    ComputePoints();
    CheckMaximumPoints();
    if (test_server) sys_log(0,"Flag %d Apply %d ",affect->dwFlag,static_cast<unsigned int>(affect->bApplyOn));
    LPDESC d=GetDesc();
    if (d) SendAffectRemovePacket(d,m_dwPlayerID,affect->dwType,affect->bApplyOn);
    delete affect;
    return true;
}
bool CHARACTER::RemoveAffect(DWORD type)
{
    type = static_cast<std::uint32_t>(type);
    if (type == AFFECT_SKILL_BOOK_BONUS) sys_log(0,"XXX RemoveAffect SKILL_BOOK_BONUS from %s",GetName());
    if (type == AFFECT_BLOCK_CHAT) ChatPacket(1,locale_find("\xc3\xa4\xc6\xc3\x20\xb1\xdd\xc1\xf6\xb0\xa1\x20\xc7\xae\xb7\xc8\xbd\xc0\xb4\xcf\xb4\xd9\x2e",g_iUseLocale));
    bool flag=false;
    for (;;) {
        CAffect* affect=FindAffect(type,0);
        if (!affect) break;
        RemoveAffect(affect);
        flag=true;
    }
    return flag;
}

namespace {
struct AffectEventInfo { LPCHARACTER character; };
}

long affect_event(LPEVENT event)
{
    LPCHARACTER ch=static_cast<AffectEventInfo*>(event->info)->character;
    return ch->UpdateAffect() ? passes_per_sec : 0;
}

void CHARACTER::StartAffectEvent()
{
    if (!m_pkAffectEvent) {
        void* rawInfo = nullptr;
        const DWORD id = __AllocEventInfo(sizeof(AffectEventInfo), &rawInfo);
        static_cast<AffectEventInfo*>(rawInfo)->character = this;
        m_pkAffectEvent=event_create_ex(affect_event,id,passes_per_sec,
                                        const_cast<char*>("char_affect.cpp"),0x9d);
        sys_log(1,"StartAffectEvent %s %p %p",GetName(),this,m_pkAffectEvent);
    }
}

bool CHARACTER::UpdateAffect()
{
    using namespace mount_polymorph_detail;
    if (GetPoint(35)>0) {
        int value;
        if (m_points.hp<m_pointsInstant.iMaxHP) {
            int rate;
            if (LC_IsYMIR()) rate=Mul(m_pointsInstant.iMaxHP,9)/100;
            else {
                bool japan=LC_IsJapan();
                rate=Mul(m_pointsInstant.iMaxHP,7)/100;
                if (japan) rate=MIN(350,rate);
            }
            value=MIN(GetPoint(35),rate);
            PointChange(5,value,false,false);
        } else value=GetPoint(35);
        PointChange(35,Sub(0,value),false,false);
    }
    if (GetPoint(36)>0) {
        const int maximum=m_pointsInstant.iMaxSP;
        int value;
        if (m_points.sp<maximum) {
            const int rate=Mul(maximum,7)/100;
            value=MIN(GetPoint(36),rate);
            PointChange(7,value,false,false);
        } else value=GetPoint(36);
        PointChange(36,Sub(0,value),false,false);
    }
    if (GetPoint(105)>0) PointChange(5,GetPoint(105),false,false);
    if (GetPoint(106)>0) PointChange(7,GetPoint(106),false,false);
    if (GetStamina()<GetMaxStamina()) {
        DWORD now=get_dword_time();
        if (static_cast<unsigned int>(now-GetStopTime())/3000)
            PointChange(9,GetMaxStamina(),false,false);
    }
    if (ProcessAffect() && !GetPoint(35) && !GetPoint(36) &&
        GetStamina()==GetMaxStamina()) {
        m_pkAffectEvent=0;
        return false;
    }
    return true;
}

int CHARACTER::ProcessAffect()
{
    using namespace mount_polymorph_detail;
    bool changed=false;
    for (BYTE i=0;i<10;++i) {
        CAffect* affect=FindAffect(AFFECT_PREMIUM_START + i, 0);
        if (!affect) continue;
        int remaining=GetPremiumRemainSeconds(i);
        if (remaining<0) {
            RemoveAffect(AFFECT_PREMIUM_START + i);
            changed=true;
        } else affect->lDuration=Add(remaining,1);
    }
    CAffect* hair=FindAffect(AFFECT_HAIR, 0);
    if (hair) {
        int limit=GetQuestFlag("hair.limit_time");
        if (limit < static_cast<std::int32_t>(get_global_time())) {
            SetPart(3,0);
            RemoveAffect(AFFECT_HAIR);
        } else hair->lDuration=Add(hair->lDuration,1);
    }
    const DWORD high = m_afAffectFlag.bits[1];
    const TAffectFlag oldFlags(m_afAffectFlag.bits[0], high);
    long move=GetPoint(19), attack=GetPoint(17);
    std::list<CAffect*>::iterator it=m_list_pkAffect.begin();
    while (it!=m_list_pkAffect.end()) {
        CAffect* affect=*it;
        bool end=affect->dwType>=151 && affect->dwType<=162 && (!m_pGuild || !m_pGuild->UnderAnyWar(3));
        const auto spCost = static_cast<std::int32_t>(affect->lSPCost);
        if (spCost > 0) {
            if (m_points.sp < spCost)
                end = true;
            else
                PointChange(POINT_SP, Sub(0, affect->lSPCost), false, false);
        }
        affect->lDuration=Sub(affect->lDuration,1);
        if (end || affect->lDuration<1) {
            it=m_list_pkAffect.erase(it);
            ComputeAffect(affect,false);
            changed=true;
            LPDESC desc=GetDesc();
            if (desc) SendAffectRemovePacket(desc,m_dwPlayerID,affect->dwType,affect->bApplyOn);
            delete affect;
        } else ++it;
    }
    if (changed) {
        if (oldFlags != m_afAffectFlag ||
            move!=GetPoint(19) || attack!=GetPoint(17)) UpdatePacket();
        CheckMaximumPoints();
    }
    return m_list_pkAffect.empty();
}

// 0813d350
namespace {
TPacketAffectElement MakeAffectElement(const CAffect& affect)
{
    TPacketAffectElement element;
    element.dwType = affect.dwType;
    element.bApplyOn = affect.bApplyOn;
    element.lApplyValue = affect.lApplyValue;
    element.dwFlag = affect.dwFlag;
    element.lDuration = affect.lDuration;
    element.lSPCost = affect.lSPCost;
    return element;
}
}

void SendAffectAddPacket(LPDESC desc, CAffect* affect)
{
    TPacketGCAffectAdd packet;
    packet.bHeader = 126;
    packet.elem = MakeAffectElement(*affect);
    desc->Packet(&packet, sizeof(packet));
}

// 0813e380
bool CHARACTER::AddAffect(DWORD type, BYTE point, long value, DWORD flag, long duration,
                          long spCost, bool overrideExisting, bool isCube)
{
    type = static_cast<std::uint32_t>(type);
    value = static_cast<std::int32_t>(value);
    duration = static_cast<std::int32_t>(duration);
    flag = static_cast<std::uint32_t>(flag);
    spCost = static_cast<std::int32_t>(spCost);
    if (type == AFFECT_BLOCK_CHAT && duration > 1)
        ChatPacket(1, locale_find("\277\356\277\265\300\332\040\301\246\301\246\267\316\040\303\244\306\303\300\314\040\261\335\301\366\040\265\307\276\372\275\300\264\317\264\331\056", g_iUseLocale));
    if (duration == 0) {
        duration = 1;
        // The old call supplies a leading zero before the type.
        _sys_err("AddAffect", 543, "Character::AddAffect lDuration == 0 type %d", 0, type);
    }
    if (type == AFFECT_SKILL_BOOK_BONUS)
        sys_log(0, "XXX AddAffect SKILL_BOOK_BONUS from %s", GetName());
    if (test_server)
        sys_log(0, "AddAffect %s type %d apply %d %d flag %u duration %d",
                GetName(), static_cast<int>(type), static_cast<int>(point), static_cast<int>(value), static_cast<unsigned int>(flag), static_cast<int>(duration));
    sys_log(1, "AddAffect %s type %d apply %d %d flag %u duration %d",
            GetName(), static_cast<int>(type), static_cast<int>(point), static_cast<int>(value), static_cast<unsigned int>(flag), static_cast<int>(duration));

    CAffect* affect = FindAffect(type, isCube ? point : 0);
    if (flag == AFF_STUN) {
        const long x = GetX();
        const long y = GetY();
        if (m_posDest.x != x || m_posDest.y != y) {
            m_posStart.x = m_posDest.x = x;
            m_posStart.y = m_posDest.y = y;
            battle_end(this);
            SyncPacket();
        }
    }
    if (affect && overrideExisting) {
        ComputeAffect(affect, false);
        LPDESC desc = GetDesc();
        if (desc)
            SendAffectRemovePacket(desc, m_dwPlayerID, affect->dwType, affect->bApplyOn);
    } else {
        affect = new CAffect;
        m_list_pkAffect.push_back(affect);
    }

    affect->dwType = type;
    affect->bApplyOn = point;
    affect->lApplyValue = value;
    affect->lDuration = duration;
    affect->dwFlag = flag;
    affect->lSPCost = spCost;
    const WORD oldMove = GetPoint(19);
    const WORD oldAttack = GetPoint(17);
    ComputeAffect(affect, true);
    if (affect->dwFlag || oldMove != GetPoint(19) || oldAttack != GetPoint(17))
        UpdatePacket();
    StartAffectEvent();

    LPDESC desc = GetDesc();
    if (desc) {
        SendAffectAddPacket(desc, affect);
        const DWORD currentType = affect->dwType;
        if (currentType != AFFECT_WAR_FLAG && currentType != AFFECT_REVIVE_INVISIBLE && currentType - AFFECT_PREMIUM_START > 9) {
            TPacketGDAddAffect packet;
            packet.dwPID = m_dwPlayerID;
            packet.elem = MakeAffectElement(*affect);
            db_clientdesc->DBPacket(32, 0, &packet, sizeof(packet));
        }
    }
    return true;
}

// 0813ed10
bool CHARACTER::IsGoodAffect(BYTE type)
{
    switch (type) {
    case 3: case 4: case 19: case 34: case 49:
    case 63: case 64: case 65: case 79:
    case 94: case 95: case 96: case 110: case 111:
    case 200: case 201: case 204: case 205: case 206: case 207:
    case 224:
        return true;
    default:
        return false;
    }
}

// 0813eda0
void CHARACTER::RemoveBadAffect()
{
    sys_log(0, "RemoveBadAffect %s", GetName());
    RemovePoison();
    RemoveFire();
    RemoveAffect(210);
    RemoveAffect(211);
    RemoveAffect(80);
}

// 0813eb90
void CHARACTER::RemoveGoodAffect()
{
    static const DWORD types[] = {
        200, 201, 204, 205, 207, 206, 224, 3, 4, 19, 34, 49, 63, 64, 65, 79,
        94, 95, 110, 111, 96
    };
    for (unsigned int i = 0; i < sizeof(types)/sizeof(types[0]); ++i)
        RemoveAffect(types[i]);
}

void CHARACTER::SaveAffect()
{
    std::list<CAffect*>::iterator it=m_list_pkAffect.begin();
    while (it!=m_list_pkAffect.end()) {
        CAffect* affect=*it++;
        DWORD type=affect->dwType;
        if (type == AFFECT_WAR_FLAG || type == AFFECT_REVIVE_INVISIBLE || (type >= AFFECT_PREMIUM_START && type <= AFFECT_PREMIUM_END)) continue;
        sys_log(0,"AFFECT_SAVE: %lu %u %ld %ld",affect->dwType,static_cast<unsigned int>(affect->bApplyOn),
                affect->lApplyValue,affect->lDuration);
        TPacketGDAddAffect packet;
        packet.dwPID=m_dwPlayerID;
        packet.elem.dwType=affect->dwType;
        packet.elem.bApplyOn=affect->bApplyOn;
        packet.elem.lApplyValue=affect->lApplyValue;
        packet.elem.dwFlag=affect->dwFlag;
        packet.elem.lDuration=affect->lDuration;
        packet.elem.lSPCost=affect->lSPCost;
        db_clientdesc->DBPacket(32,0,&packet,25);
    }
}

void CHARACTER::ClearAffect(bool save)
{
    const TAffectFlag oldFlags = m_afAffectFlag;
    const WORD oldMove = static_cast<WORD>(GetPoint(POINT_MOV_SPEED));
    const WORD oldAttack = static_cast<WORD>(GetPoint(POINT_ATT_SPEED));
    for (auto it = m_list_pkAffect.begin(); it != m_list_pkAffect.end();) {
        auto current = it++;
        CAffect* affect = *current;
        const DWORD type = affect->dwType;
        if (save && (type == AFFECT_REVIVE_INVISIBLE || type == AFFECT_WAR_FLAG || type == AFFECT_BLOCK_CHAT ||
                     (type >= AFFECT_PREMIUM_START && type < 600)))
            continue;
        if (auto* desc = save ? GetDesc() : nullptr) {
            const BYTE point = affect->bApplyOn;
            const DWORD pid = GetPlayerID();
            const DWORD currentType = affect->dwType;
            SendAffectRemovePacket(desc, pid, currentType, point);
        }
        ComputeAffect(affect, false);
        m_list_pkAffect.erase(current);
        delete affect;
    }
    if (oldFlags != m_afAffectFlag ||
        oldMove != GetPoint(POINT_MOV_SPEED) || oldAttack != GetPoint(POINT_ATT_SPEED))
        UpdatePacket();
    CheckMaximumPoints();
    if (m_list_pkAffect.empty())
        event_cancel(&m_pkAffectEvent);
}

namespace {
struct load_affect_login_event_info {
    DWORD pid;
    DWORD handle;
    DWORD count;
};
}

// 0813dea0
long load_affect_login_event(LPEVENT event)
{
    auto* info = static_cast<load_affect_login_event_info*>(event->info);
    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(info->pid);
    DESC* desc = character ? character->GetDesc() : nullptr;
    if (!desc || desc->GetHandle() != info->handle || desc->IsPhase(0))
        return 0;
    if (desc->IsPhase(1) || desc->IsPhase(2) || desc->IsPhase(3) ||
        desc->IsPhase(4) || desc->IsPhase(6))
        return passes_per_sec;

    if (desc->IsPhase(5)) {
        sys_log(1, "Affect Load by Event");
        character->LoadAffect(info->count, reinterpret_cast<TPacketAffectElement*>(info + 1));
    } else {
        _sys_err("load_affect_login_event", 0x192, "input_db.cpp:quest_login_event INVALID PHASE pid %d",
                 static_cast<int>(info->pid));
    }
    return 0;
}

// 0813dfd0
void CHARACTER::LoadAffect(DWORD count, TPacketAffectElement* elements)
{
    DESC* desc = GetDesc();
    if (!desc)
        return;
    if (!desc->IsPhase(5)) {
        sys_log(test_server ? 0 : 1,
                test_server ? "Load Affect :Create event for load affect" : "Create event for load affect");
        constexpr auto elementSize = sizeof(TPacketAffectElement);
        if (count > (std::numeric_limits<int>::max() - sizeof(load_affect_login_event_info)) / elementSize)
            return;
        void* raw;
        const DWORD id = __AllocEventInfo(sizeof(load_affect_login_event_info) + count * elementSize, &raw);
        auto* info = static_cast<load_affect_login_event_info*>(raw);
        info->pid = GetPlayerID();
        info->handle = desc->GetHandle();
        info->count = count;
        if (count)
            std::memcpy(info + 1, elements, count * elementSize);
        event_create_ex(load_affect_login_event, id, passes_per_sec, const_cast<char*>("char_affect.cpp"), 0x1a8);
        return;
    }

    sys_log(test_server ? 0 : 1, test_server ? "Load Affect : Affect: %s count %d" : "Affect: %s count %d",
            GetName(), static_cast<int>(count));
    const TAffectFlag oldFlags = m_afAffectFlag;
    const long moveSpeed = GetPoint(POINT_MOV_SPEED);
    const long attackSpeed = GetPoint(POINT_ATT_SPEED);
    for (DWORD i = 0; i < count; ++i) {
        const auto& element = elements[i];
        if (element.dwType == 78)
            continue;
        if (element.bApplyOn == 255) {
            _sys_err("LoadAffect", 0x1bf, "invalid affect data %s ApplyOn %u ApplyValue %ld",
                     GetName(), static_cast<unsigned>(element.bApplyOn), static_cast<long>(element.lApplyValue));
            continue;
        }
        if (element.dwFlag > 64) {
            _sys_err("LoadAffect", 0x1bf, "invalid affect flag %s %lu", GetName(), static_cast<unsigned long>(element.dwFlag));
            continue;
        }
        if (test_server)
            sys_log(0, "Load Affect : Affect %s %d %d", GetName(), static_cast<int>(element.dwType),
                    element.bApplyOn);

        auto* affect = new CAffect{element.dwType, element.bApplyOn, element.lApplyValue,
                                   element.dwFlag, element.lDuration, element.lSPCost};
        m_list_pkAffect.push_back(affect);
        if (DESC* current = GetDesc())
            SendAffectAddPacket(current, affect);
        ComputeAffect(affect, true);
    }
    if (CArenaManager::instance().IsArenaMap(GetMapIndex()))
        RemoveGoodAffect();

    if (oldFlags != m_afAffectFlag ||
        moveSpeed != GetPoint(POINT_MOV_SPEED) || attackSpeed != GetPoint(POINT_ATT_SPEED))
        UpdatePacket();
    StartAffectEvent();
}
