#include "castle.h"
#include "config.h"
#include "constants.h"
#include "start_position.h"
#include "locale_service.h"
#include "p2p.h"
#include "buffer_manager.h"
#include "../../libthecore/include/utils.h"
#include <cstdint>
#include <cstring>

void SendNotice(const char* text);
#include "char.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "../../libthecore/include/log.h"
#include <cctype>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

enum CASTLE_STATE {
    CASTLE_SIEGE_NONE = 0,
    CASTLE_SIEGE_START = 1,
    CASTLE_SIEGE_END = 2,
};
static CASTLE_STATE s_siege_state = CASTLE_SIEGE_NONE;

// 08118d00
static bool FN_is_castle_map(int map)
{
    return static_cast<unsigned int>(map) - 181u < 3u;
}

// 0811a210
bool castle_is_tower_vnum(DWORD vnum)
{
    return vnum - static_cast<DWORD>(11506) < 5;
}

// 0811a010
bool castle_can_attack(CHARACTER* attacker, CHARACTER* victim)
{
    if (!attacker || !victim || !FN_is_castle_map(attacker->GetMapIndex()))
        return false;
    if (attacker->GetDesc() && victim->GetDesc())
        return true;
    if (s_siege_state == CASTLE_SIEGE_END && castle_is_tower_vnum(victim->GetRaceNum()))
        return attacker->GetEmpire() == victim->GetEmpire();
    return attacker->GetEmpire() != victim->GetEmpire();
}

namespace {
constexpr int frogCount = 20;
constexpr int guardRegions = 4;
constexpr int guardCount = 10;
constexpr DWORD frogVnum = 11505;

struct CastlePoint { int x, y; };
struct CastleRect { int sx, sy, ex, ey; };
struct CastleResidents
{
    CHARACTER* frogs[frogCount];
    CHARACTER* guards[guardRegions][guardCount];
    DWORD guardGroups[guardRegions][guardCount];
};

CastleResidents residents[4];
bool castleInitialized;
const int castleMaps[4] = {0, 181, 183, 182};

const CastlePoint frogPositions[4][frogCount] = {
    {},
    {
        {225, 45},
        {231, 45},
        {237, 45},
        {243, 45},
        {249, 45},
        {225, 50},
        {231, 50},
        {237, 50},
        {243, 50},
        {249, 50},
        {261, 45},
        {267, 45},
        {273, 45},
        {279, 45},
        {285, 45},
        {261, 50},
        {267, 50},
        {273, 50},
        {279, 50},
        {285, 50},
    },
    {
        {221, 36},
        {227, 36},
        {233, 36},
        {239, 36},
        {245, 36},
        {269, 36},
        {275, 36},
        {281, 36},
        {287, 36},
        {293, 36},
        {221, 41},
        {227, 41},
        {233, 41},
        {239, 41},
        {245, 41},
        {269, 41},
        {275, 41},
        {281, 41},
        {287, 41},
        {293, 41},
    },
    {
        {225, 45},
        {231, 45},
        {237, 45},
        {243, 45},
        {249, 45},
        {225, 50},
        {231, 50},
        {237, 50},
        {243, 50},
        {249, 50},
        {261, 45},
        {267, 45},
        {273, 45},
        {279, 45},
        {285, 45},
        {261, 50},
        {267, 50},
        {273, 50},
        {279, 50},
        {285, 50},
    },
};
const CastleRect guardBounds[4][guardRegions] = {
    {},
    {
        {74, 170, 96, 180},
        {237, 135, 270, 146},
        {235, 260, 278, 273},
        {421, 167, 435, 205},
    },
    {
        {109, 172, 128, 202},
        {237, 140, 282, 153},
        {232, 261, 279, 276},
        {390, 173, 403, 205},
    },
    {
        {74, 170, 96, 120},
        {237, 135, 270, 146},
        {235, 260, 278, 273},
        {421, 167, 435, 205},
    },
};

bool IsCastleEmpire(int empire)
{
    return empire >= 1 && empire <= 3;
}

long FN_castle_map_index(int empire)
{
    return IsCastleEmpire(empire) ? castleMaps[empire] : 0;
}

int FN_empty_frog_index(int empire)
{
    for (int slot = 0; slot < frogCount; ++slot)
        if (!residents[empire].frogs[slot])
            return slot;
    return -1;
}

const CastlePoint* FN_empty_frog_pos(int empire)
{
    const int slot = FN_empty_frog_index(empire);
    return slot >= 0 ? &frogPositions[empire][slot] : nullptr;
}

int FN_empty_guard_pos(int empire, int region)
{
    for (int slot = 0; slot < guardCount; ++slot)
        if (!residents[empire].guards[region][slot])
            return slot;
    return -1;
}
}

