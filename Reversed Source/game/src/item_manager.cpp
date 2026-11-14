#include "safebox.h"
#include "log.h"
#include "stdio.h"
#include "blend_item.h"
#include "skill.h"
#include "locale_service.h"
#include "utils.h"
#include "../../libthecore/include/utils.h"
#include "config.h"
#include "char_manager.h"
#include "constants.h"
#include "db.h"
#include "item_manager_private_types.h"
#include "priv_manager.h"
#include "../../libthecore/include/main.h"
#include <cassert>
// Old item manager lifecycle, registries and persistence scheduling.
#include "item_manager.h"
#include "item.h"
#include "char.h"
#include "desc_client.h"
#include "../../libthecore/include/log.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <strings.h>
#include <utility>
#include <algorithm>
#include "questmanager.h"
#include "../../libthecore/include/memcpy.h"

// 08190630/08190a40: m_iTopOfTable is deliberately not initialized here.
ITEM_MANAGER::ITEM_MANAGER() : m_dwVIDCount(0),m_dwCurrentID(0) {}
ITEM_MANAGER::~ITEM_MANAGER() { Destroy(); }
// 08191c50: wipe_second owns VID entries; other pointer maps are not wiped here.
void ITEM_MANAGER::Destroy() {
    for(std::map<DWORD,CItem*>::iterator it=m_VIDMap.begin();it!=m_VIDMap.end();++it) delete it->second;
    m_VIDMap.clear();
}
CItem* ITEM_MANAGER::Find(DWORD id) {
    std::map<DWORD,CItem*>::iterator it=m_map_pkItemByID.find(static_cast<unsigned int>(id));
    return it==m_map_pkItemByID.end() ? 0 : it->second;
}
CItem* ITEM_MANAGER::FindByVID(DWORD vid) {
    std::map<DWORD,CItem*>::iterator it=m_VIDMap.find(static_cast<unsigned int>(vid));
    return it==m_VIDMap.end() ? 0 : it->second;
}
DWORD ITEM_MANAGER::GetRefineFromVnum(DWORD vnum) {
    std::map<DWORD,DWORD>::iterator it=m_map_ItemRefineFrom.find(static_cast<unsigned int>(vnum));
    return it==m_map_ItemRefineFrom.end() ? 0 : it->second;
}
void ITEM_MANAGER::DelayedSave(CItem* item) {
    if(item->GetID()) m_set_pkItemForDelayedSave.insert(item);
}
void ITEM_MANAGER::FlushDelayedSave(CItem* item) {
    std::set<CItem*>::iterator it=m_set_pkItemForDelayedSave.find(item);
    if(it!=m_set_pkItemForDelayedSave.end()) {
        m_set_pkItemForDelayedSave.erase(it);
        SaveSingleItem(item);
    }
}
void ITEM_MANAGER::SaveSingleItem(CItem* item) {
    CHARACTER* owner = item->GetOwner();
    if (!owner) {
        const auto id = static_cast<std::uint32_t>(item->GetID());
        const auto owner = static_cast<std::uint32_t>(item->GetLastOwnerPID());
        db_clientdesc->DBPacketHeader(31, 0, sizeof(id) + sizeof(owner));
        db_clientdesc->Packet(&id,sizeof(id));
        db_clientdesc->Packet(&owner,sizeof(owner));
        sys_log(0,"ITEM_DELETE %s:%lu",item->GetName(),static_cast<unsigned long>(id));
    } else {
        const int window = item->GetWindow();
        const char* ownerName = owner->GetName();
        const char* itemName = item->GetName();
        const auto id = static_cast<std::int32_t>(item->GetID());
        sys_log(0, "ITEM_SAVE %s:%d in %s window %d", itemName, id, ownerName, window);
        TPlayerItem packet;
        packet.id=item->GetID(); packet.window=item->GetWindow();
        packet.pos=packet.window==2 ? item->GetCell()-90 : static_cast<WORD>(item->GetCell());
        packet.count=item->GetCount(); packet.vnum=item->GetVnum();
        if(packet.window==3 || packet.window==4) packet.owner=item->GetOwner()->GetDesc()->GetAccountTable().id;
        else packet.owner=item->GetOwner()->GetPlayerID();
        const long* sockets = item->GetSockets();
        if (sizeof(long) == sizeof(std::int32_t)) {
            thecore_memcpy(packet.alSockets, sockets, sizeof(packet.alSockets));
        } else {
            const std::int32_t packedSockets[] = {
                static_cast<std::int32_t>(sockets[0]),
                static_cast<std::int32_t>(sockets[1]),
                static_cast<std::int32_t>(sockets[2]),
            };
            thecore_memcpy(packet.alSockets, packedSockets, sizeof(packet.alSockets));
        }
        thecore_memcpy(packet.aAttr, item->GetAttributes(), sizeof(packet.aAttr));
        db_clientdesc->DBPacketHeader(30,0,sizeof(packet));
        db_clientdesc->Packet(&packet,sizeof(packet));
    }
}
void ITEM_MANAGER::Update() {
    std::set<CItem*>::iterator it=m_set_pkItemForDelayedSave.begin();
    while(it!=m_set_pkItemForDelayedSave.end()) {
        std::set<CItem*>::iterator current=it++;
        CItem* item=*current;
        if(!item->GetOwner() || !(item->GetFlag() & 0x10)) {
            SaveSingleItem(item);
            m_set_pkItemForDelayedSave.erase(current);
        }
    }
}
void ITEM_MANAGER::GracefulShutdown() {
    std::set<CItem*>::iterator it=m_set_pkItemForDelayedSave.begin();
    while(it!=m_set_pkItemForDelayedSave.end()) SaveSingleItem(*it++);
    m_set_pkItemForDelayedSave.clear();
}

