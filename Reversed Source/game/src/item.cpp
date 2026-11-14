#include <cstdint>
#include <string.h>
// Target-backed old item lifecycle and ground packet encoding.
#include "item.h"
#include "utils.h"
#include "item_addon.h"
#include "item_manager.h"
#include "log.h"
#include "dev_log.h"
#include "config.h"
#include "constants.h"
#include "locale.hpp"
#include "marriage.h"
#include "char.h"
#include "desc.h"
#include "sectree.h"
#include "sectree_manager.h"
#include "packet.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/memcpy.h"
#include "../../libthecore/include/utils.h"
#include <cassert>
#include <cstring>
#include <cstdlib>

namespace apply_item_detail {
inline int bits(unsigned int value);
inline int sub(int a, int b);
inline int mul(int a, int b);
}

// 08168370
DWORD CItem::GetRefineFromVnum()
{
    const DWORD vnum = GetVnum();
    return ITEM_MANAGER::instance().GetRefineFromVnum(vnum);
}

// 08166260/08166290: item scalar initialization is explicitly separate.
CItem::CItem() {}
CItem::~CItem() { Destroy(); }
void CItem::Initialize() {
    CEntity::Initialize(1);
    m_bWindow=0; m_pOwner=0; m_dwID=0; m_bEquipped=false;
    SetFlag(0); m_dwCount=0; m_bCell=0; m_dwVID=0; m_pProto=0;
    m_bExchanging=false;
    std::memset(m_alSockets,0,sizeof(m_alSockets));
    std::memset(m_aAttr,0,sizeof(m_aAttr));
    m_pkDestroyEvent=0; m_pkOwnershipEvent=0; m_dwOwnershipPID=0;
    m_pkUniqueExpireEvent=0; m_pkAccessorySocketExpireEvent=0;
    m_bSkipSave=false; m_dwLastOwnerPID=0;
}
void CItem::Destroy() {
    event_cancel(&m_pkDestroyEvent);
    event_cancel(&m_pkOwnershipEvent);
    event_cancel(&m_pkUniqueExpireEvent);
    event_cancel(&m_pkAccessorySocketExpireEvent);
    CEntity::Destroy();
    if(GetSectree()) GetSectree()->RemoveEntity(this);
}
// 081667a0
void CItem::RemoveFlag(long bit)
{
    SetFlag(apply_item_detail::bits(static_cast<unsigned int>(GetFlag()) & ~static_cast<unsigned int>(bit)));
}

// 081667b0
void CItem::AddFlag(long bit)
{
    SetFlag(apply_item_detail::bits(static_cast<unsigned int>(GetFlag()) | static_cast<unsigned int>(bit)));
}

void CItem::SetProto(TItemTable* table) {
    assert(table);
    m_pProto=table; SetFlag(apply_item_detail::bits(table->dwFlags));
}
void CItem::EncodeInsertPacket(LPENTITY entity) {
    if (auto* desc = entity->GetDesc()) {
        TPacketGCItemGroundAdd packet;
        packet.bHeader=26;
        packet.x=GetX(); packet.y=GetY(); packet.z=GetZ();
        packet.dwVnum=m_pProto->dwVnum; packet.dwVID=m_dwVID;
        desc->Packet(&packet,sizeof(packet));
        const char* recipient = static_cast<CHARACTER*>(entity)->GetName();
        const char* name = m_pProto ? m_pProto->szLocaleName : nullptr;
        sys_log(2, "Item::EncodeInsertPacket %s to %s", name, recipient);
    }
}
void CItem::EncodeRemovePacket(LPENTITY entity) {
    if (auto* desc = entity->GetDesc()) {
        TPacketGCItemGroundDel packet;
        packet.bHeader=27; packet.dwVID=m_dwVID;
        desc->Packet(&packet,sizeof(packet));
        const char* recipient = static_cast<CHARACTER*>(entity)->GetName();
        const char* name = m_pProto ? m_pProto->szLocaleName : nullptr;
        sys_log(2, "Item::EncodeRemovePacket %s to %s", name, recipient);
    }
}

// 08167c40: skip-save is checked on enqueue, not inside SaveSingleItem.
void CItem::Save() {
    if(!m_bSkipSave) ITEM_MANAGER::instance().DelayedSave(this);
}
// 081667d0: the visible count is narrowed to one byte; the DB record keeps DWORD count.
void CItem::UpdatePacket() {
    if(m_pOwner && m_pOwner->GetDesc()) {
        TPacketGCItemUpdate packet;
        packet.header=25; packet.pos=m_bCell; packet.count=static_cast<BYTE>(m_dwCount);
        for(int i=0;i<3;++i) packet.alSockets[i]=m_alSockets[i];
        thecore_memcpy(packet.aAttr, m_aAttr, sizeof(m_aAttr));
        const char* ownerName = m_pOwner->GetName();
        const char* itemName = GetName();
        sys_log(2, "UpdatePacket %s -> %s", itemName, ownerName);
        m_pOwner->GetDesc()->Packet(&packet,sizeof(packet));
    }
}

