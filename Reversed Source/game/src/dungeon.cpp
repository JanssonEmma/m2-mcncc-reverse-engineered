#include "dungeon.h"
#include "char.h"
#include "desc.h"
#include <cstdint>
#include <climits>
#include "char_manager.h"
#include "constants.h"
#include "item.h"
#include "item_manager.h"
#include "config.h"
#include "party.h"
#include "packet.h"
#include "regen.h"
#include "../../libthecore/include/utils.h"
#include "../../libthecore/include/log.h"
#include <utility>
#include <cstdio>

CDungeon::CDungeon(long originalMap, long map)
    : m_lOrigMapIndex(static_cast<std::uint32_t>(originalMap)), m_lMapIndex(static_cast<std::uint32_t>(map)),
      m_map_Area(SECTREE_MANAGER::instance().GetDungeonArea(originalMap))
{
    Initialize();
}

void CDungeon::Initialize()
{
    m_iMonsterCount = 0;
    m_bExitAllAtEliminate = false;
    m_bWarpAtEliminate = false;
    deadEvent = 0;
    m_iMobKill = 0;
    m_iStoneKill = 0;
    m_bUsePotion = false;
    m_bUseRevive = false;
}

CDungeon::~CDungeon()
{
    ClearRegen();
    event_cancel(&deadEvent);
}

void CDungeon::AddRegen(regen* record)
{
    m_regen.push_back(record);
}

void CDungeon::ClearRegen()
{
    for (std::list<regen*>::iterator it = m_regen.begin(); it != m_regen.end(); ++it) {
        event_cancel(&(*it)->event);
        CHARACTER::InvalidateRegen(*it);
        delete *it;
    }
    m_regen.clear();
}

CDungeonManager::CDungeonManager() {}
CDungeonManager::~CDungeonManager() {}

namespace {
struct DungeonDeadInfo {
    CDungeon* dungeon;
};

std::int32_t AddDungeonCoordinate(int base, int offset)
{
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(base) + static_cast<std::uint32_t>(offset));
}

struct DungeonMapInfo {
    std::int32_t map;
};

template <typename Function>
void ForEachMapNeighborPlayer(SECTREE_MAP& map, Function function)
{
    auto visit = [&](LPENTITY entity) {
        if (entity->IsType(0) && entity->GetDesc())
            function(static_cast<CHARACTER*>(entity));
    };
    for (const auto& entry : map)
        entry.second->ForEachAround(visit);
}

struct FWarpToPosition
{
    std::int32_t destination;
    std::int32_t x;
    std::int32_t y;

    void operator()(CEntity* entity)
    {
        if (!entity->IsType(0) || !entity->GetDesc())
            return;
        CHARACTER* character = static_cast<CHARACTER*>(entity);
        if (static_cast<std::int32_t>(character->GetMapIndex()) == destination) {
            character->Show(destination, x, y, 0, false);
            character->Stop();
        } else {
            character->WarpSet(x, y, destination);
        }
    }
};

void WarpMapPlayers(SECTREE_MAP& source, long destination, long tileX, long tileY)
{
    const auto x = static_cast<std::int32_t>(static_cast<std::uint32_t>(tileX) * 100U);
    const auto y = static_cast<std::int32_t>(static_cast<std::uint32_t>(tileY) * 100U);
    FWarpToPosition warp{static_cast<std::int32_t>(destination), x, y};
    for (const auto& entry : source)
        entry.second->ForEachAround(warp);
}

}

EVENTFUNC(dungeon_exit_all_event)
{
    const DungeonMapInfo* info = static_cast<DungeonMapInfo*>(event->info);
    if (CDungeon* dungeon = CDungeonManager::instance().Find(info->map))
        dungeon->ExitAll();
    return 0;
}

EVENTFUNC(dungeon_jump_to_event)
{
    const DungeonMapInfo* info = static_cast<DungeonMapInfo*>(event->info);
    if (CDungeon* dungeon = CDungeonManager::instance().Find(info->map))
        dungeon->JumpToEliminateLocation();
    else
        _sys_err("dungeon_jump_to_event", 0x3b9, "cannot find dungeon with map index %lu",
                 static_cast<unsigned long>(static_cast<std::uint32_t>(info->map)));
    return 0;
}

