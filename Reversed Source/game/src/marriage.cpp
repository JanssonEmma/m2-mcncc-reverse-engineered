#include "char.h"
#include "marriage.h"
#include "wedding.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include <cstdint>
#include "config.h"
#include "questmanager.h"
#include "utils.h"
#include "../../libthecore/include/utils.h"
#include "desc_client.h"
#include <cstring>
#include "../../common/tables.h"
#include "../../libthecore/include/log.h"

namespace marriage {
void Align(DWORD& first, DWORD& second)
{
    if (second<first) {
        DWORD old=first;
        first=second;
        second=old;
    }
}

void SendLoverInfo(CHARACTER* character, const std::string& name, int lovePoint)
{
    struct LoverInfo {
        BYTE header;
        char name[25];
        BYTE lovePoint;
    } packet = {0x83, {}, static_cast<BYTE>(lovePoint)};
    static_assert(sizeof(packet) == 27, "lover info wire packet");
    std::strncpy(packet.name, name.c_str(), sizeof(packet.name) - 1);
    if (DESC* desc = character->GetDesc())
        desc->Packet(&packet, sizeof(packet));
}

namespace {
struct NearCheckInfo {
    TMarriage* marriage;
};
}

EVENTFUNC(near_check_event)
{
    static_cast<NearCheckInfo*>(event->info)->marriage->NearCheck();
    return passes_per_sec * 5;
}

void TMarriage::Login(CHARACTER* character)
{
    const DWORD pid = character->GetPlayerID();
    if (pid == pid1) {
        ch1 = character;
        if (is_married)
            SendLoverInfo(ch1, name2, GetMarriagePoint());
    } else if (pid == pid2) {
        ch2 = character;
        if (is_married)
            SendLoverInfo(ch2, name1, GetMarriagePoint());
    }

    if (!ch1 || !ch2)
        return;

    ch1->SetMarryPartner(ch2);
    ch2->SetMarryPartner(ch1);
    if (is_married) {
        sys_log(0, "lover_login %lu %lu", pid1, pid2);
        ch1->ChatPacket(5, "lover_login");
        ch2->ChatPacket(5, "lover_login");
    }
    StartNearCheckEvent();
}

void TMarriage::StartNearCheckEvent()
{
    StopNearCheckEvent();
    NearCheckInfo* info;
    const DWORD id = __AllocEventInfo(sizeof(NearCheckInfo), reinterpret_cast<void**>(&info));
    info->marriage = this;
    eventNearCheck = event_create_ex(near_check_event, id, 1, const_cast<char*>("marriage.cpp"), 0x138);
}

void TMarriage::NearCheck()
{
    if (!is_married)
        return;
    if (!ch1 || !ch2) {
        StopNearCheckEvent();
        return;
    }

    const int lovePoint = GetMarriagePoint();
    const bool near = IsNear();
    sys_log(0, "NearCheck %lu %lu %d %d %d", pid1, pid2, near, isLastNear, byLastLovePoint, lovePoint);
    if (near != isLastNear) {
        isLastNear = near;
        const char* command = near ? "lover_near" : "lover_far";
        ch1->ChatPacket(5, "%s", command);
        ch2->ChatPacket(5, "%s", command);
    }
    if (byLastLovePoint != lovePoint) {
        byLastLovePoint = static_cast<BYTE>(lovePoint);
        const BYTE packet[] = {0x84, byLastLovePoint};
        if (DESC* desc = ch1->GetDesc())
            desc->Packet(packet, sizeof(packet));
        if (DESC* desc = ch2->GetDesc())
            desc->Packet(packet, sizeof(packet));
    }
}

void CManager::Login(CHARACTER* character)
{
    if (TMarriage* marriage = Get(character->GetPlayerID()))
        marriage->Login(character);
}

TMarriage* CManager::Get(DWORD pid)
{
    std::map<DWORD,TMarriage*>::iterator it=m_MarriageByPID.find(pid);
    return it==m_MarriageByPID.end() ? 0 : it->second;
}

void CManager::RequestUpdate(DWORD first, DWORD second, int points, BYTE married)
{
    Align(first,second);
    TPacketMarriageUpdate packet;
    packet.byMarried=married;
    packet.dwPID1=first;
    packet.dwPID2=second;
    packet.lLovePoint=points;
    db_clientdesc->DBPacket(71,0,&packet,sizeof(packet));
}

void TMarriage::Save()
{
    sys_log(0,"TMarriage::Save() - RequestUpdate.bSave=%d",bSave);
    if (bSave) {
        CManager::instance().RequestUpdate(pid1,pid2,love_point,is_married);
        bSave=false;
    }
}
}