// 08166af0: the inventory bound checks the item's current cell, not the argument.
bool CItem::AddToCharacter(CHARACTER* character,BYTE cell) {
    assert(!GetSectree());
    assert(!m_pOwner);
    if(m_bCell>=90) return false;
    if(character->GetDesc()) m_dwLastOwnerPID = static_cast<std::uint32_t>(character->GetPlayerID());
    event_cancel(&m_pkDestroyEvent);
    character->SetItem(cell,this);
    m_pOwner=character;
    Save();
    return true;
}

// Old event info has one native item pointer (4 bytes in the target).
struct item_event_info { CItem* item; };

long item_destroy_event(LPEVENT event)
{
    CItem* item = static_cast<item_event_info*>(event->info)->item;
    if (CHARACTER* owner = item->GetOwner()) {
        const char* ownerName = owner->GetName();
        const char* itemName = item->GetName();
        _sys_err("item_destroy_event", 0x4f, "item_destroy_event: Owner exist. (item %s owner %s)",
                 itemName, ownerName);
    }
    item->SetDestroyEvent(0);
    ITEM_MANAGER::instance().DestroyItem(item);
    return 0;
}

void CItem::SetDestroyEvent(LPEVENT event)
{
    m_pkDestroyEvent = event;
}

void CItem::StartDestroyEvent()
{
    if (m_pkDestroyEvent)
        return;

    void* data;
    const DWORD infoId = __AllocEventInfo(sizeof(item_event_info), &data);
    static_cast<item_event_info*>(data)->item = this;
    SetDestroyEvent(event_create_ex(item_destroy_event, infoId, apply_item_detail::mul(passes_per_sec, 300),
                                    const_cast<char*>("item.cpp"), 100));
}
// 08167f50: expiry releases the handle but deliberately retains the stored PID.
long ownership_event(LPEVENT event) {
    CItem* item=static_cast<item_event_info*>(event->info)->item;
    item->SetOwnershipEvent(0);
    TPacketGCItemOwnership packet{};
    packet.bHeader=31; packet.dwVID=item->GetVID(); packet.szName[0]=0;
    item->PacketAround(&packet,sizeof(packet));
    return 0;
}
// 08167fb0: this setter does not change the PID.
void CItem::SetOwnershipEvent(LPEVENT event) { m_pkOwnershipEvent=event; }
bool CItem::DistanceValid(CHARACTER* character)
{
    if (!GetSectree() || !character)
        return false;
    const auto y = GetY();
    const auto x = GetX();
    const auto dy = apply_item_detail::sub(y, character->GetY());
    const auto dx = apply_item_detail::sub(x, character->GetX());
    return DISTANCE_APPROX(dx, dy) < 301;
}

bool CItem::IsOwnership(CHARACTER* character) {
    return !m_pkOwnershipEvent || m_dwOwnershipPID==character->GetPlayerID();
}
void CItem::SetOwnership(CHARACTER* character,int seconds) {
    TPacketGCItemOwnership packet{};
    if(!character) {
        if(!m_pkOwnershipEvent) return;
        event_cancel(&m_pkOwnershipEvent);
        m_dwOwnershipPID=0;
        packet.bHeader=31; packet.dwVID=m_dwVID; packet.szName[0]=0;
    } else {
        if(m_pkOwnershipEvent) return;
        m_dwOwnershipPID=character->GetPlayerID();
        item_event_info* info;
        DWORD id=__AllocEventInfo(sizeof(item_event_info),reinterpret_cast<void**>(&info));
        info->item=this;
        SetOwnershipEvent(event_create_ex(ownership_event, id, apply_item_detail::mul(seconds, passes_per_sec),
                                          const_cast<char*>("item.cpp"), 934));
        packet.bHeader=31; packet.dwVID=m_dwVID;
        std::strncpy(packet.szName,character->GetName(),24); packet.szName[24]=0;
    }
    PacketAround(&packet,sizeof(packet));
}
bool CItem::AddToGround(long mapIndex, const PIXEL_POSITION& position)
{
    if (GetSectree()) {
        _sys_err("AddToGround", 0x137, "sectree already assigned");
        return false;
    }
    if (m_pOwner) {
        _sys_err("AddToGround", 0x13d, "owner pointer not null");
        return false;
    }

    auto& sectors = SECTREE_MANAGER::instance();
    const auto y = position.y;
    const auto x = position.x;
    LPSECTREE sectree = sectors.Get(mapIndex, x, y);
    if (!sectree) {
        const int loggedY = static_cast<int>(position.y);
        const int loggedX = static_cast<int>(position.x);
        _sys_err("AddToGround", 0x145, "cannot find sectree by %dx%d", loggedX, loggedY);
        return false;
    }

    m_bWindow = 5;
    SetXYZ(position);
    sectree->InsertEntity(this);
    UpdateSectree();
    Save();
    return true;
}