EVENTFUNC(dungeon_dead_event)
{
    CDungeon* dungeon = static_cast<DungeonDeadInfo*>(event->info)->dungeon;
    dungeon->deadEvent = 0;
    CDungeonManager::instance().Destroy(dungeon);
    return 0;
}

void CDungeon::IncMember(CHARACTER* character)
{
    m_set_pkCharacter.insert(character);
    event_cancel(&deadEvent);
}

void CDungeon::DecMember(CHARACTER* character)
{
    m_set_pkCharacter.erase(character);
    CheckDestroy();
}

void CDungeon::CheckDestroy()
{
    if (!m_set_pkCharacter.empty() || deadEvent)
        return;

    void* data;
    const DWORD id = __AllocEventInfo(sizeof(DungeonDeadInfo), &data);
    static_cast<DungeonDeadInfo*>(data)->dungeon = this;
    const std::int32_t delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * 10u);
    deadEvent = event_create_ex(dungeon_dead_event, id, delay,
                                const_cast<char*>("dungeon.cpp"), 0xac);
}

void CDungeon::IncPartyMember(CParty* party, CHARACTER* character)
{
    const auto entry = m_map_pkParty.find(party);
    if (entry == m_map_pkParty.end())
        m_map_pkParty.emplace(party, 1);
    else
        entry->second = static_cast<std::int32_t>(static_cast<std::uint32_t>(entry->second) + 1u);
    IncMember(character);
}

void CDungeon::DecPartyMember(CParty* party, CHARACTER* character)
{
    std::map<CParty*, int>::iterator it = m_map_pkParty.find(party);
    if (it == m_map_pkParty.end())
        _sys_err("DecPartyMember", 0xc3, "DUNGEON cannot find party");
    else {
        it->second = static_cast<std::int32_t>(static_cast<std::uint32_t>(it->second) - 1u);
        if (it->second == 0)
            QuitParty(party);
    }
    DecMember(character);
}

void CDungeon::QuitParty(CParty* party)
{
    party->SetDungeon(0);
    m_map_pkParty.erase(party);
}

void CDungeon::IncMonster()
{
    m_iMonsterCount = static_cast<std::int32_t>(static_cast<std::uint32_t>(m_iMonsterCount) + 1u);
    sys_log(0, "MonsterCount %d", m_iMonsterCount);
}

void CDungeon::DecMonster()
{
    m_iMonsterCount = static_cast<std::int32_t>(static_cast<std::uint32_t>(m_iMonsterCount) - 1u);
    CheckEliminated();
}

void CDungeon::CheckEliminated()
{
    if (m_iMonsterCount > 0)
        return;

    TEVENTFUNC callback;
    int line;
    if (m_bExitAllAtEliminate) {
        sys_log(0, "Eliminate exit event");
        m_bExitAllAtEliminate = false;
        if (!m_iWarpDelay) {
            ExitAll();
            return;
        }
        callback = dungeon_exit_all_event;
        line = 0x3d8;
    } else if (m_bWarpAtEliminate) {
        sys_log(0, "eliminate warp event");
        m_bWarpAtEliminate = false;
        if (!m_iWarpDelay) {
            JumpToEliminateLocation();
            return;
        }
        callback = dungeon_jump_to_event;
        line = 0x3eb;
    } else {
        sys_log(0, "No eliminate event");
        return;
    }

    void* data;
    const DWORD id = __AllocEventInfo(sizeof(DungeonMapInfo), &data);
    static_cast<DungeonMapInfo*>(data)->map = static_cast<std::int32_t>(m_lMapIndex);
    const std::int32_t delay = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(passes_per_sec) * static_cast<std::uint32_t>(m_iWarpDelay));
    event_create_ex(callback, id, delay, const_cast<char*>("dungeon.cpp"), line);
}

namespace {
struct FNotice
{
    const char* message;

    void operator()(LPENTITY entity)
    {
        if (entity->IsType(0))
            static_cast<CHARACTER*>(entity)->ChatPacket(2, "%s", message);
    }
};
}