CHARACTER* castle_spawn_frog(int empire)
{
    if (!IsCastleEmpire(empire))
        return nullptr;
    const CastlePoint* position = FN_empty_frog_pos(empire);
    if (!position)
        return nullptr;
    return castle_spawn_frog_force(empire, static_cast<int>(position - frogPositions[empire]));
}

CHARACTER* castle_spawn_frog_force(int empire, int slot)
{
    if (!IsCastleEmpire(empire) || slot < 0 || slot >= frogCount)
        return nullptr;

    const int mapIndex = FN_castle_map_index(empire);
    const auto* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!map)
        return nullptr;

    const auto& position = frogPositions[empire][slot];
    CHARACTER* frog = CHARACTER_MANAGER::instance().SpawnMob(frogVnum, mapIndex,
        map->m_setting.iBaseX + position.x * 100, map->m_setting.iBaseY + position.y * 100,
        0, false, 1, true);
    if (frog) {
        frog->SetEmpire(static_cast<BYTE>(empire));
        residents[empire].frogs[slot] = frog;
    }
    return frog;
}

CHARACTER* castle_spawn_guard(int empire, DWORD group, int region)
{
    if (!IsCastleEmpire(empire) || region < 0 || region >= guardRegions)
        return nullptr;

    const int mapIndex = FN_castle_map_index(empire);
    const auto* map = SECTREE_MANAGER::instance().GetMap(mapIndex);
    if (!map)
        return nullptr;

    auto& record = residents[empire];
    const int slot = FN_empty_guard_pos(empire, region);
    if (slot < 0)
        return nullptr;

    const auto& bounds = guardBounds[empire][region];
    const int x = map->m_setting.iBaseX;
    const int y = map->m_setting.iBaseY;
    CHARACTER* guard = CHARACTER_MANAGER::instance().SpawnGroup(group, mapIndex,
        x + bounds.sx * 100, y + bounds.sy * 100, x + bounds.ex * 100, y + bounds.ey * 100,
        nullptr, false, nullptr);
    if (guard) {
        guard->SetEmpire(static_cast<BYTE>(empire));
        record.guards[region][slot] = guard;
        record.guardGroups[region][slot] = group;
    }
    return guard;
}

int castle_guard_count(int empire, int region)
{
    if (!IsCastleEmpire(empire) || region < 0 || region >= guardRegions)
        return 0;
    int count = 0;
    for (CHARACTER* guard : residents[empire].guards[region])
        if (guard)
            ++count;
    return count;
}

int castle_frog_count(int empire)
{
    if (!IsCastleEmpire(empire))
        return 0;
    int count = 0;
    for (CHARACTER* frog : residents[empire].frogs)
        if (frog)
            ++count;
    return count;
}

bool castle_boot()
{
    castleInitialized = true;
    std::ifstream file("castle_data.txt");
    if (!file)
        return false;

    int empire = 0;
    int region = 0;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream fields(line);
        std::string key;
        fields >> key;
        for (char& c : key)
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

        if (key == "empire") {
            if (!(fields >> empire)) {
                _sys_err("castle_boot", 0x1db, "wrong empire number is null");
                return false;
            }
        } else if (key == "frog") {
            int present;
            for (int slot = 0; slot < frogCount && fields >> present; ++slot) {
                if (present)
                    castle_spawn_frog_force(empire, slot);
            }
        } else if (key == "guard") {
            DWORD group;
            while (fields >> group) {
                if (group)
                    castle_spawn_guard(empire, group, region);
            }
            ++region;
        } else if (key == "end") {
            region = 0;
        }
    }
    return true;
}