namespace item_lookup_detail {
inline int add(int a,int b) { return static_cast<int>(static_cast<unsigned int>(a)+static_cast<unsigned int>(b)); }
inline int sub(int a,int b) { return static_cast<int>(static_cast<unsigned int>(a)-static_cast<unsigned int>(b)); }
inline int half_arithmetic(int value) {
    unsigned int bits=static_cast<unsigned int>(value);
    return static_cast<int>((bits>>1)|(bits&0x80000000U));
}
}
//08193080: negative RealNumber -> NULL, otherwise address of full prototype.
TItemTable* ITEM_MANAGER::GetTable(DWORD vnum) {
    int rnum=RealNumber(vnum);
    return rnum<0?0:&m_vec_prototype[rnum];
}
//081930c0: unsigned prototype-vnum compare, signed interval and SAR midpoint.
int ITEM_MANAGER::RealNumber(DWORD vnum) {
    const auto itemVnum = static_cast<std::uint32_t>(vnum);
    int bottom=0;
    const auto size = m_vec_prototype.size();
    int top = static_cast<int>(size);
    for (;;) {
        const int middle = item_lookup_detail::half_arithmetic(item_lookup_detail::add(top, bottom));
        if (middle < 0 || static_cast<std::size_t>(middle) >= size)
            return -1;
        const auto current = m_vec_prototype[middle].dwVnum;
        if (current == itemVnum)
            return middle;
        if (bottom >= top)
            return -1;
        if (current > itemVnum)
            top = item_lookup_detail::sub(middle, 1);
        else
            bottom = item_lookup_detail::add(middle, 1);
    }
}

// Add to FULL existing ITEM_MANAGER public API (both nonconst):
// DWORD GetNewID();
// bool SetMaxItemID(DWORD dwID);
// Config ownership: extern DWORD g_dwItemIDRange[2];
// Define ONCE in config.cpp: DWORD g_dwItemIDRange[2]={0,0};
// g_dwItemIDRange[1] is original address083231a8, NOT a second symbol.
DWORD ITEM_MANAGER::GetNewID()
{
    // The 2009 binary aborts if an ID is requested before the range is initialized.
    if (m_dwCurrentID == 0) {
        assert(m_dwCurrentID != 0);
        std::abort();
    }
    DWORD value=m_dwCurrentID;
    if(value<g_dwItemIDRange[1]) m_dwCurrentID=value+1;
    else {
        _sys_err("GetNewID",203,"FATAL ERROR! ITEM ID Overflow, contact your administrator!");
        thecore_shutdown(); // Existing REAL libthecore implementation.
        value=0;
    }
    return value;
}
bool ITEM_MANAGER::SetMaxItemID(DWORD id)
{
    if(id==0) return false;
    if(g_dwItemIDRange[0]<id && id<g_dwItemIDRange[1]) {
        m_dwCurrentID=id;
        sys_log(0,"ITEM: ID start at %lu",id);
        return true;
    }
    _sys_err("SetMaxItemID",218,"FATAL ERROR! item id out of range! id %lu, range %lu ~ %lu",id,g_dwItemIDRange[0],g_dwItemIDRange[1]);
    thecore_shutdown();
    return false;
}

// 08194a40 and original item_manager.cpp local event owner at 0842d2c8.
static struct DropEvent_CharStone {
    int percent_lv01_10;
    int percent_lv11_30;
    int percent_lv31_MX;
    int level_range;
    bool alive;

    DropEvent_CharStone()
        : percent_lv01_10(100), percent_lv11_30(200), percent_lv31_MX(300),
          level_range(10), alive(false) {}
} gs_dropEvent_charStone;