void CDungeon::Notice(const char* message)
{
    sys_log(0, "XXX Dungeon Notice %p %s", this, message);
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map) {
        _sys_err("Notice", 0x36f, "Cannot get map");
        return;
    }
    FNotice notice{message};
    for (const auto& entry : *map)
        entry.second->ForEachEntity(notice);
}

void CDungeon::SetFlag(std::string name, int value)
{
    const auto flag = m_map_Flag.find(name);
    if (flag != m_map_Flag.end())
        flag->second = value;
    else
        m_map_Flag.emplace(std::move(name), value);
}

int CDungeon::GetFlag(std::string name)
{
    const auto flag = m_map_Flag.find(name);
    return flag == m_map_Flag.end() ? 0 : flag->second;
}

namespace {
struct FPurgeSectree
{
    void operator()(LPENTITY entity)
    {
        if (entity->IsType(0)) {
            if (!entity->GetDesc())
                CHARACTER_MANAGER::instance().DestroyCharacter(static_cast<CHARACTER*>(entity));
        } else if (entity->IsType(1)) {
            ITEM_MANAGER::instance().DestroyItem(static_cast<CItem*>(entity));
        } else {
            _sys_err("operator()", 0x2c9, "unknown entity type %d is in dungeon", entity->GetType());
        }
    }
};
}

void CDungeon::Purge()
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;
    FPurgeSectree purge;
    for (auto it = map->begin(); it != map->end();) {
        auto* sector = (it++)->second;
        sector->ForEachEntity(purge);
    }
}

namespace {
struct FKillSectree
{
    void operator()(LPENTITY entity)
    {
        if (entity->IsType(0) && !entity->GetDesc())
            static_cast<CHARACTER*>(entity)->Dead(nullptr, false);
    }
};
}


void CDungeon::KillAll()
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;
    FKillSectree kill;
    for (auto it = map->begin(); it != map->end();) {
        auto* sector = (it++)->second;
        sector->ForEachEntity(kill);
    }
}

struct FExitDungeonToStartPosition
{
    void operator()(CHARACTER* character) const
    {
        const BYTE empire = character->GetEmpire();
        PIXEL_POSITION position;
        if (empire < 4 && SECTREE_MANAGER::instance().GetRecallPositionByEmpire(g_start_map[empire], empire, position))
            character->WarpSet(position.x, position.y, 0);
        else
            character->ExitToSavedLocation();
    }
};

void CDungeon::ExitAllToStartPosition()
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map) {
        _sys_err("ExitAllToStartPosition", 0x39c, "Cannot get map");
        return;
    }
    ForEachMapNeighborPlayer(*map, FExitDungeonToStartPosition{});
}

void CDungeon::Spawn(DWORD vnum, const char* position)
{
    const auto area = m_map_Area.find(position);
    if (area == m_map_Area.end()) {
        _sys_err("Spawn", 0x206, "Wrong position string : %s", position);
        return;
    }
    const TAreaInfo& bounds = area->second;
    const int direction = bounds.dir == -1
        ? number_ex(0, 359, const_cast<char*>("dungeon.cpp"), 0x20d) : bounds.dir;
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;

    const int x = number_ex(bounds.sx, bounds.ex, const_cast<char*>("dungeon.cpp"), 0x210);
    const int y = number_ex(bounds.sy, bounds.ey, const_cast<char*>("dungeon.cpp"), 0x211);
    const int worldY = AddDungeonCoordinate(map->m_setting.iBaseY, y);
    const int worldX = AddDungeonCoordinate(map->m_setting.iBaseX, x);
    if (CHARACTER* character = CHARACTER_MANAGER::instance().SpawnMob(
            vnum, m_lMapIndex, worldX, worldY, 0, false, direction, true))
        character->SetDungeon(this);
}

CHARACTER* CDungeon::SpawnMob(DWORD vnum, int x, int y)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    sys_log(0, "CDungeon::SpawnMob %lu %d %d", vnum, x, y);
    if (!map)
        return nullptr;
    const auto worldY = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(map->m_setting.iBaseY) + static_cast<std::uint32_t>(y) * 100U);
    const auto worldX = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(map->m_setting.iBaseX) + static_cast<std::uint32_t>(x) * 100U);
    CHARACTER* character = CHARACTER_MANAGER::instance().SpawnMob(vnum, m_lMapIndex,
        worldX, worldY, 0, false, -1, true);
    if (character) {
        character->SetDungeon(this);
        sys_log(0, "CDungeon::SpawnMob name %s", character->GetName());
    }
    return character;
}

