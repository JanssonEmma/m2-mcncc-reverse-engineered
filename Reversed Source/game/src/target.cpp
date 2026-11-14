#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "desc.h"
#include "packet.h"
#include "questmanager.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"
#include <cstdint>
#include <cstring>
#include "target.h"

CTargetManager::CTargetManager() : m_iID(0) {}
CTargetManager::~CTargetManager() {}

TargetInfo* CTargetManager::GetTargetInfo(DWORD pid, int type, int argument)
{
    const auto found = m_map_kListEvent.find(static_cast<std::uint32_t>(pid));
    if (found==m_map_kListEvent.end()) return 0;
    std::list<LPEVENT>::iterator it=found->second.begin();
    while (it!=found->second.end()) {
        TargetInfo* info=static_cast<TargetInfo*>(event_info(*it++));
        if ((info->iType&type) && info->iArg1==argument) return info;
    }
    return 0;
}

void SendTargetCreatePacket(DESC* desc, TargetInfo* info)
{
    if (!info->bSendToClient) return;
    TPacketGCTargetCreate packet;
    packet.bHeader=125;
    packet.lID=info->iID;
    packet.bType=info->iType;
    packet.dwVID=info->iArg1;
    std::strncpy(packet.szName,info->szTargetDesc,32);
    packet.szName[32]=0;
    desc->Packet(&packet, sizeof(packet));
}

void SendTargetDeletePacket(DESC* desc, int id)
{
    TPacketGCTargetDelete packet;
    packet.bHeader=124;
    packet.lID=id;
    desc->Packet(&packet, sizeof(packet));
}

void SendTargetUpdatePacket(DESC* desc, int id, int x, int y)
{
    TPacketGCTargetUpdate packet;
    packet.bHeader=123;
    packet.lID=id;
    packet.lX=x;
    packet.lY=y;
    desc->Packet(&packet, sizeof(packet));
    sys_log(0,"SendTargetUpdatePacket %d %dx%d",id,x,y);
}

namespace {
int TargetDistance(CHARACTER* owner, int x, int y)
{
    const auto dx = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(owner->GetX()) - static_cast<std::uint32_t>(x));
    const auto dy = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(owner->GetY()) - static_cast<std::uint32_t>(y));
    return DISTANCE_APPROX(dx, dy);
}

long TargetDelay(int distance)
{
    return MINMAX(passes_per_sec/2,distance/(1500/passes_per_sec),passes_per_sec*5);
}
}

long target_event(LPEVENT event)
{
    TargetInfo* info=static_cast<TargetInfo*>(event->info);
    CHARACTER* owner=info->pkChr;
    int distance=5000;
    if (info->iMapIndex != static_cast<std::int32_t>(owner->GetMapIndex())) return TargetDelay(distance);
    int x=0,y=0;
    CHARACTER* target=0;
    if (info->iType==TARGET_TYPE_POS) {
        x=info->iArg1;
        y=info->iArg2;
        distance=TargetDistance(owner,x,y);
    } else if (info->iType==TARGET_TYPE_VID) {
        target=CHARACTER_MANAGER::instance().Find(static_cast<std::uint32_t>(info->iArg1));
        if (target) {
            const auto ownerMap = static_cast<std::uint32_t>(owner->GetMapIndex());
            if (static_cast<std::uint32_t>(target->GetMapIndex()) == ownerMap) {
                x = target->GetX();
                y = target->GetY();
                distance = TargetDistance(owner, x, y);
            }
        }
    }
    bool result=true;
    if (distance < 501) {
        auto& manager = quest::CQuestManager::instance();
        const DWORD ownerId = owner->GetPlayerID();
        const auto questIndex = info->dwQuestIndex;
        result = manager.Target(ownerId, questIndex, info->szTargetName, "arrive");
    }
    if (!target && info->iType==TARGET_TYPE_VID) {
        const DWORD deadOwnerId = owner->GetPlayerID();
        quest::CQuestManager::instance().Target(deadOwnerId,info->dwQuestIndex,info->szTargetName,"die");
        const DWORD deleteOwnerId = owner->GetPlayerID();
        CTargetManager::instance().DeleteTarget(deleteOwnerId,info->dwQuestIndex,info->szTargetName);
    }
    if (event->is_force_to_end) {
        sys_log(0,"target_event: event canceled");
        return 0;
    }
    if (x!=info->iOldX || y!=info->iOldY) {
        if (info->bSendToClient) SendTargetUpdatePacket(owner->GetDesc(),info->iID,x,y);
        info->iOldX=x;
        info->iOldY=y;
    }
    return result ? TargetDelay(distance) : passes_per_sec;
}