void castle_save()
{
    if (!castleInitialized)
        return;
    std::FILE* file = std::fopen("castle_data.txt", "w");
    if (!file) {
        _sys_err("castle_save", 0x212, "<FAIL> fopen(%s)", "castle_data.txt");
        return;
    }

    for (int empire = 1; empire <= 3; ++empire) {
        const auto& record = residents[empire];
        std::fprintf(file, "section\n\tempire %d\n\tfrog ", empire);
        for (CHARACTER* frog : record.frogs)
            std::fprintf(file, " %d", frog ? 1 : 0);
        std::fputc('\n', file);
        for (const auto& groups : record.guardGroups) {
            std::fprintf(file, "\tguard ");
            for (DWORD group : groups)
                std::fprintf(file, " %d", static_cast<int>(group));
            std::fputc('\n', file);
        }
        std::fprintf(file, "end\n");
    }
    std::fclose(file);
}

bool castle_is_guard_vnum(DWORD vnum)
{
    return vnum - static_cast<DWORD>(11100) < 18;
}

void castle_guard_die(CHARACTER* character, CHARACTER*)
{
    if (!character || !IsCastleEmpire(character->GetEmpire()))
        return;
    auto& record = residents[character->GetEmpire()];
    for (int region = 0; region < guardRegions; ++region) {
        for (int slot = 0; slot < guardCount; ++slot) {
            if (record.guards[region][slot] == character) {
                record.guards[region][slot] = nullptr;
                record.guardGroups[region][slot] = 0;
            }
        }
    }
    castle_save();
}

void castle_frog_die(CHARACTER* character, CHARACTER* killer)
{
    if (!character || !killer || !IsCastleEmpire(character->GetEmpire()))
        return;
    for (CHARACTER*& frog : residents[character->GetEmpire()].frogs) {
        if (frog == character) {
            frog = nullptr;
            killer->PointChange(POINT_GOLD, 10000000, true, false);
            castle_save();
            return;
        }
    }
}

namespace {
constexpr int towerSlots = 10;
struct CastleSiege
{
    CHARACTER* towers[towerSlots];
    LPEVENT siegeEvent;
    LPEVENT stoneEvent;
};

struct CastleSiegeEventInfo { int empire; int pulse; };
struct CastleStoneEventInfo { int empire; int spawnCount; };

CastleSiege sieges[4];
int siegeEmpire;

const CastleRect siegeBounds[4] = {
    {0, 0, 0, 0},
    {85, 135, 420, 265},
    {120, 130, 405, 276},
    {85, 135, 420, 265},
};
const char towerDestroyed[] = "%s\300\314 \272\300\310\255\270\246 \306\304\261\253\307\337\275\300\264\317\264\331.";
const char failedDefense[] = "%s\300\314 \274\366\274\272\277\241 \275\307\306\320\307\317\277\251 \300\374\300\357\277\241 \306\320\271\350\307\317\277\264\275\300\264\317\264\331..";
const char allTowersDestroyed[] = "%s\300\314 \270\360\265\347 \272\300\310\255\270\246 \306\304\261\253\307\317\277\264\275\300\264\317\264\331.";
const char towersSpawned[] = "%s\277\241 \300\374\300\357\300\307 \275\303\300\333\300\273 \276\313\270\256\264\302 \272\300\310\255\260\241 \263\252\305\270\263\265\275\300\264\317\264\331.";
const char defenseContinues[] = "%s\277\241\274\255 \272\300\310\255\270\246 \265\321\267\257\275\316\260\355 \300\374\305\365\260\241 \301\370\307\340\301\337\300\324\264\317\264\331.";
const char defenseSucceeded[] = "%s\300\314 \274\366\274\272\277\241 \274\272\260\370\307\337\275\300\264\317\264\331.";
const char rewardWindow[] = "\301\366\261\335\272\316\305\315 %s\300\272 30\272\320\260\243 \272\300\310\255\270\246 \306\304\261\253\307\317\277\251 \272\270\273\363\300\273 \310\271\265\346 \307\322 \274\366 \300\326\275\300\264\317\264\331.";
const char rewardWindowEnded[] = "30\272\320\300\314 \260\346\260\372\307\337\275\300\264\317\264\331.. \272\300\310\255\260\241 \273\347\266\363\301\375\264\317\264\331.";

int CastleRandom(int low, int high, int line)
{
    return number_ex(low, high, const_cast<char*>("castle.cpp"), line);
}

void SendCastleNotice(const char* message)
{
    const auto length = static_cast<std::uint32_t>(std::strlen(message) + 1);
    const BYTE header[] = {
        4, static_cast<BYTE>(length), static_cast<BYTE>(length >> 8),
        static_cast<BYTE>(length >> 16), static_cast<BYTE>(length >> 24),
    };
    TEMP_BUFFER buffer;
    buffer.write(header, sizeof(header));
    buffer.write(message, length);
    P2P_MANAGER::instance().Send(buffer.read_peek(), buffer.size());
    SendNotice(message);
}

void SendCastleEmpireNotice(const char* key, int empire)
{
    const char* nation = locale_find(g_nation_name[empire], g_iUseLocale);
    char notice[1024];
    std::snprintf(notice, sizeof(notice), locale_find(key, g_iUseLocale), nation);
    SendCastleNotice(notice);
}

CHARACTER* SpawnSiegeMob(DWORD vnum, int empire, const SECTREE_MAP& map)
{
    const auto& bounds = siegeBounds[empire];
    const int x = map.m_setting.iBaseX;
    const int y = map.m_setting.iBaseY;
    const auto world = [](int base, int tile) {
        return static_cast<std::int32_t>(
            static_cast<std::uint32_t>(base) + static_cast<std::uint32_t>(tile) * 100u);
    };
    return CHARACTER_MANAGER::instance().SpawnMobRange(vnum, FN_castle_map_index(empire),
        world(x, bounds.sx), world(y, bounds.sy), world(x, bounds.ex), world(y, bounds.ey),
        false, false, false);
}

// 081189e0
DWORD FN_random_stone()
{
    const DWORD stones[] = {8012, 8013, 8014, 8024, 8025, 8026, 8027};
    return stones[CastleRandom(0, 6, 0x140)];
}

DWORD FN_random_tower()
{
    const DWORD towers[] = {11506, 11507, 11508, 11509, 11510};
    return towers[CastleRandom(0, 4, 0x2eb)];
}
}