namespace marriage {
void TMarriage::StopNearCheckEvent()
{
    byLastLovePoint=0;
    isLastNear=false;
    event_cancel(&eventNearCheck);
}

void TMarriage::Logout(DWORD pid)
{
    if (pid==pid1) ch1=0;
    else if (pid==pid2) ch2=0;
    if (!ch1 && !ch2) return;

    Save();
    if (ch1) {
        ch1->SetMarryPartner(0);
        if (is_married) ch1->ChatPacket(5,"lover_logout");
    }
    if (ch2) {
        ch2->SetMarryPartner(0);
        if (is_married) ch2->ChatPacket(5,"lover_logout");
    }
    StopNearCheckEvent();
}

void CManager::Logout(CHARACTER* character)
{
    DWORD pid=character->GetPlayerID();
    TMarriage* record=Get(pid);
    if (record) record->Logout(pid);
}
}

namespace marriage {
// 0823ebc0: item-specific values for the four marriage grades.
static const struct {
    DWORD vnum;
    int values[4];
} g_ItemBonus[] = {
    {71069, {4, 5, 6, 8}},
    {71070, {10, 12, 15, 20}},
    {71071, {4, 5, 6, 8}},
    {71072, {-4, -5, -6, -8}},
    {71073, {20, 25, 30, 40}},
    {71074, {12, 16, 20, 30}},
};

bool CManager::IsMarriageUniqueItem(DWORD vnum)
{
    for (const auto& bonus : g_ItemBonus) {
        if (bonus.vnum == vnum)
            return true;
    }
    return false;
}

bool CManager::IsMarried(DWORD pid)
{
    TMarriage* record = Get(pid);
    return record && record->is_married;
}

bool CManager::IsEngaged(DWORD pid)
{
    const TMarriage* record = Get(pid);
    return record && !record->is_married;
}

bool CManager::IsEngagedOrMarried(DWORD pid)
{
    return Get(pid) != nullptr;
}

bool TMarriage::IsNear()
{
    return is_married && ch1 && ch2 && ch1->GetMapIndex() == ch2->GetMapIndex();
}

int TMarriage::GetMarriagePoint()
{
    if (test_server) {
        const int overrideValue = quest::CQuestManager::instance().GetEventFlag("lovepoint");
        if (overrideValue)
            return MINMAX(0, overrideValue, 100);
    }

    const bool fast = ch1 && ch2 &&
                      (ch1->GetPremiumRemainSeconds(5) > 0 || ch2->GetPremiumRemainSeconds(5) > 0);
    const int cap = fast ? 40 : 30;
    const auto elapsed = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(get_global_time()) - static_cast<std::uint32_t>(marry_time));
    const int periods = elapsed / (test_server ? 60 : 86400);
    const int timePoints = MIN(periods * (fast ? 2 : 1), cap);
    const int lovePoints = MIN(love_point / 1000000, cap);
    return MIN(lovePoints + 50 + timePoints, 100);
}

int TMarriage::GetMarriageGrade()
{
    const int points = MINMAX(50, GetMarriagePoint(), 100);
    if (points <= 64)
        return 0;
    if (points <= 79)
        return 1;
    return points < 100 ? 2 : 3;
}