CItem* CItem::RemoveFromGround() {
    if(GetSectree()) {
        SetOwnership(0,10);
        GetSectree()->RemoveEntity(this);
        ViewCleanup();
        Save();
    }
    return this;
}

// 08167be0: the prototype is required; all six signed values are exposed.
// 0x081683e0
int CItem::GetRefineLevel()
{
    const char* name = GetName();
    const char* suffix = name ? std::strrchr(name, '+') : nullptr;
    return suffix ? std::atoi(suffix + 1) : 0;
}

long CItem::GetValue(DWORD idx) {
    if (idx >= 6) {
        assert(idx < 6);
        std::abort();
    }
    return m_pProto->alValues[idx];
}
// 08167d70: bulk replacement schedules persistence without a client update.
// 08166760
void CItem::UsePacketEncode(CHARACTER* character, CHARACTER* victim, packet_item_use* packet)
{
    if (!GetVnum())
        return;
    packet->header = 0x16;
    packet->ch_vid = static_cast<DWORD>(character->GetVID());
    packet->victim_vid = static_cast<DWORD>(victim->GetVID());
    packet->pos = m_bCell;
    packet->vnum = GetVnum();
}

// 08167ca0
bool CItem::CreateSocket(BYTE slot, BYTE gold)
{
    assert(slot < 3);
    if (slot >= 3)
        return false;
    if (static_cast<std::uint32_t>(m_alSockets[slot])) {
        const char* name = GetName();
        _sys_err("CreateSocket", 0x33a, "Item::CreateSocket : socket already exist %s %d",
                 name ? name : "", static_cast<int>(slot));
        return false;
    }
    m_alSockets[slot] = gold ? 2 : 1;
    UpdatePacket();
    Save();
    return true;
}

// 08168430
bool CItem::IsPolymorphItem()
{
    return GetType() == 19;
}

// 08168df0
long CItem::FindApplyValue(BYTE type)
{
    if (m_pProto) {
        for (const auto& apply : m_pProto->aApplies)
            if (apply.bType == type)
                return apply.lValue;
    }
    return 0;
}

void CItem::SetSockets(const long* sockets) {
    if (sizeof(long) == sizeof(std::int32_t)) {
        SetPackedSockets(sockets);
        return;
    }
    const std::int32_t packed[] = {
        static_cast<std::int32_t>(sockets[0]),
        static_cast<std::int32_t>(sockets[1]),
        static_cast<std::int32_t>(sockets[2]),
    };
    SetPackedSockets(packed);
}

void CItem::SetPackedSockets(const void* sockets) {
    if (sizeof(long) == sizeof(std::int32_t)) {
        thecore_memcpy(m_alSockets, sockets, 3 * sizeof(std::int32_t));
    } else {
        std::int32_t destination[3];
        for (int index = 0; index < 3; ++index)
            destination[index] = static_cast<std::int32_t>(m_alSockets[index]);
        thecore_memcpy(destination, sockets, sizeof(destination));
        for (int index = 0; index < 3; ++index)
            m_alSockets[index] = destination[index];
    }
    Save();
}

// 08167db0: the old signed guard checks only the upper bound.
void CItem::SetSocket(int index,long value) {
    if (index >= 3) {
        assert(index < 3);
        std::abort();
    }
    const int socket = apply_item_detail::bits(static_cast<unsigned int>(value));
    m_alSockets[index] = socket;
    UpdatePacket(); Save();
    dev_log("item.cpp",853,"SetSocket",L_DEB1,const_cast<char*>("ITEM SetSocket (name,apply_type,apply_value) = (%s, %d, %d)"),GetName(),index,socket);
    LogManager::instance().ItemLog(index,static_cast<unsigned int>(socket),0,m_dwID,"SET_SOCKET","");
}
// 08168450
long unique_expire_event(LPEVENT event)
{
    CItem* item = static_cast<item_event_info*>(event->info)->item;
    if (item->GetValue(2) == 0) {
        const auto expiry = static_cast<std::int32_t>(item->GetSockets()[2]);
        if (expiry > 1) {
            item->SetSocket(2, expiry - 1);
            return apply_item_detail::mul(passes_per_sec, 60);
        }
        const char* name = item->GetName();
        const auto id = item->GetID();
        sys_log(0, "UNIQUE_ITEM: expire %s %lu", name, id);
    } else {
        const int now = apply_item_detail::bits(static_cast<unsigned>(get_global_time()));
        if (now <= static_cast<std::int32_t>(item->GetSockets()[2]))
            return apply_item_detail::mul(passes_per_sec, 600);
    }

    item->SetUniqueExpireEvent(nullptr);
    ITEM_MANAGER::instance().RemoveItem(item, "UNIQUE_EXPIRE");
    return 0;
}