bool DropEvent_CharStone_SetValue(const std::string& name, int value)
{
    if (name == "drop_char_stone") {
        gs_dropEvent_charStone.alive = value != 0;
        sys_log(0, value ? "dropevent.drop_char_stone = on" : "dropevent.drop_char_stone = off");
    } else if (name == "drop_char_stone.percent_lv01_10") {
        gs_dropEvent_charStone.percent_lv01_10 = value;
    } else if (name == "drop_char_stone.percent_lv11_30") {
        gs_dropEvent_charStone.percent_lv11_30 = value;
    } else if (name == "drop_char_stone.percent_lv31_MX") {
        gs_dropEvent_charStone.percent_lv31_MX = value;
    } else if (name == "drop_char_stone.level_range") {
        gs_dropEvent_charStone.level_range = value;
    } else {
        return false;
    }

    sys_log(0, "dropevent.drop_char_stone: %d", static_cast<int>(gs_dropEvent_charStone.alive));
    // The target loads each signed int into x87 and stores the quotient directly as double.
    sys_log(0, "dropevent.drop_char_stone.percent_lv01_10: %f", gs_dropEvent_charStone.percent_lv01_10 / 100.0);
    sys_log(0, "dropevent.drop_char_stone.percent_lv11_30: %f", gs_dropEvent_charStone.percent_lv11_30 / 100.0);
    sys_log(0, "dropevent.drop_char_stone.percent_lv31_MX: %f", gs_dropEvent_charStone.percent_lv31_MX / 100.0);
    sys_log(0, "dropevent.drop_char_stone.level_range: %d", gs_dropEvent_charStone.level_range);
    return true;
}

void ITEM_MANAGER::DestroyItem(LPITEM item)
{
    if(item->GetSectree()) item->RemoveFromGround();
    if(item->GetOwner()) {
        const char* ownerName=item->GetOwner()->GetName();
        _sys_err("DestroyItem",501,"DestroyItem: GetOwner %s %s!!",item->GetName(),ownerName);
        item->RemoveFromCharacter();
    }
    std::set<CItem*>::iterator delayed=m_set_pkItemForDelayedSave.find(item);
    if(delayed!=m_set_pkItemForDelayedSave.end()) m_set_pkItemForDelayedSave.erase(delayed);
    const DWORD id=item->GetID();
    sys_log(0,"ITEM_DESTROY %s:%lu",item->GetName(),id);
    if(!item->GetSkipSave() && id!=0) {
        const auto wireID = static_cast<std::uint32_t>(id);
        const auto ownerID = static_cast<std::uint32_t>(item->GetLastOwnerPID());
        db_clientdesc->DBPacketHeader(31, 0, sizeof(wireID) + sizeof(ownerID));
        db_clientdesc->Packet(&wireID, sizeof(wireID));
        db_clientdesc->Packet(&ownerID, sizeof(ownerID));
    } else {
        const int skip = item->GetSkipSave();
        const char* name = item->GetName();
        sys_log(0, "ITEM_DESTROY_SKIP %s:%lu (skip=%d)", name, id, skip);
    }
    if(id!=0) m_map_pkItemByID.erase(id);
    m_VIDMap.erase(item->GetVID());
    delete item;
}

namespace auto_give_detail {
static int signed32(unsigned int v) { return v <= 0x7fffffffU ? (int)v : -1-(int)(0xffffffffU-v); }
}

LPITEM ITEM_MANAGER::CreateItem(DWORD vnum,DWORD count,DWORD id,bool tryMagic,int rarePct)
{
    vnum = static_cast<std::uint32_t>(vnum);
    id = static_cast<unsigned int>(id);
    if(!vnum) return 0;
    if((LC_IsKorea() || LC_IsYMIR()) && vnum==50300 && tryMagic) {
        DWORD skill;
        do { skill=number_ex(1,120,"item_manager.cpp",245); }
        while(!CSkillManager::instance().Get(skill));
        vnum=skill+50400;
    }
    TItemTable* table=GetTable(vnum);
    if(!table) return 0;
    if(m_map_pkItemByID.find(id)!=m_map_pkItemByID.end()) {
        LPITEM duplicate=m_map_pkItemByID[id];
        auto* owner = duplicate->GetOwner();
        const char* name = duplicate->GetName();
        _sys_err("CreateItem", 272, "ITEM_ID_DUP: %lu %s owner %p", id, name, static_cast<void*>(owner));
        return 0;
    }
    LPITEM item=new CItem;
    item->Initialize();
    item->SetProto(table);
    if(item->GetType()!=9) {
        if(id==0) {
            item->SetID(GetNewID());
            if(item->GetType()==16) {
                long value;
                if(item->GetValue(2)==0) value=item->GetValue(0);
                else {
                    const time_t now=get_global_time();
                    value=auto_give_detail::signed32((unsigned int)item->GetValue(0)+(unsigned int)now);
                }
                item->SetSocket(2,value);
            }
        } else {
            item->SetID(id);
            item->SetSkipSave(true);
        }
    } else item->SetSkipSave(true);
    if(item->GetType()!=9) {
        if(!item->IsStackable()) count=1;
        else {
            count=(DWORD)MINMAX(1,auto_give_detail::signed32(count),200);
            if(tryMagic && count<2 && (item->GetFlag() & 128)) count=item->GetValue(1);
        }
    }
    m_dwVIDCount = static_cast<std::uint32_t>(m_dwVIDCount + 1u);
    const DWORD vid = m_dwVIDCount;
    item->SetVID(vid);
    m_VIDMap.insert(std::make_pair(vid,item));
    if(item->GetID()!=0) m_map_pkItemByID.insert(std::make_pair(item->GetID(),item));
    if(!item->SetCount(count)) return 0;
    item->SetSkipSave(false);
    if(item->GetType()==16 && item->GetValue(2)!=0) item->StartUniqueExpireEvent();
    if(id==0) {
        if(item->GetType()==27 && Blend_Item_find(item->GetVnum())) {
            Blend_Item_set_value(item);
            return item;
        }
        const auto addonType = table->sAddonType;
        if (addonType != 0)
            item->ApplyAddon(addonType);
        if(tryMagic) {
            if(rarePct==-1) rarePct=table->bAlterToMagicItemPct;
            if(number_ex(1,100,"item_manager.cpp",361)<=rarePct) item->AlterToMagicItem();
        }
        const BYTE socketCount = table->bGainSocketPct;
        if (socketCount != 0)
            item->AlterToSocketItem(socketCount);
        if(vnum==50300 || vnum==70037) {
            DWORD skill;
            do { skill=number_ex(1,120,"item_manager.cpp",375); }
            while(!CSkillManager::instance().Get(skill));
            item->SetSocket(0,skill);
        }
    }
    return item;
}