CHARACTER* CDungeon::SpawnGroup(DWORD vnum, long x, long y, float radius, bool aggressive, int count)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return nullptr;
    const int offset = static_cast<int>(radius);
    const std::int64_t centerX = static_cast<std::int64_t>(map->m_setting.iBaseX) + x;
    const std::int64_t centerY = static_cast<std::int64_t>(map->m_setting.iBaseY) + y;
    const std::int64_t sx = centerX - offset;
    const std::int64_t sy = centerY - offset;
    const std::int64_t ex = centerX;
    const std::int64_t ey = centerY;
    if (sx < INT_MIN || sx > INT_MAX || sy < INT_MIN || sy > INT_MAX
        || ex < INT_MIN || ex > INT_MAX || ey < INT_MIN || ey > INT_MAX)
        return nullptr;
    CHARACTER* first = nullptr;
    for (int index = 0; index < count; ++index) {
        CHARACTER* leader = CHARACTER_MANAGER::instance().SpawnGroup(
            vnum, m_lMapIndex, static_cast<int>(sx), static_cast<int>(sy),
            static_cast<int>(ex), static_cast<int>(ey), nullptr, aggressive, this);
        if (leader && !first)
            first = leader;
    }
    return first;
}

namespace {
int UniqueSpawnFailureCoordinate(int base, int start, int end)
{
    const auto sum = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(start) + static_cast<std::uint32_t>(end));
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(base) + (sum >> 1));
}
}

void CDungeon::SpawnUnique(const char* key, DWORD vnum, const char* position)
{
    const auto area = m_map_Area.find(position);
    if (area == m_map_Area.end()) {
        _sys_err("SpawnUnique", 0x186, "Wrong position string : %s", position);
        return;
    }
    const TAreaInfo& bounds = area->second;
    const int direction = bounds.dir == -1
        ? number_ex(0, 359, const_cast<char*>("dungeon.cpp"), 0x18d) : bounds.dir;
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;

    for (int attempt = 0; attempt < 100; ++attempt) {
        const int localX = number_ex(bounds.sx, bounds.ex, const_cast<char*>("dungeon.cpp"), 0x193);
        const int localY = number_ex(bounds.sy, bounds.ey, const_cast<char*>("dungeon.cpp"), 0x194);
        const int y = AddDungeonCoordinate(map->m_setting.iBaseY, localY);
        const int x = AddDungeonCoordinate(map->m_setting.iBaseX, localX);
        CHARACTER* character = CHARACTER_MANAGER::instance().SpawnMob(
            vnum, m_lMapIndex, x, y, 0, false, direction, true);
        if (character) {
            m_map_UniqueMob.emplace(key, character);
            character->SetDungeon(this);
            character->AddAffect(213, 0, 0, 8, 65535, 0, true, false);
            return;
        }
        const int centerY = UniqueSpawnFailureCoordinate(map->m_setting.iBaseY, bounds.sy, bounds.ey);
        const int centerX = UniqueSpawnFailureCoordinate(map->m_setting.iBaseX, bounds.sx, bounds.ex);
        _sys_err("SpawnUnique", 0x1a1, "Cannot spawn at %d %d", centerX, centerY);
    }
}

void CDungeon::SpawnStoneDoor(const char* key, const char* position)
{
    SpawnUnique(key, 13001, position);
}

void CDungeon::SpawnWoodenDoor(const char* key, const char* position)
{
    SpawnUnique(key, 13000, position);
    UniqueSetMaxHP(key, 10000);
    UniqueSetHP(key, 10000);
    UniqueSetDefGrade(key, 300);
}