int TMarriage::GetBonus(DWORD vnum, bool share, CHARACTER* character)
{
    if (!is_married)
        return 0;
    for (const auto& bonus : g_ItemBonus) {
        if (bonus.vnum != vnum)
            continue;

        bool equipped = false;
        if (ch1 && (share || character != ch1))
            equipped = ch1->IsEquipUniqueItem(vnum);
        if (ch2 && (share || character != ch2)) {
            if (ch2->IsEquipUniqueItem(vnum))
                equipped = true;
        }
        return equipped ? bonus.values[GetMarriageGrade()] : 0;
    }
    return 0;
}

void TMarriage::Update(DWORD points)
{
    if (!ch1 || !ch2 || !points || !is_married)
        return;
    love_point = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(love_point) + static_cast<std::uint32_t>(points));
    bSave = true;
    if (test_server) {
        CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
        const double millions = static_cast<double>(love_point) / 1000000.0;
        if (CHARACTER* first = characters.FindByPID(pid1))
            first->ChatPacket(3, "lovepoint bykill %.3g total %d", millions, GetMarriagePoint());
        if (CHARACTER* second = characters.FindByPID(pid2))
            second->ChatPacket(3, "lovepoint bykill %.3g total %d", millions, GetMarriagePoint());
    }
}
}

namespace marriage {
namespace {
#pragma pack(push, 1)
struct MarriagePairPacket
{
    std::uint32_t first;
    std::uint32_t second;
};

struct MarriageAddPacket
{
    MarriagePairPacket couple;
    std::uint32_t marryTime;
    char firstName[25];
    char secondName[25];
};
#pragma pack(pop)

static_assert(sizeof(MarriagePairPacket) == 8, "marriage pair wire size");
static_assert(sizeof(MarriageAddPacket) == 62, "marriage add wire size");
}

void CManager::RequestAdd(DWORD first, DWORD second, const char* firstName, const char* secondName)
{
    if (second < first) {
        std::swap(first, second);
        std::swap(firstName, secondName);
    }

    MarriageAddPacket packet = {};
    packet.couple.first = static_cast<std::uint32_t>(first);
    packet.couple.second = static_cast<std::uint32_t>(second);
    std::strncpy(packet.firstName, firstName, sizeof(packet.firstName) - 1);
    std::strncpy(packet.secondName, secondName, sizeof(packet.secondName) - 1);
    db_clientdesc->DBPacket(70, 0, &packet, sizeof(packet));
}

void CManager::RequestRemove(DWORD first, DWORD second)
{
    Align(first, second);
    const MarriagePairPacket packet = {
        static_cast<std::uint32_t>(first), static_cast<std::uint32_t>(second),
    };
    db_clientdesc->DBPacket(72, 0, &packet, sizeof(packet));
}

void CManager::RequestEndWedding(DWORD first, DWORD second)
{
    const MarriagePairPacket packet = {
        static_cast<std::uint32_t>(first), static_cast<std::uint32_t>(second),
    };
    db_clientdesc->DBPacket(75, 0, &packet, sizeof(packet));
}

void TMarriage::SetMarried()
{
    is_married = true;
    bSave = true;
    Save();
    if (ch1 && ch2) {
        SendLoverInfo(ch1, name2, GetMarriagePoint());
        SendLoverInfo(ch2, name1, GetMarriagePoint());
        ch1->ChatPacket(5, "lover_login");
        ch2->ChatPacket(5, "lover_login");
    }
}

void TMarriage::WarpToWeddingMap(DWORD pid)
{
    if (!pWeddingInfo)
        return;
    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid);
    if (!character)
        return;

    const DWORD map = pWeddingInfo->dwMapIndex;
    PIXEL_POSITION position;
    if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire(map / 10000, 0, position)) {
        _sys_err("WarpToWeddingMap", 0x181, "cannot get warp position");
        return;
    }
    character->SaveExitLocation();
    character->WarpSet(position.x, position.y, map);
}