void CItem::SetUniqueExpireEvent(LPEVENT event)
{
    m_pkUniqueExpireEvent = event;
}

void CItem::StartUniqueExpireEvent()
{
    if (GetType() != 16 || m_pkUniqueExpireEvent)
        return;
    if (GetVnum() == 70048 && m_pOwner)
        m_pOwner->ShowAlignment(false);

    const auto saved = static_cast<std::int32_t>(m_alSockets[1]);
    const int seconds = !saved || saved > 60 ? 60 : saved;
    SetSocket(1, 0);
    void* eventData;
    const DWORD info = __AllocEventInfo(sizeof(item_event_info), &eventData);
    static_cast<item_event_info*>(eventData)->item = this;
    SetUniqueExpireEvent(event_create_ex(unique_expire_event, info, apply_item_detail::mul(seconds, passes_per_sec),
        const_cast<char*>("item.cpp"), 0x473));
}

// 08168680: wall-clock unique items keep running when unequipped.
void CItem::StopUniqueExpireEvent() {
    if(m_pkUniqueExpireEvent && GetValue(2)==0) {
        if(GetVnum()==70048) m_pOwner->ShowAlignment(true);
        const auto remaining = static_cast<std::int32_t>(event_time(m_pkUniqueExpireEvent));
        SetSocket(1, remaining / passes_per_sec);
        event_cancel(&m_pkUniqueExpireEvent);
    }
}

// 08168110: count only the uninterrupted prefix of occupied sockets.
bool CItem::AddSocket()
{
    const int count = GetSocketCount();
    if (count == 3)
        return false;
    m_alSockets[count] = 1;
    return true;
}

int CItem::GetSocketCount() {
    for (int index = 0; index < 3; ++index) {
        if (static_cast<std::uint32_t>(m_alSockets[index]) == 0)
            return index;
    }
    return 3;
}
// 08168810: the old accessory predicate requires an assigned prototype.
// 08168cf0
bool CItem::CanPutInto(CItem* item)
{
    if (item->GetType() != 2)
        return false;
    const DWORD targetVnum = item->GetVnum();
    DWORD base;
    switch (item->GetSubType()) {
    case 3: base = 14000; break;
    case 5: base = 16000; break;
    case 6: base = 17000; break;
    default: return false;
    }
    const auto group = static_cast<unsigned int>(targetVnum - base) / 20;
    return group < 12 && GetVnum() == group + 50623;
}

bool CItem::IsAccessoryForSocket() {
    return m_pProto->bType==2 &&
        (m_pProto->bSubType==3 || m_pProto->bSubType==5 || m_pProto->bSubType==6);
}
// 081688d0: use the normal socket mutation path even when the value is unchanged.
void CItem::SetAccessorySocketMaxGrade(int maxGrade) {
    SetSocket(1,MINMAX(0,maxGrade,3));
}

// Add includes "constants.h" and "locale.hpp" to item.cpp.
// 08168850; read the grade after the first real SetSocket callback chain.
void CItem::SetAccessorySocketGrade(int iGrade) {
    SetSocket(0, MINMAX(0, iGrade, GetAccessorySocketMaxGrade()));
    int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];
    if (test_server) iDownTime /= 60;
    SetAccessorySocketDownGradeTime(iDownTime);
}
// 08168910; retain original DWORD and raw CP949 bytes in an ASCII source file.
void CItem::SetAccessorySocketDownGradeTime(DWORD time) {
    const int seconds = apply_item_detail::bits(static_cast<unsigned>(time));
    SetSocket(2, seconds);
    if (test_server) {
        CHARACTER* ch = GetOwner();
        if (ch) {
            const char* name = GetName();
            const char* format = locale_find("\045\163\040\277\241\274\255\040\274\322\304\317\040\272\374\301\372\266\247\261\356\301\366\040\263\262\300\272\040\275\303\260\243\040\045\144", g_iUseLocale);
            ch->ChatPacket(1, format, name, seconds);
        }
    }
}

// 08168170, item.cpp. Input 3 also logs the clamped count.
void CItem::AlterToSocketItem(int iSocketCount) {
    if(iSocketCount>2) {
        iSocketCount=3;
        sys_log(0,"Invalid Socket Count %d, set to maximum",3);
    }
    for(int i=0;i<iSocketCount;++i) SetSocket(i,1);
}