void CDungeon::SpawnNameMob(DWORD vnum, int x, int y, const char* name)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;
    const auto worldY = static_cast<std::int32_t>(static_cast<std::uint32_t>(map->m_setting.iBaseY) + y);
    const auto worldX = static_cast<std::int32_t>(static_cast<std::uint32_t>(map->m_setting.iBaseX) + x);
    if (CHARACTER* character = CHARACTER_MANAGER::instance().SpawnMob(
            vnum, m_lMapIndex, worldX, worldY, 0, false, -1, true)) {
        character->SetName(name);
        character->SetDungeon(this);
    }
}

void CDungeon::SpawnGotoMob(long fromX, long fromY, long toX, long toY)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    sys_log(0, "SpawnGotoMob %d %d to %d %d", static_cast<int>(fromX), static_cast<int>(fromY),
             static_cast<int>(toX), static_cast<int>(toY));
    if (!map)
        return;

    constexpr DWORD MOB_GOTO_VNUM = 20039;
    const auto worldX = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(map->m_setting.iBaseX) + static_cast<std::uint32_t>(fromX) * 100u);
    const auto worldY = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(map->m_setting.iBaseY) + static_cast<std::uint32_t>(fromY) * 100u);
    if (CHARACTER* character = CHARACTER_MANAGER::instance().SpawnMob(
            MOB_GOTO_VNUM, m_lMapIndex, worldX, worldY, 0, false, -1, true)) {
        char name[31];
        std::snprintf(name, 30, ". %ld %ld", static_cast<long>(static_cast<std::int32_t>(toX)),
                      static_cast<long>(static_cast<std::int32_t>(toY)));
        character->SetName(name);
        character->SetDungeon(this);
    }
}

void CDungeon::SpawnMoveUnique(const char* key, DWORD vnum, const char* from, const char* to)
{
    const auto destination = m_map_Area.find(to);
    if (destination == m_map_Area.end()) {
        _sys_err("SpawnMoveUnique", 0x154, "Wrong position string : %s", to);
        return;
    }
    const auto source = m_map_Area.find(from);
    if (source == m_map_Area.end()) {
        _sys_err("SpawnMoveUnique", 0x15b, "Wrong position string : %s", from);
        return;
    }

    const TAreaInfo& start = source->second;
    const TAreaInfo& finish = destination->second;
    const int direction = start.dir == -1
        ? number_ex(0, 359, const_cast<char*>("dungeon.cpp"), 0x163) : start.dir;
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;
    for (int attempt = 0; attempt < 100; ++attempt) {
        const int localX = number_ex(start.sx, start.ex, const_cast<char*>("dungeon.cpp"), 0x169);
        const int localY = number_ex(start.sy, start.ey, const_cast<char*>("dungeon.cpp"), 0x16a);
        const int localTX = number_ex(finish.sx, finish.ex, const_cast<char*>("dungeon.cpp"), 0x16b);
        const int localTY = number_ex(finish.sy, finish.ey, const_cast<char*>("dungeon.cpp"), 0x16c);
        const int y = AddDungeonCoordinate(map->m_setting.iBaseY, localY);
        const int x = AddDungeonCoordinate(map->m_setting.iBaseX, localX);
        CHARACTER* character = CHARACTER_MANAGER::instance().SpawnMob(
            vnum, m_lMapIndex, x, y, 0, false, direction, true);
        if (!character) {
            const int centerY = UniqueSpawnFailureCoordinate(map->m_setting.iBaseY, start.sy, start.ey);
            const int centerX = UniqueSpawnFailureCoordinate(map->m_setting.iBaseX, start.sx, start.ex);
            _sys_err("SpawnMoveUnique", 0x17b, "Cannot spawn at %d %d", centerX, centerY);
            continue;
        }

        m_map_UniqueMob.emplace(key, character);
        character->AddAffect(213, 0, 0, 8, 65535, 0, true, false);
        character->SetDungeon(this);
        const int ty = AddDungeonCoordinate(map->m_setting.iBaseY, localTY);
        const int tx = AddDungeonCoordinate(map->m_setting.iBaseX, localTX);
        if (character->Goto(tx, ty))
            character->SendMovePacket(0, 0, 0, 0, 0, 0, -1);
    }
}