// 08118670
static void FN_warp_to_village(int empire)
{
    auto* map = SECTREE_MANAGER::instance().GetMap(FN_castle_map_index(siegeEmpire));
    if (!map)
        return;
    for (const auto& entry : *map) {
        entry.second->ForEachEntity([&](LPENTITY entity) {
            if (!entity->IsType(0) || !entity->GetDesc())
                return;
            auto* player = static_cast<CHARACTER*>(entity);
            if (player->GetEmpire() == empire)
                player->WarpSet(g_start_position[empire][0], g_start_position[empire][1],
                                g_start_map[empire]);
        });
    }
}

// 081187e0
static void FN_warp_losers(int winner)
{
    for (int empire = 1; empire <= 3; ++empire) {
        if (empire != winner)
            FN_warp_to_village(empire);
    }
}

void castle_end_siege()
{
    siegeEmpire = 0;
    s_siege_state = CASTLE_SIEGE_NONE;
    for (int empire = 1; empire <= 3; ++empire) {
        auto& siege = sieges[empire];
        event_cancel(&siege.siegeEvent);
        event_cancel(&siege.stoneEvent);
        for (CHARACTER*& tower : siege.towers) {
            if (tower) {
                CHARACTER* oldTower = tower;
                tower = nullptr;
                CHARACTER_MANAGER::instance().DestroyCharacter(oldTower);
            }
        }
    }
}

EVENTFUNC(castle_siege_event)
{
    auto* info = static_cast<CastleSiegeEventInfo*>(event->info);
    info->pulse += passes_per_sec * 300;
    if (info->pulse < passes_per_sec * 1800) {
        SendCastleEmpireNotice(defenseContinues, siegeEmpire);
        return passes_per_sec * 300;
    }
    if (s_siege_state == CASTLE_SIEGE_START) {
        SendCastleEmpireNotice(defenseSucceeded, siegeEmpire);
        SendCastleEmpireNotice(rewardWindow, siegeEmpire);
        s_siege_state = CASTLE_SIEGE_END;
        if (LC_IsJapan())
            FN_warp_losers(siegeEmpire);
        return passes_per_sec * 1800;
    }
    sieges[info->empire].siegeEvent = nullptr;
    if (s_siege_state == CASTLE_SIEGE_END) {
        SendCastleNotice(locale_find(rewardWindowEnded, g_iUseLocale));
        castle_end_siege();
    }
    return 0;
}