// 08192c20
void ITEM_MANAGER::RemoveItem(LPITEM item, const char* reason)
{
    CHARACTER* owner = item->GetOwner();
    if (owner) {
        char hint[64];
        const char* name = item->GetName();
        const auto count = static_cast<unsigned int>(item->GetCount());
        snprintf(hint, sizeof(hint), "%s %lu ", name, static_cast<unsigned long>(count));
        const auto y = owner->GetY();
        auto& logs = LogManager::instance();
        const auto id = item->GetID();
        const auto x = owner->GetX();
        const auto pid = owner->GetPlayerID();
        logs.ItemLog(pid, x, y, id, reason ? reason : "REMOVE", hint);
        const BYTE window = item->GetWindow();
        if (window == 3 || window == 4) {
            CSafebox* box = window == 4 ? owner->GetMall() : owner->GetSafebox();
            if (box) box->Remove(item->GetCell());
        } else {
            owner->SyncQuickslot(1, item->GetCell(), 255);
            item->RemoveFromCharacter();
        }
    }
    DestroyItem(item);
}

bool ITEM_MANAGER::Initialize(TItemTable* table, int size)
{
    // Save vnums before replacing the vector that owns live prototype pointers.
    std::vector<std::pair<CItem*, DWORD> > liveItems;
    for (std::map<DWORD, CItem*>::iterator it = m_VIDMap.begin(); it != m_VIDMap.end(); ++it)
        liveItems.push_back(std::make_pair(it->second, it->second->GetVnum()));

    m_vec_prototype.clear();
    m_vec_prototype.resize(size);
    if (size)
        thecore_memcpy(m_vec_prototype.data(), table, sizeof(TItemTable) * size);

    m_map_ItemRefineFrom.clear();
    for (int i = 0; i < size; ++i) {
        const TItemTable& row = m_vec_prototype[i];
        const DWORD vnum = row.dwVnum;
        if (row.dwRefinedVnum)
            m_map_ItemRefineFrom.insert(std::make_pair(static_cast<DWORD>(row.dwRefinedVnum), vnum));
        if (row.bType == 18 || (row.dwFlags & 0xc00))
            quest::CQuestManager::instance().RegisterNPCVnum(vnum);
        const TItemTable& registeredRow = m_vec_prototype[i];
        const DWORD registeredVnum = registeredRow.dwVnum;
        m_map_vid.insert(std::make_pair(registeredVnum, registeredRow));
        if (test_server)
            sys_log(0, "ITEM_INFO %d %s ", static_cast<int>(registeredVnum), registeredRow.szName);
    }

    char line[512] = {};
    int length = 0;
    for (int i = 0; i < size; ++i) {
        const TItemTable& row = m_vec_prototype[i];
        length += std::sprintf(line + length, "%5ld %-16s", static_cast<long>(auto_give_detail::signed32(row.dwVnum)), row.szLocaleName);
        if ((i + 1) % 4 == 0) {
            if (!test_server)
                sys_log(0, "%s", line);
            length = 0;
        } else {
            line[length++] = '\t';
            line[length] = '\0';
        }
    }
    if ((size + 1) % 4 && !test_server)
        sys_log(0, "%s", line);

    sys_log(0, "ITEM_VID_MAP %d", static_cast<int>(m_VIDMap.size()));
    for (const auto& entry : liveItems) {
        TItemTable* proto = GetTable(entry.second);
        if (!proto) {
            _sys_err("Initialize", 0xbb, "cannot reset item table");
            entry.first->SetProto(0);
        }
        entry.first->SetProto(proto);
    }
    return true;
}