LPEVENT CTargetManager::GetTargetEvent(DWORD pid, DWORD quest, const char* name)
{
    const auto found = m_map_kListEvent.find(static_cast<std::uint32_t>(pid));
    if (found==m_map_kListEvent.end()) return 0;
    std::list<LPEVENT>::iterator it=found->second.begin();
    while (it!=found->second.end()) {
        LPEVENT event=*it++;
        TargetInfo* info=static_cast<TargetInfo*>(event_info(event));
        if (info->dwQuestIndex == static_cast<std::uint32_t>(quest) && std::strcmp(info->szTargetName,name)==0) return event;
    }
    return 0;
}

void CTargetManager::CreateTarget(DWORD pid, DWORD quest, const char* name, int type,
                                 int arg1, int arg2, int map, const char* description, int send)
{
    sys_log(0,"CreateTarget : target pid %lu quest %lu name %s arg %d %d %d",pid,quest,name,type,arg1,arg2);
    CHARACTER* owner=CHARACTER_MANAGER::instance().FindByPID(pid);
    if (!owner) {
        _sys_err("CreateTarget",0xb0,"Cannot find character ptr by PID %lu",pid);
        return;
    }
    if (static_cast<std::int32_t>(owner->GetMapIndex()) != map) return;
    LPEVENT existing=GetTargetEvent(pid,quest,name);
    TargetInfo* info;
    DWORD infoID=0;
    if (existing) {
        info=static_cast<TargetInfo*>(event_info(existing));
        sys_log(0,"CreateTarget : same target will be replaced");
        if (info->bSendToClient) {
            const int id = info->iID;
            DESC* desc = owner->GetDesc();
            SendTargetDeletePacket(desc, id);
        }
    } else {
        void* rawInfo = nullptr;
        infoID = __AllocEventInfo(sizeof(TargetInfo), &rawInfo);
        info = static_cast<TargetInfo*>(rawInfo);
        info->pkChr=owner;
        info->dwQuestIndex = static_cast<std::uint32_t>(quest);
        info->dwPID = static_cast<std::uint32_t>(pid);
        std::strncpy(info->szTargetName,name,32);
        info->szTargetName[32]=0;
        info->iMapIndex=map;
    }
    if (description) {
        std::strncpy(info->szTargetDesc,description,32);
        info->szTargetDesc[32]=0;
    } else info->szTargetDesc[0]=0;
    m_iID = static_cast<std::int32_t>(static_cast<std::uint32_t>(m_iID) + 1u);
    info->iID = m_iID;
    info->iType=type;
    info->iArg1=arg1;
    info->iArg2=arg2;
    info->bSendToClient=send!=0;
    info->iOldX=info->iOldY=0;
    if (!existing) {
        std::list<LPEVENT>& events=m_map_kListEvent[static_cast<std::uint32_t>(pid)];
        events.push_back(event_create_ex(target_event,infoID,passes_per_sec,const_cast<char*>("target.cpp"),0xf6));
    }
    SendTargetCreatePacket(owner->GetDesc(),info);
}

void CTargetManager::DeleteTarget(DWORD pid, DWORD quest, const char* name)
{
    const auto found = m_map_kListEvent.find(static_cast<std::uint32_t>(pid));
    if (found==m_map_kListEvent.end()) return;
    std::list<LPEVENT>& events=found->second;
    for (std::list<LPEVENT>::iterator it=events.begin();it!=events.end();) {
        LPEVENT event=*it;
        TargetInfo* info=static_cast<TargetInfo*>(event_info(event));
        if (info->dwQuestIndex != static_cast<std::uint32_t>(quest) || (name && std::strcmp(info->szTargetName,name)!=0)) {
            ++it;
            continue;
        }
        if (info->bSendToClient) {
            DESC* desc = info->pkChr->GetDesc();
            const int id = info->iID;
            SendTargetDeletePacket(desc, id);
        }
        event_cancel(&event);
        it=events.erase(it);
    }
}

void CTargetManager::Logout(DWORD pid)
{
    const auto found = m_map_kListEvent.find(static_cast<std::uint32_t>(pid));
    if (found==m_map_kListEvent.end()) return;
    std::list<LPEVENT>::iterator it=found->second.begin();
    while (it!=found->second.end()) {
        LPEVENT& event=*it++;
        event_cancel(&event);
    }
    m_map_kListEvent.erase(found);
}