// 08168720; include "item_addon.h" at original item.cpp include block.
void CItem::ApplyAddon(int iAddonType) {
    CItemAddonManager::instance().ApplyAddonTo(iAddonType,this);
}

// 081681e0. Exact old locale/type thresholds; independent second/third draws.
void CItem::AlterToMagicItem() {
    if(GetAttributeSetIndex()<0) return;
    int second,third;
    if(g_iUseLocale==0) {
        const BYTE type = GetType();
        if(type==1) {second=30;third=15;}
        else if(type!=2) return;
        else if(GetSubType()==0) {second=20;third=10;}
        else {second=10;third=5;}
    } else {
        const BYTE type = GetType();
        if(type==1) {second=20;third=5;}
        else if(type!=2) return;
        else if(GetSubType()==0) {second=10;third=2;}
        else {second=10;third=1;}
    }
    PutAttribute(aiItemMagicAttributePercentHigh);
    if(number_ex(1,100,const_cast<char*>("item.cpp"),1047)<=second)
        PutAttribute(aiItemMagicAttributePercentLow);
    if(number_ex(1,100,const_cast<char*>("item.cpp"),1050)<=third)
        PutAttribute(aiItemMagicAttributePercentLow);
}


// Original 08167f10. The prototype must be present.
// 08167ed0
int CItem::GetGold()
{
    if (GetFlag() & 0x08) {
        const auto gold = static_cast<std::uint32_t>(GetProto()->dwGold);
        const auto count = static_cast<std::uint32_t>(GetCount());
        return apply_item_detail::bits(gold ? count / gold : count);
    }
    return apply_item_detail::bits(static_cast<std::uint32_t>(GetProto()->dwGold));
}

int CItem::GetShopBuyPrice() { return m_pProto->dwShopBuyPrice; }

