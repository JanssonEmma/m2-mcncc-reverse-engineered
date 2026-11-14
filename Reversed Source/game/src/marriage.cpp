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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
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
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * 5u);
}

void TMarriage::Login(CHARACTER* character)
{
    const auto pid = static_cast<std::uint32_t>(character->GetPlayerID());
    if (pid == pid1) {
        const bool married = is_married;
        ch1 = character;
        if (married) {
            const int points = GetMarriagePoint();
            SendLoverInfo(ch1, name2, points);
        }
    } else if (pid == pid2) {
        const bool married = is_married;
        ch2 = character;
        if (married) {
            const int points = GetMarriagePoint();
            SendLoverInfo(ch2, name1, points);
        }
    }

    if (!IsOnline())
        return;

    ch1->SetMarryPartner(ch2);
    ch2->SetMarryPartner(ch1);
    if (is_married) {
        sys_log(0, "lover_login %lu %lu", static_cast<unsigned long>(pid1), static_cast<unsigned long>(pid2));
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
    if (!IsOnline()) {
        StopNearCheckEvent();
        return;
    }

    const int lovePoint = GetMarriagePoint();
    const bool near = IsNear();
    sys_log(0, "NearCheck %lu %lu %d %d %d", static_cast<unsigned long>(pid1), static_cast<unsigned long>(pid2), near, isLastNear, byLastLovePoint, lovePoint);
    if (IsNear() && !isLastNear) {
        isLastNear = true;
        ch1->ChatPacket(5, "lover_near");
        ch2->ChatPacket(5, "lover_near");
    } else if (!IsNear() && isLastNear) {
        isLastNear = false;
        ch1->ChatPacket(5, "lover_far");
        ch2->ChatPacket(5, "lover_far");
    }
    const BYTE previousLovePoint = byLastLovePoint;
    if (previousLovePoint != GetMarriagePoint()) {
        byLastLovePoint = static_cast<BYTE>(GetMarriagePoint());
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
    const auto it = m_MarriageByPID.find(static_cast<std::uint32_t>(pid));
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
        auto& manager = CManager::instance();
        const BYTE married = is_married;
        const int points = love_point;
        const auto second = pid2;
        const auto first = pid1;
        manager.RequestUpdate(first, second, points, married);
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
    if (static_cast<std::uint32_t>(pid)==pid1) ch1=0;
    else if (static_cast<std::uint32_t>(pid)==pid2) ch2=0;
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
    std::uint32_t vnum;
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
        if (bonus.vnum == static_cast<std::uint32_t>(vnum))
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
    if (!is_married || !IsOnline())
        return false;

    const auto secondMap = static_cast<std::int32_t>(ch2->GetMapIndex());
    return static_cast<std::int32_t>(ch1->GetMapIndex()) == secondMap;
}

int TMarriage::GetMarriagePoint()
{
    if (test_server) {
        const int overrideValue = quest::CQuestManager::instance().GetEventFlag("lovepoint");
        if (overrideValue)
            return MINMAX(0, overrideValue, 100);
    }

    const bool fast = IsOnline() &&
                      (ch1->GetPremiumRemainSeconds(5) > 0 || ch2->GetPremiumRemainSeconds(5) > 0);
    const int cap = fast ? 40 : 30;
    const auto now = static_cast<std::uint32_t>(get_global_time());
    const auto elapsed = static_cast<std::int32_t>(now - static_cast<std::uint32_t>(marry_time));
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
        if (bonus.vnum != static_cast<std::uint32_t>(vnum))
            continue;

        int equipped = 0;
        if (ch1 && (share || character != ch1) && ch1->IsEquipUniqueItem(bonus.vnum))
            ++equipped;
        if (ch2 && (share || character != ch2) && ch2->IsEquipUniqueItem(bonus.vnum))
            ++equipped;
        return equipped > 0 ? bonus.values[GetMarriageGrade()] : 0;
    }
    return 0;
}

void TMarriage::Update(DWORD points)
{
    const auto delta = static_cast<std::uint32_t>(points);
    if (!IsOnline() || !delta || !is_married)
        return;
    love_point = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(love_point) + delta);
    bSave = true;
    if (test_server) {
        if (CHARACTER* first = CHARACTER_MANAGER::instance().FindByPID(pid1)) {
            const int total = GetMarriagePoint();
            const double millions = static_cast<double>(love_point) / 1000000.0;
            first->ChatPacket(3, "lovepoint bykill %.3g total %d", millions, total);
        }
        if (CHARACTER* second = CHARACTER_MANAGER::instance().FindByPID(pid2)) {
            const int total = GetMarriagePoint();
            const double millions = static_cast<double>(love_point) / 1000000.0;
            second->ChatPacket(3, "lovepoint bykill %.3g total %d", millions, total);
        }
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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
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
    if (IsOnline()) {
        const int firstPoints = GetMarriagePoint();
        SendLoverInfo(ch1, name2, firstPoints);
        const int secondPoints = GetMarriagePoint();
        SendLoverInfo(ch2, name1, secondPoints);
        ch1->ChatPacket(5, "lover_login");
        ch2->ChatPacket(5, "lover_login");
    }
}

void TMarriage::WarpToWeddingMap(DWORD pid)
{
    if (!pWeddingInfo)
        return;
    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(static_cast<std::uint32_t>(pid));
    if (!character)
        return;

    const auto map = static_cast<unsigned int>(pWeddingInfo->dwMapIndex);
    PIXEL_POSITION position;
    if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire(map / 10000, 0, position)) {
        _sys_err("WarpToWeddingMap", 0x181, "cannot get warp position");
        return;
    }
    character->SaveExitLocation();
    if (!pWeddingInfo)
        return;
    const auto destinationMap = static_cast<int>(pWeddingInfo->dwMapIndex);
    const auto y = position.y;
    const auto x = position.x;
    character->WarpSet(x, y, destinationMap);
}

void TMarriage::RequestEndWedding()
{
    if (!pWeddingInfo)
        return;

    auto& manager = CManager::instance();
    const auto second = pid2;
    const auto first = pid1;
    manager.RequestEndWedding(first, second);
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
    if (IsOnline()) {
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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("WeddingReady", 0x251, "wrong marriage %lu, %lu", first, second);
        return;
    }
    if (!record->pWeddingInfo)
        record->pWeddingInfo = new TWeddingInfo;
    record->pWeddingInfo->dwMapIndex = static_cast<std::uint32_t>(map);
}

void CManager::WeddingStart(DWORD first, DWORD second)
{
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
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
    first = static_cast<std::uint32_t>(first);
    second = static_cast<std::uint32_t>(second);
    TMarriage* record = Get(first);
    if (!record || record->GetOther(first) != second) {
        _sys_err("WeddingEnd", 0x27c, "wrong marriage %lu, %lu", first, second);
        return;
    }
    if (!record->pWeddingInfo) {
        _sys_err("WeddingEnd", 0x282, "not under wedding %lu, %lu", first, second);
        return;
    }
    if (map_allow_find(81)) {
        auto& weddings = WeddingManager::instance();
        const DWORD map = record->pWeddingInfo->dwMapIndex;
        if (!weddings.End(map)) {
            const DWORD failedMap = record->pWeddingInfo ? record->pWeddingInfo->dwMapIndex : map;
            _sys_err("WeddingEnd", 0x28a, "wedding map error: map_index=%d", static_cast<int>(failedMap));
            return;
        }
    }
    delete record->pWeddingInfo;
    record->pWeddingInfo = nullptr;
    m_setWedding.erase({first, second});
}
}