bool ITEM_MANAGER::GetVnum(const char* name, DWORD& vnum)
{
    const std::size_t length = std::strlen(name);
    for (const TItemTable& row : m_vec_prototype)
    {
        if (strncasecmp(name, row.szLocaleName, length) == 0)
        {
            vnum = row.dwVnum;
            return true;
        }
    }
    return false;
}

bool ITEM_MANAGER::GetVnumByOriginalName(const char* name, DWORD& vnum)
{
    const std::size_t length = std::strlen(name);
    for (const TItemTable& row : m_vec_prototype) {
        if (strncasecmp(name, row.szName, length) == 0) {
            vnum = row.dwVnum;
            return true;
        }
    }
    return false;
}

int CSpecialItemGroup::GetOneIndex() const
{
    if (IsEmpty())
        return 0;
    const int roll = number_ex(1, m_vecProbs.back(), const_cast<char*>("item_manager.h"), 0x33);
    const auto selected = std::lower_bound(m_vecProbs.begin(), m_vecProbs.end(), roll);
    return static_cast<int>(selected - m_vecProbs.begin());
}

const CSpecialItemGroup* ITEM_MANAGER::GetSpecialItemGroup(DWORD vnum)
{
    std::map<DWORD, CSpecialItemGroup*>::iterator it = m_map_pkSpecialItemGroup.find(static_cast<unsigned int>(vnum));
    return it != m_map_pkSpecialItemGroup.end() ? it->second : 0;
}

int CMobItemGroup::GetOneIndex() const
{
    if (IsEmpty())
        return 0;
    const int roll = number_ex(1, m_vecProbs.back(), const_cast<char*>("item_manager.h"), 0x84);
    const auto selected = std::lower_bound(m_vecProbs.begin(), m_vecProbs.end(), roll);
    return static_cast<int>(selected - m_vecProbs.begin());
}

const CMobItemGroup::SMobItemGroupInfo* CMobItemGroup::SelectItem() const
{
    const auto index = static_cast<std::vector<int>::size_type>(GetOneIndex());
    return index < m_vecProbs.size() && index < m_vecItems.size() ? &m_vecItems[index] : nullptr;
}

std::set<DWORD> g_set_lotto;

void load_lotto()
{
    static bool loaded = false;
    if (loaded)
        return;
    loaded = true;

    std::FILE* file = std::fopen("lotto.txt", "r");
    if (!file)
        return;

    char line[256];
    while (std::fgets(line, sizeof(line), file))
        g_set_lotto.insert(static_cast<unsigned int>(std::atoi(line)));
    std::fclose(file);
}

DWORD lotto()
{
    load_lotto();
    char source[] = "item_manager.cpp";
    unsigned int value;
    do
    {
        value = 0;
        for (int digit = 0; digit < 6; ++digit)
            value = value * 10 + number_ex(1, 9, source, 0x294);
    } while (g_set_lotto.find(value) != g_set_lotto.end());

    if (std::FILE* file = std::fopen("lotto.txt", "a+"))
    {
        std::fprintf(file, "%u\n", value);
        std::fclose(file);
    }
    return value;
}

namespace {
int DropProbabilityProduct(int left, int right)
{
    const auto bits = static_cast<unsigned>(left) * static_cast<unsigned>(right);
    int value;
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}
}

int GetDropPerKillPct(int minimum, int fallback, int deltaPercent, const char* flag)
{
    int value = quest::CQuestManager::instance().GetEventFlag(flag);
    if (!value)
        return 0;
    if (!test_server && (value < minimum || value < 0))
        value = fallback;
    return value ? DropProbabilityProduct(deltaPercent, 40000) / value : 0;
}