void TMarriage::RequestEndWedding()
{
    if (pWeddingInfo)
        CManager::instance().RequestEndWedding(pid1, pid2);
}
}

namespace marriage {
CManager::CManager() = default;
CManager::~CManager() = default;

// 080b12e0
bool CManager::Initialize()
{
    return true;
}

// 080b12f0
void CManager::Destroy() {}

TMarriage::~TMarriage()
{
    StopNearCheckEvent();
    if (ch1 && ch2) {
        ch1->ChatPacket(5, "lover_divorce");
        ch2->ChatPacket(5, "lover_divorce");
    }
    if (ch1)
        ch1->SetMarryPartner(nullptr);
    if (ch2)
        ch2->SetMarryPartner(nullptr);
    delete pWeddingInfo;
}

void CManager::Add(DWORD first, DWORD second, time_t marriedAt, const char* firstName, const char* secondName)
{
    if (first == second || IsEngagedOrMarried(first) || IsEngagedOrMarried(second)) {
        _sys_err("Add", 0x1e5, "cannot marry already married character. %d - %d",
                 static_cast<int>(first), static_cast<int>(second));
        return;
    }
    if (second < first) {
        std::swap(first, second);
        std::swap(firstName, secondName);
    }

    auto* record = new TMarriage{};
    record->pid1 = first;
    record->pid2 = second;
    record->marry_time = marriedAt;
    record->name1 = firstName;
    record->name2 = secondName;
    m_Marriages.insert(record);
    m_MarriageByPID.emplace(first, record);
    m_MarriageByPID.emplace(second, record);

    auto& characters = CHARACTER_MANAGER::instance();
    if (characters.FindByPID(first) && characters.FindByPID(second)) {
        const MarriagePairPacket packet = {static_cast<std::uint32_t>(first), static_cast<std::uint32_t>(second)};
        db_clientdesc->DBPacket(73, 0, &packet, sizeof(packet));
    }
}

void CManager::Update(DWORD first, DWORD second, long totalPoints, BYTE married)
{
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("Update", 0x214, "not under marriage : %lu %lu", first, second);
        return;
    }
    record->love_point = static_cast<int>(totalPoints);
    record->is_married = married != 0;
}

void CManager::Remove(DWORD first, DWORD second)
{
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("Remove", 0x22b, "not under marriage : %lu %lu", first, second);
        return;
    }
    m_Marriages.erase(record);
    m_MarriageByPID.erase(first);
    m_MarriageByPID.erase(second);
    m_setWedding.erase({first, second});
    m_setWedding.erase({second, first});
    delete record;
}

void CManager::WeddingReady(DWORD first, DWORD second, DWORD map)
{
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("WeddingReady", 0x251, "wrong marriage %lu, %lu", first, second);
        return;
    }
    if (!record->pWeddingInfo)
        record->pWeddingInfo = new TWeddingInfo;
    record->pWeddingInfo->dwMapIndex = map;
}

void CManager::WeddingStart(DWORD first, DWORD second)
{
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("WeddingStart", 0x266, "wrong marriage %lu, %lu", first, second);
        return;
    }
    if (!record->pWeddingInfo)
        return;
    record->WarpToWeddingMap(first);
    record->WarpToWeddingMap(second);
    m_setWedding.emplace(first, second);
}

void CManager::WeddingEnd(DWORD first, DWORD second)
{
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("WeddingEnd", 0x27c, "wrong marriage %lu, %lu", first, second);
        return;
    }
    if (!record->pWeddingInfo) {
        _sys_err("WeddingEnd", 0x282, "not under wedding %lu, %lu", first, second);
        return;
    }
    const DWORD map = record->pWeddingInfo->dwMapIndex;
    if (map_allow_find(81) && !WeddingManager::instance().End(map)) {
        _sys_err("WeddingEnd", 0x28a, "wedding map error: map_index=%d", static_cast<int>(map));
        return;
    }
    delete record->pWeddingInfo;
    record->pWeddingInfo = nullptr;
    m_setWedding.erase({first, second});
}
}