void CDungeon::SpawnMoveGroup(DWORD vnum, const char* from, const char* to, int count)
{
    const auto destination = m_map_Area.find(to);
    if (destination == m_map_Area.end()) {
        _sys_err("SpawnMoveGroup", 0x289, "Wrong position string : %s", to);
        return;
    }
    const auto source = m_map_Area.find(from);
    if (source == m_map_Area.end()) {
        _sys_err("SpawnMoveGroup", 0x291, "Wrong position string : %s", from);
        return;
    }

    const TAreaInfo& start = source->second;
    const TAreaInfo& finish = destination->second;
    if (start.dir == -1)
        number_ex(0, 359, const_cast<char*>("dungeon.cpp"), 0x29a);

    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map)
        return;
    for (int index = 0; index < count; ++index) {
        const int localX = number_ex(finish.sx, finish.ex, const_cast<char*>("dungeon.cpp"), 0x2a0);
        const int tx = AddDungeonCoordinate(map->m_setting.iBaseX, localX);
        const int localY = number_ex(finish.sy, finish.ey, const_cast<char*>("dungeon.cpp"), 0x2a1);
        const int ty = AddDungeonCoordinate(map->m_setting.iBaseY, localY);

        const int ey = AddDungeonCoordinate(map->m_setting.iBaseY, start.ey);
        const int ex = AddDungeonCoordinate(map->m_setting.iBaseX, start.ex);
        const int sy = AddDungeonCoordinate(map->m_setting.iBaseY, start.sy);
        const int sx = AddDungeonCoordinate(map->m_setting.iBaseX, start.sx);
        CHARACTER_MANAGER::instance().SpawnMoveGroup(
            vnum, m_lMapIndex, sx, sy, ex, ey, tx, ty, nullptr, true);
    }
}

void CDungeon::SetUnique(const char* key, DWORD vid)
{
    if (CHARACTER* character = CHARACTER_MANAGER::instance().Find(vid)) {
        m_map_UniqueMob.emplace(key, character);
        character->AddAffect(213, 0, 0, 8, 65535, 0, true, false);
    }
}

void CDungeon::PurgeUnique(const std::string& key)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end()) {
        _sys_err("PurgeUnique", 0x1c2, "Unknown Key or Dead: %s", key.c_str());
        return;
    }
    CHARACTER* character = entry->second;
    m_map_UniqueMob.erase(entry);
    CHARACTER_MANAGER::instance().DestroyCharacter(character);
}

void CDungeon::KillUnique(const std::string& key)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end()) {
        _sys_err("KillUnique", 0x1cf, "Unknown Key or Dead: %s", key.c_str());
        return;
    }
    CHARACTER* character = entry->second;
    m_map_UniqueMob.erase(entry);
    character->Dead(nullptr, false);
}

bool CDungeon::IsUniqueDead(const std::string& key)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end()) {
        _sys_err("IsUniqueDead", 0x1f9, "Unknown Key or Dead : %s", key.c_str());
        return true;
    }
    return entry->second->IsDead();
}

double CDungeon::GetUniqueHpPerc(const std::string& key)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end()) {
        _sys_err("GetUniqueHpPerc", 0x1dc, "Unknown Key : %s", key.c_str());
        return 0;
    }
    CHARACTER* character = entry->second;
    // The original x87 result was stored directly as a Lua double.
    const int maxHP = character->GetMaxHP();
    const int hp = character->GetHP();
    return static_cast<double>(100.0L * hp / maxHP);
}

void CDungeon::UniqueSetDefGrade(const std::string& key, int grade)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end()) {
        _sys_err("UniqueSetDefGrade", 0x149, "Unknown Key : %s", key.c_str());
        return;
    }
    CHARACTER* character = entry->second;
    const auto current = static_cast<std::uint32_t>(character->GetPoint(POINT_DEF_GRADE));
    const auto delta = static_cast<std::int32_t>(static_cast<std::uint32_t>(grade) - current);
    entry->second->PointChange(POINT_DEF_GRADE, delta, false, false);
}

void CDungeon::UniqueSetHP(const std::string& key, int hp)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end())
        _sys_err("UniqueSetHP", 0x13e, "Unknown Key : %s", key.c_str());
    else
        entry->second->SetHP(hp);
}