bool ITEM_MANAGER::CreateDropItem(CHARACTER* mob, CHARACTER* killer, std::vector<CItem*>& drops)
{
    char source[] = "item_manager.cpp";
    const int level = killer->GetLevel();
    const bool stone = mob->IsStone();
    const bool bossRates = !stone && mob->GetMobRank() > 3;
    const int mobLevel = mob->GetLevel();
    const int killerLevel = killer->GetLevel();
    const int index = MINMAX(0, mobLevel - killerLevel + 15, 30);
    const int* rates = bossRates ? aiPercentByDeltaLevForBoss : aiPercentByDeltaLev;
    int delta = rates[index];
    const BYTE rank = mob->GetMobRank();
    if (number_ex(1, 50000, source, 0x2e7) == 1)
        delta = static_cast<int>(static_cast<unsigned>(delta) + 1000u);
    else if (number_ex(1, 10000, source, 0x2e9) == 1)
        delta = static_cast<int>(static_cast<unsigned>(delta) + 500u);
    sys_log(3, "CreateDropItem for level: %d rank: %u pct: %d", level, static_cast<unsigned>(rank), delta);

    delta = DropProbabilityProduct(delta, CHARACTER_MANAGER::instance().GetMobItemRate(killer)) / 100;
    if (killer->GetPremiumRemainSeconds(1) > 0 || killer->IsEquipUniqueGroup(10002))
        delta = DropProbabilityProduct(delta, 2);
    if (killer->GetPoint(126) > 0 && killer->IsPCBang()) {
        const int cafeBonus = killer->GetPoint(126);
        const int bonus = DropProbabilityProduct(delta, cafeBonus) / 100;
        delta = static_cast<int>(static_cast<unsigned>(delta) + static_cast<unsigned>(bonus));
    }
    const int privilege = CPrivManager::instance().GetPriv(killer, 1);
    const bool unique = killer->IsEquipUniqueItem(0x1119b);
    if (privilege + unique == -100)
        return false;
    const int range = distribution_test_server ? 4000000 / 3 : 4000000;

    const auto create = [&](DWORD vnum, DWORD count, int rare = -1) {
        if (CItem* item = CreateItem(vnum, count, 0, true, rare))
            drops.push_back(item);
    };
    const auto scaled = [delta](DWORD probability) {
        return static_cast<int>(static_cast<unsigned int>(probability) * static_cast<unsigned int>(delta) / 100);
    };

    const auto& commonDrops = g_vec_pkCommonDropItem[rank];
    for (std::size_t index = 0; index < commonDrops.size(); ++index) {
        const CItemDropInfo& row = commonDrops[index];
        if (level < row.m_iLevelStart || level > row.m_iLevelEnd)
            continue;
        const int probability = DropProbabilityProduct(delta, row.m_iPercent) / 100;
        sys_log(3, "CreateDropItem %d ~ %d %d(%d)", row.m_iLevelStart, row.m_iLevelEnd,
                static_cast<int>(row.m_dwVnum), probability);
        if (number_ex(1, range, source, 0x316) > probability)
            continue;
        const TItemTable* table = GetTable(row.m_dwVnum);
        if (!table)
            continue;
        if (table->bType == 19) {
            if (row.m_dwVnum == mob->GetPolymorphItemVnum()) {
                if (CItem* item = CreateItem(row.m_dwVnum, 1, 0, true, -1)) {
                    item->SetSocket(0, mob->GetRaceNum());
                    drops.push_back(item);
                }
            }
        } else {
            create(row.m_dwVnum, 1);
        }
    }

    std::map<DWORD, CDropItemGroup*>::iterator drop = m_map_pkDropItemGroup.find(mob->GetRaceNum());
    if (drop != m_map_pkDropItemGroup.end()) {
        const auto rows = drop->second->GetVector();
        for (const auto& row : rows) {
            const int probability = scaled(row.dwPct);
            if (number_ex(1, range, source, 0x33d) <= probability) {
                const DWORD vnum = row.dwVnum;
                const int count = row.iCount;
                create(vnum, count);
            }
        }
    }
    std::map<DWORD, CMobItemGroup*>::iterator kill = m_map_pkMobItemGroup.find(mob->GetRaceNum());
    if (kill != m_map_pkMobItemGroup.end() && kill->second) {
        CMobItemGroup* group = kill->second;
        if (!group->IsEmpty()) {
            const int probability = DropProbabilityProduct(delta, 40000) / group->GetKillPerDrop();
            if (number_ex(1, range, source, 0x355) <= probability) {
                if (const CMobItemGroup::SMobItemGroupInfo* row = group->SelectItem()) {
                    const int rarePercent = row->iRarePct;
                    const int count = row->iCount;
                    const DWORD vnum = row->dwItemVnum;
                    create(vnum, count, rarePercent);
                }
            }
        }
    }
    std::map<DWORD, CLevelItemGroup*>::iterator limit = m_map_pkLevelItemGroup.find(mob->GetRaceNum());
    if (limit != m_map_pkLevelItemGroup.end() && limit->second->GetLevelLimit() <= static_cast<DWORD>(level)) {
        const auto rows = limit->second->GetVector();
        for (const auto& row : rows) {
            if (static_cast<DWORD>(number_ex(1, 1000000, source, 0x36e)) <= row.dwPct) {
                const DWORD vnum = row.dwVNum;
                const int count = row.iCount;
                create(vnum, count);
            }
        }
    }
    if (LC_IsJapan() || killer->GetPremiumRemainSeconds(1) > 0 || killer->IsEquipUniqueGroup(10002)) {
        std::map<DWORD, CBuyerThiefGlovesItemGroup*>::iterator glove = m_map_pkGloveItemGroup.find(mob->GetRaceNum());
        if (glove != m_map_pkGloveItemGroup.end()) {
            const auto rows = glove->second->GetVector();
            for (const auto& row : rows) {
                const int probability = scaled(row.dwPct);
                if (number_ex(1, range, source, 0x38a) <= probability) {
                    const DWORD vnum = row.dwVnum;
                    const int count = row.iCount;
                    create(vnum, count);
                }
            }
        }
    }

    const DWORD etcVnum = mob->GetMobDropItemVnum();
    if (etcVnum) {
        std::map<DWORD, DWORD>::iterator etc = m_map_dwEtcItemDropProb.find(mob->GetMobDropItemVnum());
        if (etc != m_map_dwEtcItemDropProb.end()) {
            const int probability = scaled(etc->second);
            if (number_ex(1, range, source, 0x39e) <= probability)
                create(mob->GetMobDropItemVnum(), 1);
        }
    }
    if (mob->IsStone() && mob->GetDropMetinStoneVnum()) {
        const int scaledChance = DropProbabilityProduct(mob->GetDropMetinStonePct(), delta);
        const int roll = number_ex(1, range, source, 0x3ac);
        if (roll <= DropProbabilityProduct(scaledChance, 400))
            create(mob->GetDropMetinStoneVnum(), 1);
    }

    if (killer->IsHorseRiding()) {
        const int probability = GetDropPerKillPct(1000, 1000000, delta, "horse_skill_book_drop");
        if (number_ex(1, range, source, 0x3b5) <= probability) {
            sys_log(0, "EVENT HORSE_SKILL_BOOK_DROP");
            create(50060, 1);
        }
    }
    const int lotto = GetDropPerKillPct(100, 1000, delta, "lotto_drop");
    if (number_ex(1, range, source, 0x3be) <= lotto) {
        DWORD* data = new DWORD[3];
        data[0] = 50001;
        data[1] = 1;
        data[2] = quest::CQuestManager::instance().GetEventFlag("lotto_round");
        const DWORD pid = killer->GetPlayerID();
        DBManager::instance().ReturnQuery(3, pid, data,
            "INSERT INTO lotto_list VALUES(0, 'server%s', %lu, NOW())", get_table_postfix(), pid);
    }

    CreateQuestDropItem(mob, killer, drops, delta, range);
    for (CItem* item : drops)
        DBManager::instance().SendMoneyLog(8, item->GetVnum(), item->GetCount());
    return !drops.empty();
}