EVENTFUNC(castle_stone_event)
{
    auto* info = static_cast<CastleStoneEventInfo*>(event->info);
    const auto* map = SECTREE_MANAGER::instance().GetMap(FN_castle_map_index(siegeEmpire));
    if (map && info->spawnCount < 30) {
        for (int count = 0; count < 15; ++count)
            SpawnSiegeMob(FN_random_stone(), info->empire, *map);
        info->spawnCount += 15;
        if (info->spawnCount < 30)
            return passes_per_sec * 1800;
    }
    sieges[info->empire].stoneEvent = nullptr;
    return 0;
}

// 08119960
static void FN_spawn_tower(int empire, SECTREE_MAP* map)
{
    for (CHARACTER*& tower : sieges[empire].towers) {
        if (tower)
            continue;
        tower = SpawnSiegeMob(FN_random_tower(), empire, *map);
        if (tower)
            tower->SetEmpire(static_cast<BYTE>(empire));
        return;
    }
}

bool castle_spawn_tower(int empire)
{
    if (!IsCastleEmpire(empire))
        return false;
    auto* map = SECTREE_MANAGER::instance().GetMap(FN_castle_map_index(empire));
    if (!map)
        return false;

    auto& siege = sieges[empire];
    for (CHARACTER*& tower : siege.towers) {
        if (tower) {
            CHARACTER* oldTower = tower;
            tower = nullptr;
            oldTower->Dead(nullptr, true);
        }
    }
    const int spawnCount = CastleRandom(5, 10, 0x313);
    for (int count = 0; count < spawnCount; ++count)
        FN_spawn_tower(empire, map);
    SendCastleEmpireNotice(towersSpawned, empire);
    return true;
}

void castle_start_siege(int empire)
{
    if (!IsCastleEmpire(empire) || s_siege_state != CASTLE_SIEGE_NONE)
        return;

    siegeEmpire = empire;
    s_siege_state = CASTLE_SIEGE_START;
    castle_spawn_tower(empire);

    void* raw = nullptr;
    const DWORD siegeId = __AllocEventInfo(sizeof(CastleSiegeEventInfo), &raw);
    *static_cast<CastleSiegeEventInfo*>(raw) = {empire, 0};
    sieges[empire].siegeEvent = event_create_ex(castle_siege_event, siegeId, passes_per_sec * 300,
                                              const_cast<char*>("castle.cpp"), 0x25d);

    const DWORD stoneId = __AllocEventInfo(sizeof(CastleStoneEventInfo), &raw);
    *static_cast<CastleStoneEventInfo*>(raw) = {empire, 0};
    sieges[empire].stoneEvent = event_create_ex(castle_stone_event, stoneId, passes_per_sec,
                                              const_cast<char*>("castle.cpp"), 0x267);
}

void castle_siege(int empire)
{
    auto& maps = SECTREE_MANAGER::instance();
    if (!maps.GetMap(181) || !maps.GetMap(182) || !maps.GetMap(183))
        return;
    if (s_siege_state == CASTLE_SIEGE_NONE)
        castle_start_siege(empire);
    else
        castle_end_siege();
}

void castle_tower_die(CHARACTER* character, CHARACTER* killer)
{
    if (!character || !killer || !IsCastleEmpire(siegeEmpire) ||
        s_siege_state == CASTLE_SIEGE_NONE)
        return;

    const int killerEmpire = killer->GetEmpire();
    SendCastleEmpireNotice(towerDestroyed, IsCastleEmpire(killerEmpire) ? killerEmpire : 0);
    bool anyRemaining = false;
    for (CHARACTER*& tower : sieges[siegeEmpire].towers) {
        if (tower == character)
            tower = nullptr;
        anyRemaining |= tower != nullptr;
    }
    if (anyRemaining)
        return;

    SendCastleEmpireNotice(s_siege_state == CASTLE_SIEGE_START ? failedDefense : allTowersDestroyed,
                           siegeEmpire);
    castle_end_siege();
}