// C++03, original int/long32 x86 arithmetic. Include <string.h>.
namespace apply_item_detail {
typedef char require_int32[sizeof(int)==4?1:-1];
typedef char require_long32[sizeof(long)==4?1:-1];
inline int bits(unsigned int value) { int result; memcpy(&result,&value,4); return result; }
inline int add(int a,int b) { return bits(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline int sub(int a,int b) { return bits(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline int mul(int a,int b) { return bits(static_cast<unsigned int>(a)*static_cast<unsigned int>(b)); }
inline int negate(int value) { return bits(0u-static_cast<unsigned int>(value)); }
inline int removal(int value,bool add) { return add?value:negate(value); }
inline int skill(int value,bool add) { return add?value:bits(static_cast<unsigned int>(value)^0x00800000u); }
}

// Original item.cpp, CItem::ModifyPoints 08167420. Full body; requires REAL ApplyPoint.
void CItem::ModifyPoints(bool bAdd)
{
    if (!IsAccessoryForSocket() && static_cast<BYTE>(m_pProto->bType-1)<2) {
        for (int i=0;i<3;++i) {
            const auto dwVnum = static_cast<std::uint32_t>(m_alSockets[i]);
            if (dwVnum>2) {
                TItemTable* p=ITEM_MANAGER::instance().GetTable(dwVnum);
                if (!p) _sys_err("ModifyPoints",0x247,"cannot find table by vnum %u",dwVnum);
                else if (p->bType==10) {
                    for (int j=0;j<3;++j) {
                        if (p->aApplies[j].bType) {
                            int value=p->aApplies[j].lValue;
                            value=p->aApplies[j].bType==APPLY_SKILL?apply_item_detail::skill(value,bAdd):apply_item_detail::removal(value,bAdd);
                            m_pOwner->ApplyPoint(p->aApplies[j].bType,value);
                        }
                    }
                }
            }
        }
    }
    for (int i=0;i<3;++i) {
        TItemTable* p=m_pProto;
        if (p->aApplies[i].bType) {
            sys_log(2,"%s %d %d",p?p->szLocaleName:static_cast<const char*>(0),static_cast<unsigned int>(m_pProto->aApplies[i].bType),p->aApplies[i].lValue);
            p=m_pProto;
            int value;
            if (p->aApplies[i].bType==APPLY_SKILL) value=apply_item_detail::skill(p->aApplies[i].lValue,bAdd);
            else {
                if (IsAccessoryForSocket()) {
                    value=m_pProto->aApplies[i].lValue;
                    int grade=MINMAX(0,m_alSockets[0],3);
                    if (grade) value=apply_item_detail::add(value,MAX(grade,apply_item_detail::mul(value,aiAccessorySocketEffectivePct[grade])/100));
                } else value=m_pProto->aApplies[i].lValue;
                value=apply_item_detail::removal(value,bAdd);
            }
            m_pOwner->ApplyPoint(m_pProto->aApplies[i].bType,value);
        }
    }
    for (int i=0;i<7;++i) {
        if (m_aAttr[i].bType) {
            int value=m_aAttr[i].sValue;
            value=m_aAttr[i].bType==APPLY_SKILL?apply_item_detail::skill(value,bAdd):apply_item_detail::removal(value,bAdd);
            m_pOwner->ApplyPoint(m_aAttr[i].bType,value);
        }
    }
    TItemTable* p=m_pProto;
    BYTE type=p->bType;
    if (type==2) {
        BYTE subtype=p?p->bSubType:0;
        if (subtype!=0 && (p?p->bSubType:0)!=1 && (p?p->bSubType:0)!=4 && (p?p->bSubType:0)!=2) return;
        if (bAdd) {
            if (p->bSubType!=0) return;
            m_pOwner->SetPart(0,static_cast<WORD>(p->dwVnum));
        } else {
            if (p->bSubType!=0) return;
            const WORD originalPart = m_pOwner->GetOriginalPart(0);
            m_pOwner->SetPart(0, originalPart);
        }
    } else if (type==1 || type==13 || type==24) {
        if (m_bCell!=94) return;
        WORD value=bAdd?(p?static_cast<WORD>(p->dwVnum):0):m_pOwner->GetOriginalPart(1);
        m_pOwner->SetPart(1,value);
    }
}

bool CItem::SetCount(DWORD count)
{
    const auto itemCount = static_cast<std::uint32_t>(count);
    m_dwCount = itemCount;
    CHARACTER* owner;
    if (itemCount != 0 || (owner = m_pOwner) == nullptr) {
        UpdatePacket();
        Save();
        return true;
    }
    if(GetSubType()==7 || GetSubType()==0) {
        const BYTE cell=m_bCell;
        RemoveFromCharacter();
        LPITEM next=owner->FindSpecifyItem(GetVnum()); // Original reloads prototype after removal.
        if(next) owner->ChainQuickslotItem(next,1,cell);
        else owner->SyncQuickslot(1,cell,255);
        ITEM_MANAGER::instance().DestroyItem(this);
    } else {
        owner->SyncQuickslot(1,m_bCell,255);
        ITEM_MANAGER& manager=ITEM_MANAGER::instance(); // Captured BEFORE removal.
        LPITEM removed=RemoveFromCharacter();
        manager.DestroyItem(removed);
    }
    return false;
}

// Complete original08166a30. Unequip result intentionally ignored.
LPITEM CItem::RemoveFromCharacter()
{
    if(!m_pOwner) _sys_err("RemoveFromCharacter",235,"Item::RemoveFromCharacter owner null");
    else {
        if(!m_bEquipped) {
            if(m_bWindow!=3 && m_bWindow!=4) {
                if(m_bCell<90) m_pOwner->SetItem(m_bCell,0);
                else _sys_err("RemoveFromCharacter",255,"CItem::RemoveFromCharacter: pos >= INVENTORY_MAX_NUM");
            }
            m_pOwner=0;
            m_bCell=0;
        } else Unequip();
        m_bWindow=0;
        Save();
    }
    return this;
}
// Complete original08167a70. No m_bEquipped guard, no added null/proto guards.
int CItem::FindEquipCell(CHARACTER* character)
{
    if (!character || !m_pProto || !GetWearFlag() || GetType() == 26)
        return -1;
    const BYTE job = character->GetJob();
    if (job < 4 && (GetAntiFlag() & (4UL << job)))
        return -1;

    constexpr const char* limitMessages[] = {
        "",
        "\267\271\272\247\300\314\040\263\267\276\306\040\302\370\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056",
        "\261\331\267\302\300\314\040\263\267\276\306\040\302\370\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056",
        "\271\316\303\270\300\314\040\263\267\276\306\040\302\370\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056",
        "\301\366\264\311\300\314\040\263\267\276\306\040\302\370\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056",
        "\303\274\267\302\300\314\040\263\267\276\306\040\302\370\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056",
    };
    constexpr BYTE limitPoints[] = {0, POINT_LEVEL, POINT_ST, POINT_DX, POINT_IQ, POINT_HT};
    for (const TItemLimit& limit : m_pProto->aLimits) {
        const auto required = limit.lValue;
        const BYTE type = limit.bType;
        if (!type || type >= sizeof(limitPoints))
            continue;
        const long actual = type == 1 ? character->GetLevel() : character->GetPoint(limitPoints[type]);
        if (actual < required) {
            character->ChatPacket(1, LC_TEXT(limitMessages[type]));
            return -1;
        }
    }

    constexpr struct { DWORD flag; BYTE cell; } wear[] = {
        {1, 0}, {2, 1}, {4, 2}, {8, 3}, {0x10, 4}, {0x100, 10},
        {0x20, 5}, {0x40, 6}, {0x200, 9},
    };
    for (const auto& entry : wear) {
        if (GetWearFlag() & entry.flag)
            return entry.cell;
    }

    if (!(GetWearFlag() & 0x80)) {
        if (GetWearFlag() & 0x800) {
            for (BYTE slot = 11; slot < 19; ++slot) {
                if (!character->GetWear(slot))
                    return slot;
            }
        }
        return -1;
    }

    marriage::CManager& marriages = marriage::CManager::instance();
    if (marriages.IsMarriageUniqueItem(GetVnum()) && !marriages.IsMarried(character->GetPlayerID())) {
        character->ChatPacket(1, LC_TEXT(
            "\260\341\310\245\307\317\301\366\040\276\312\300\272\040\273\363\305\302\277\241\274\255\040"
            "\277\271\271\260\300\273\040\302\370\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056"));
        return -1;
    }

    const CItem* const first = character->GetWear(7);
    CItem* other = first ? character->GetWear(7) : character->GetWear(8);
    const int group = other ? other->GetSpecialGroup() : 0;
    if (!other || (other->GetVnum() != GetVnum() && (!group || group != GetSpecialGroup())))
        return first ? 8 : 7;
    character->ChatPacket(1, LC_TEXT(
        "\260\260\300\272\040\301\276\267\371\300\307\040\300\257\264\317\305\251\040\276\306\300\314\305\333\040"
        "\265\316\040\260\263\270\246\040\265\277\275\303\277\241\040\300\345\302\370\307\322\040\274\366\040"
        "\276\370\275\300\264\317\264\331\056"));
    return -1;
}

bool CItem::EquipTo(CHARACTER* character, BYTE wearCell)
{
    if (!character) {
        _sys_err("EquipTo", 0x2be, "EquipTo: nil character");
        return false;
    }
    if (wearCell >= 19) {
        const auto* proto = GetProto();
        const int flags = proto ? static_cast<int>(proto->dwWearFlags) : 0;
        const char* name = proto ? proto->szLocaleName : nullptr;
        const int vnum = proto ? static_cast<int>(proto->dwVnum) : 0;
        _sys_err("EquipTo", 0x2c4, "EquipTo: invalid wear cell (this: #%d %s wearflag: %d cell: %d)",
                 vnum, name, flags, static_cast<int>(wearCell));
        return false;
    }
    if (character->GetWear(wearCell)) {
        const auto existing = character->GetWear(wearCell);
        const char* existingName = existing ? existing->GetName() : "";
        const char* incomingName = GetName();
        const int incomingVnum = static_cast<int>(GetVnum());
        _sys_err("EquipTo", 0x2ca, "EquipTo: item already exist (this: #%d %s cell: %d %s)",
                 incomingVnum, incomingName, static_cast<int>(wearCell), existingName);
        return false;
    }

    if (m_pOwner)
        RemoveFromCharacter();
    character->SetWear(wearCell, this);
    m_bEquipped = true;
    m_bCell = wearCell + 90;
    m_pOwner = character;
    DWORD immune = 0;
    for (BYTE slot = 0; slot < 19; ++slot) {
        if (!m_pOwner->GetWear(slot))
            continue;
        if (CItem* item = m_pOwner->GetWear(slot))
            immune |= item->GetImmuneFlag();
    }
    m_pOwner->SetImmuneFlag(immune);
    ModifyPoints(true);
    m_pOwner->ComputeBattlePoints();
    m_pOwner->UpdatePacket();
    Save();
    StartUniqueExpireEvent();
    StartAccessorySocketExpireEvent();
    return true;
}

bool CItem::Unequip()
{
    CHARACTER* owner=m_pOwner;
    BYTE cell=m_bCell;
    if(!owner || cell<90) {
        const char* name = GetName();
        const auto id = static_cast<std::uint32_t>(m_dwID);
        _sys_err("Unequip", 762, "%s %lu m_pOwner %p, GetCell %d",
                 name, static_cast<unsigned long>(id), static_cast<void*>(owner), static_cast<int>(cell));
        return false;
    }
    if(owner->GetWear((BYTE)(cell-90))!=this) {
        _sys_err("Unequip",769,"m_pOwner->GetWear() != this");
        return false;
    }
    StopUniqueExpireEvent();
    StopAccessorySocketExpireEvent();
    ModifyPoints(false);
    m_pOwner->SetWear((BYTE)(m_bCell-90),0);
    DWORD immune=0;
    for(int i=0;i<19;++i) {
        if(m_pOwner->GetWear((BYTE)i)) immune|=m_pOwner->GetWear((BYTE)i)->m_pProto->dwImmuneFlag;
    }
    owner=m_pOwner;
    owner->SetImmuneFlag(immune);
    owner->ComputeBattlePoints();
    m_pOwner->UpdatePacket();
    m_pOwner=0;m_bCell=0;m_bEquipped=false;
    return true;
}

// Original item.cpp event-info record, original size4; not a polymorphic event_info.
struct item_vid_event_info { DWORD item_vid; };

long accessory_socket_expire_event(LPEVENT event)
{
    item_vid_event_info* info=static_cast<item_vid_event_info*>(event->info);
    LPITEM item=ITEM_MANAGER::instance().FindByVID(info->item_vid);
    int current=item->GetAccessorySocketDownGradeTime();
    DWORD time;
    if (current<2 || apply_item_detail::bits(time=static_cast<DWORD>(current)-60u)<2) {
        item->SetAccessorySocketExpireEvent(0);
        item->AccessorySocketDegrade();
        return 0;
    }
    item->SetAccessorySocketDownGradeTime(time);
    return apply_item_detail::bits(time)<61?apply_item_detail::mul(time,passes_per_sec):apply_item_detail::mul(passes_per_sec,60);
}

void CItem::StartAccessorySocketExpireEvent()
{
    if (IsAccessoryForSocket() && !m_pkAccessorySocketExpireEvent &&
        static_cast<std::uint32_t>(m_alSockets[1]) != 0 &&
        static_cast<std::uint32_t>(m_alSockets[0]) != 0) {
        int seconds=m_alSockets[2];
        SetAccessorySocketExpireEvent(0);
        if (seconds<2) seconds=5;
        else seconds=MIN(seconds,60);
        void* rawInfo = nullptr;
        const DWORD id = __AllocEventInfo(sizeof(item_vid_event_info), &rawInfo);
        static_cast<item_vid_event_info*>(rawInfo)->item_vid = m_dwVID;
        LPEVENT event=event_create_ex(accessory_socket_expire_event,id,apply_item_detail::mul(seconds,passes_per_sec),const_cast<char*>("item.cpp"),0x4e8);
        SetAccessorySocketExpireEvent(event);
    }
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT event)
{
    m_pkAccessorySocketExpireEvent=event;
}

void CItem::AccessorySocketDegrade()
{
    if (m_alSockets[0]>0) {
        LPCHARACTER ch=m_pOwner;
        if (ch) {
            const TItemTable* proto=m_pProto;
            const char* format=locale_find("\x25\x73\xbf\xa1\x20\xb9\xda\xc7\xf4\xc0\xd6\xb4\xf8\x20\xba\xb8\xbc\xae\xc0\xcc\x20\xbb\xe7\xb6\xf3\xc1\xfd\xb4\xcf\xb4\xd9\x2e",g_iUseLocale);
            ch->ChatPacket(1,format,proto?proto->szLocaleName:static_cast<const char*>(0));
        }
        ModifyPoints(false);
        SetAccessorySocketGrade(apply_item_detail::sub(m_alSockets[0],1));
        ModifyPoints(true);
        DWORD time=aiAccessorySocketDegradeTime[m_alSockets[0]];
        if (test_server) time=apply_item_detail::bits(time)/60;
        SetAccessorySocketDownGradeTime(time);
        if (time) StartAccessorySocketExpireEvent();
    }
}

void CItem::StopAccessorySocketExpireEvent()
{
    LPEVENT event=m_pkAccessorySocketExpireEvent;
    if(event && IsAccessoryForSocket()) {
        const int oldTime=m_alSockets[2];
        const int remaining = apply_item_detail::bits(static_cast<unsigned>(event_time(event)));
        const int seconds = apply_item_detail::add(oldTime, apply_item_detail::sub(remaining / passes_per_sec, 60));
        event_cancel(&m_pkAccessorySocketExpireEvent);
        if (seconds < 2)
            AccessorySocketDegrade();
        else
            SetAccessorySocketDownGradeTime(seconds);
    }
}

// 08168780
int CItem::GetSpecialGroup()
{
    ITEM_MANAGER& manager = ITEM_MANAGER::instance();
    return manager.GetSpecialGroupFromItem(GetVnum());
}

void CItem::SetExchanging(bool on)
{
    m_bExchanging=on;
}

bool CItem::CheckItemUseLevel(int level)
{
    for (const auto& limit : m_pProto->aLimits) {
        if (limit.bType == 1)
            return level >= limit.lValue;
    }
    return true;
}

bool CItem::CanUsedBy(CHARACTER* character)
{
    const BYTE job = character->GetJob();
    return job >= 4 || !m_pProto || !(GetAntiFlag() & (4U << job));
}

// 08168d90
bool CItem::IsPCBangItem()
{
    if (m_pProto) {
        for (const auto& limit : m_pProto->aLimits) {
            if (limit.bType == 6)
                return true;
        }
    }
    return false;
}