namespace {
int __DropEvent_CharStone_GetDropPercent(int level)
{
    const int bracket = (level - 1) / 10;
    return bracket == 0 ? gs_dropEvent_charStone.percent_lv01_10
        : bracket > 0 && bracket < 3 ? gs_dropEvent_charStone.percent_lv11_30
        : gs_dropEvent_charStone.percent_lv31_MX;
}

void __DropEvent_CharStone_DropItem(CHARACTER* killer, CHARACTER* victim, ITEM_MANAGER& items, std::vector<CItem*>& drops)
{
    if (!gs_dropEvent_charStone.alive)
        return;

    const int level = killer->GetLevel();
    const int percent = __DropEvent_CharStone_GetDropPercent(level);
    char source[] = "item_manager.cpp";
    if (number_ex(1, 10000, source, 0x41b) > percent)
        return;

    const int logLevel = test_server || killer->GetGMLevel() ? 0 : 1;
    const int victimLevel = victim->GetLevel();
    const int difference = victimLevel - level;
    if (difference >= gs_dropEvent_charStone.level_range || difference <= -gs_dropEvent_charStone.level_range) {
        const char* victimName = victim->GetName();
        const int killerLevel = killer->GetLevel();
        const char* killerName = killer->GetName();
        sys_log(logLevel,
                "dropevent.drop_char_stone.level_range_over: killer(%s: lv%d), victim(%s: lv:%d), level_diff(%d)",
                killerName, killerLevel, victimName, victimLevel, difference);
        return;
    }

    const DWORD vnum = number_ex(0x7602, 0x760b, source, 0x42b);
    if (CItem* item = items.CreateItem(vnum, 1, 0, true, -1)) {
        drops.push_back(item);
        const char* itemName = item->GetName();
        const int victimLevel = victim->GetLevel();
        const char* victimName = victim->GetName();
        const int killerLevel = killer->GetLevel();
        const char* killerName = killer->GetName();
        sys_log(logLevel,
                "dropevent.drop_char_stone.item_drop: killer(%s: lv%d), victim(%s: lv:%d), item_name(%s)",
                killerName, killerLevel, victimName, victimLevel, itemName);
    }
}
}