void CDungeon::UniqueSetMaxHP(const std::string& key, int maxHP)
{
    const auto entry = m_map_UniqueMob.find(key);
    if (entry == m_map_UniqueMob.end())
        _sys_err("UniqueSetMaxHP", 0x133, "Unknown Key : %s", key.c_str());
    else
        entry->second->SetMaxHP(maxHP);
}

struct FWarpToDungeon
{
    long mapIndex;
    long x;
    long y;

    void operator()(CHARACTER* character) const
    {
        character->SaveExitLocation();
        character->WarpSet(x, y, mapIndex);
    }
};

void CDungeon::Join(CHARACTER* character)
{
    const long mapIndex = m_lMapIndex;
    if (SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex)) {
        const FWarpToDungeon warp = {mapIndex, map->m_setting.posSpawn.x, map->m_setting.posSpawn.y};
        warp(character);
    }
}

void CDungeon::JoinParty(CParty* party)
{
    party->SetDungeon(this);
    m_map_pkParty.emplace(party, 0);
    const long mapIndex = m_lMapIndex;
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!map)
        return;
    const FWarpToDungeon warp = {mapIndex, map->m_setting.posSpawn.x, map->m_setting.posSpawn.y};
    party->ForEachOnlineMember(warp);
}

struct FSendDestPosition
{
    TPacketGCDungeon p1;
    TPacketGCDungeonDestPosition p2;

    void operator()(CHARACTER* member) const
    {
        if (DESC* desc = member->GetDesc()) {
            desc->BufferedPacket(&p1, sizeof(p1));
            desc->Packet(&p2, sizeof(p2));
        }
    }
};

void CDungeon::SendDestPositionToParty(CParty* party, long x, long y)
{
    if (m_map_pkParty.find(party) == m_map_pkParty.end()) {
        const DWORD leaderPid = party->GetLeaderPID();
        _sys_err("SendDestPositionToParty", 0x55, "PARTY %lu not in DUNGEON %d",
                 leaderPid, static_cast<int>(m_lMapIndex));
        return;
    }

    const FSendDestPosition destination = {
        {0x6e, sizeof(TPacketGCDungeon) + sizeof(TPacketGCDungeonDestPosition), 1},
        {static_cast<std::int32_t>(x), static_cast<std::int32_t>(y)}
    };
    party->ForEachNearMember(destination);
}

struct FCountMonster
{
    int n = 0;

    void operator()(LPENTITY entity)
    {
        if (entity->IsType(0) && !entity->GetDesc())
            ++n;
    }
};

int CDungeon::CountRealMonster()
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lOrigMapIndex);
    if (!map)
    {
        _sys_err("CountRealMonster", 0x31e, "Cannot get map");
        return 0;
    }

    FCountMonster count;
    for (const auto& entry : *map)
        entry.second->ForEachAround(count);
    return count.n;
}

struct FExitDungeon
{
    void operator()(LPENTITY entity)
    {
        if (entity->IsType(0) && entity->GetDesc())
            static_cast<CHARACTER*>(entity)->ExitToSavedLocation();
    }
};


void CDungeon::ExitAll()
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (!map) {
        _sys_err("ExitAll", 0x342, "Cannot get map");
        return;
    }
    FExitDungeon exit;
    for (const auto& entry : *map)
        entry.second->ForEachAround(exit);
}

void CDungeon::JumpAll(long from, int x, int y)
{
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(from);
    if (map)
        WarpMapPlayers(*map, m_lMapIndex, x, y);
    else
        _sys_err("JumpAll", 0xf3, "Cannot get map");
}

void CDungeon::JumpToEliminateLocation()
{
    if (CDungeon* target = CDungeonManager::instance().Find(m_lWarpMapIndex)) {
        const int y = m_lWarpY;
        const int x = m_lWarpX;
        target->JumpAll(m_lMapIndex, x, y);
        if (!m_stRegenFile.empty()) {
            target->SpawnRegen(m_stRegenFile.c_str(), true);
            m_stRegenFile.clear();
        }
        return;
    }

    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (map) {
        const int x = m_lWarpX;
        const long destination = m_lWarpMapIndex;
        const int y = m_lWarpY;
        WarpMapPlayers(*map, destination, x, y);
    } else {
        _sys_err("JumpToEliminateLocation", 0x424, "Cannot get map");
    }
}