int GetThreeSkillLevelAdjust(int level)
{
    if (level < 40) return 32;
    if (level < 45) return 16;
    if (level < 50) return 8;
    if (level < 55) return 4;
    return level < 60 ? 2 : 1;
}

void ITEM_MANAGER::CreateQuestDropItem(CHARACTER* mob, CHARACTER* killer, std::vector<CItem*>& drops,
                                       int delta, int range)
{
    if (!mob || !killer)
        return;

    const char* killerName = killer->GetName();
    const char* victimName = mob->GetName();
    sys_log(0, "CreateQuestDropItem victim(%s), killer(%s)", victimName, killerName);
    __DropEvent_CharStone_DropItem(killer, mob, *this, drops);

    quest::CQuestManager& quests = quest::CQuestManager::instance();
    char source[] = "item_manager.cpp";
    const auto roll = [&](int line) { return number_ex(1, range, source, line); };
    const auto create = [&](DWORD vnum) {
        if (CItem* item = CreateItem(vnum, 1, 0, true, -1))
            drops.push_back(item);
    };
    const auto flagRoll = [&](int minimum, int fallback, const char* flag, int line) {
        const int probability = GetDropPerKillPct(minimum, fallback, delta, flag);
        return roll(line) <= probability;
    };

    const int seasonalDivisor[] = { 2000, 1000, 300, 50, 0, 0 };
    if (quests.GetEventFlag("xmas_sock") && seasonalDivisor[mob->GetMobRank()]) {
        int probability = DropProbabilityProduct(delta, 40000) / seasonalDivisor[mob->GetMobRank()];
        if (LC_IsHongKong())
            probability *= 10;
        sys_log(0, "SOCK DROP %d %d", probability, range);
        if (roll(0x48f) <= probability)
            create(0xc35a);
    }
    if (quests.GetEventFlag("drop_moon") && seasonalDivisor[mob->GetMobRank()]) {
        const int probability = DropProbabilityProduct(delta, 40000) / seasonalDivisor[mob->GetMobRank()];
        if (roll(0x4aa) <= probability)
            create(0xc35b);
    }

    const char* anniversaryLog = "\300\260\260\242\272\270\307\325 DROP EVENT ";
    if (flagRoll(100, g_iUseLocale ? 2000 : 800, "2006_drop", 0x4b2)) {
        sys_log(0, "%s", anniversaryLog);
        create(0xc375);
    }
    if (flagRoll(100, g_iUseLocale ? 2000 : 800, "2007_drop", 0x4be)) {
        sys_log(0, "%s", anniversaryLog);
        create(0xc37b);
    }
    if (flagRoll(100, 1000, "newyear_fire", 0x4e6))
        create(0xc3bb + (g_iUseLocale == 0));
    if (flagRoll(100, 500, "newyear_moon", 0x4f0)) {
        const DWORD vnums[] = { 50016, 50017, 50018, 50019, 50019, 50019 };
        sys_log(0, "EVENT NEWYEAR_MOON DROP");
        create(vnums[number_ex(0, 5, source, 0x4f5)]);
    }
    if (flagRoll(100, g_iUseLocale ? 2000 : 800, "valentine_drop", 0x4fc)) {
        sys_log(0, "EVENT VALENTINE_DROP");
        create(50024 + number_ex(0, 1, source, 0x501));
    }
    if (flagRoll(100, g_iUseLocale ? 2000 : 800, "whiteday_drop", 0x508)) {
        sys_log(0, "EVENT WHITEDAY_DROP");
        create(50031 + number_ex(0, 1, source, 0x50c));
    }

    const int killerLevel = killer->GetLevel();
    const bool highLevel = killerLevel >= 50;
    if (flagRoll(100, 1000, highLevel ? "kids_day_drop_high" : "kids_day_drop", highLevel ? 0x515 : 0x51f))
        create(0xc372);
    if (mob->GetLevel() >= 40 && mob->GetMobRank() > 3) {
        const int probability = GetDropPerKillPct(1, 1000, delta, "three_skill_item");
        const int adjustedProbability = probability / GetThreeSkillLevelAdjust(mob->GetLevel());
        if (roll(0x52a) <= adjustedProbability)
            create(0xc551);
    }
    if (flagRoll(100, 1000, "dragon_boat_festival_drop", 0x536))
        create(0xc3a5);

    const int marsMultiplier[] = { 50, 30, 5, 1, 0, 0 };
    if (killer->GetLevel() > 14 && quests.GetEventFlag("mars_drop") && marsMultiplier[mob->GetMobRank()]) {
        const int probability = GetDropPerKillPct(1000, 1500, delta, "mars_drop");
        const int draw = roll(0x54d);
        const int adjustedDraw = DropProbabilityProduct(draw, marsMultiplier[mob->GetMobRank()]);
        if (adjustedDraw <= probability)
            create(0x11193);
    }
}