void CDungeon::SpawnRegen(const char* filename, bool once)
{
    if (!filename) {
        _sys_err("SpawnRegen", 0x265, "CDungeon::SpawnRegen(filename=NULL, bOnce=%d) - m_lMapIndex[%d]",
                 once, static_cast<int>(m_lMapIndex));
        return;
    }
    SECTREE_MAP* map = SECTREE_MANAGER::instance().GetMap(m_lMapIndex);
    if (map) {
        const int baseY = map->m_setting.iBaseY;
        const int baseX = map->m_setting.iBaseX;
        regen_do(filename, m_lMapIndex, baseX, baseY, this, once);
    } else {
        _sys_err("SpawnRegen", 0x26c, "CDungeon::SpawnRegen(filename=%s, bOnce=%d) - m_lMapIndex[%d]",
                 filename, once, static_cast<int>(m_lMapIndex));
    }
}

void CDungeon::SetExitAllAtEliminate(long time)
{
    sys_log(0, "SetExitAllAtEliminate");
    m_bExitAllAtEliminate = true;
    m_iWarpDelay = static_cast<std::int32_t>(time);
}

void CDungeon::SetWarpAtEliminate(long time, long map, int x, int y, const char* filename)
{
    sys_log(0, "SetWarpAtEliminate");
    m_bWarpAtEliminate = true;
    m_iWarpDelay = static_cast<std::int32_t>(time);
    m_lWarpMapIndex = static_cast<std::int32_t>(map);
    m_lWarpX = x;
    m_lWarpY = y;
    m_stRegenFile = filename ? filename : "";
}

CDungeon* CDungeonManager::Create(long originalMap)
{
    const long map = SECTREE_MANAGER::instance().CreatePrivateMap(originalMap);
    if (!map) {
        sys_log(0, "Fail to Create Dungeon : OrginalMapindex %d NewMapindex %d", static_cast<int>(originalMap), 0);
        return 0;
    }
    CDungeon* dungeon = new CDungeon(originalMap, map);
    m_map_pkDungeon.insert(std::make_pair(static_cast<std::uint32_t>(map), dungeon));
    return dungeon;
}

CDungeon* CDungeonManager::Find(long map)
{
    const auto it = m_map_pkDungeon.find(static_cast<std::uint32_t>(map));
    return it != m_map_pkDungeon.end() ? it->second : 0;
}

void CDungeonManager::Destroy(CDungeon* dungeon)
{
    sys_log(0, "DUNGEON destroy %p", static_cast<void*>(dungeon));
    m_map_pkDungeon.erase(static_cast<std::uint32_t>(dungeon->m_lMapIndex));
    SECTREE_MANAGER::instance().DestroyPrivateMap(dungeon->m_lMapIndex);
    delete dungeon;
}

void CDungeon::IncKillCount(CHARACTER*, CHARACTER* victim)
{
    int& count = victim->IsStone() ? m_iStoneKill : m_iMobKill;
    count = static_cast<std::int32_t>(static_cast<std::uint32_t>(count) + 1u);
}

void CDungeon::DeadCharacter(CHARACTER* character)
{
    if (character->GetDesc())
        return;
    for (auto it = m_map_UniqueMob.begin(); it != m_map_UniqueMob.end();) {
        const auto entry = it++;
        if (entry->second == character) {
            m_map_UniqueMob.erase(entry);
            return;
        }
    }
}

// 08185010
// 08185020
void CDungeon::UseRevive(CHARACTER*)
{
    m_bUseRevive = true;
}

// 08185030
bool CDungeon::IsUsePotion()
{
    return m_bUsePotion;
}

// 08185040
bool CDungeon::IsUseRevive()
{
    return m_bUseRevive;
}

// 08185050
int CDungeon::GetKillMobCount()
{
    return m_iMobKill;
}

// 08185060
int CDungeon::GetKillStoneCount()
{
    return m_iStoneKill;
}

void CDungeon::UsePotion(CHARACTER*)
{
    m_bUsePotion = true;
}
