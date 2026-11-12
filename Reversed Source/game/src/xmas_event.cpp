#include "xmas_event.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "config.h"
#include "event.h"
#include "questmanager.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <cstring>

namespace xmas {
struct spawn_santa_info { std::int32_t lMapIndex; };
static_assert(sizeof(spawn_santa_info) == 4, "2009 Santa respawn event data");

long spawn_santa_event(LPEVENT event)
{
    long map=static_cast<spawn_santa_info*>(event->info)->lMapIndex;
    if (quest::CQuestManager::instance().GetEventFlag("xmas_santa")) {
        CHARACTER_MANAGER& manager=CHARACTER_MANAGER::instance();
        if (manager.GetCharacterSetByRaceNum(20031)->empty()) {
            if (!manager.SpawnMobRandomPosition(20031,map)) return passes_per_sec*5;
            sys_log(0,"santa comes to town!");
        }
    }
    return 0;
}

void SpawnSanta(long map, int delay)
{
    if (test_server) delay/=60;
    sys_log(0,"santa respawn time = %d",delay);
    void* rawInfo = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(spawn_santa_info), &rawInfo);
    static_cast<spawn_santa_info*>(rawInfo)->lMapIndex = static_cast<std::int32_t>(map);
    unsigned int bits=static_cast<unsigned int>(delay)*static_cast<unsigned int>(passes_per_sec);
    int when;
    std::memcpy(&when,&bits,sizeof(when));
    event_create_ex(spawn_santa_event,id,when,const_cast<char*>("xmas_event.cpp"),0xa9);
}
}

namespace xmas {
namespace {
void DestroyRace(DWORD race)
{
    CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
    CHARACTER_SET* members = characters.GetCharacterSetByRaceNum(race);
    CHARACTER_SET::iterator it = members->begin();
    while (it != members->end())
        characters.DestroyCharacter(*it++);
}
}

void UpdateFireworkSellers(bool spawn, const char* function, int line)
{
    if (!spawn) {
        DestroyRace(9004);
        return;
    }

    static const int positions[][3] = {
        {1, 615, 618}, {3, 500, 625}, {21, 598, 665},
        {23, 476, 360}, {41, 318, 629}, {43, 478, 375},
    };
    for (const auto& position : positions) {
        const long map = position[0];
        if (!map_allow_find(map))
            continue;
        PIXEL_POSITION base;
        if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(map, base)) {
            _sys_err(function, line, "cannot get map base position %ld", map);
            continue;
        }
        const auto x = static_cast<std::int32_t>(static_cast<std::uint32_t>(base.x) + position[1] * 100u);
        const auto y = static_cast<std::int32_t>(static_cast<std::uint32_t>(base.y) + position[2] * 100u);
        CHARACTER_MANAGER::instance().SpawnMob(9004, map, x, y, 0, false, -1, true);
    }
}

void ProcessEventFlag(const std::string& name, int previous, int value)
{
    if (name == "xmas_snow" || name == "xmas_boom" || name == "xmas_song" || name == "xmas_tree") {
        for (DESC* desc : DESC_MANAGER::instance().GetClientSet()) {
            if (CHARACTER* character = desc->GetCharacter())
                character->ChatPacket(5, "%s %d", name.c_str(), value);
        }

        if (name == "xmas_boom" && (previous == 0) != (value == 0))
            UpdateFireworkSellers(value != 0, "ProcessEventFlag", 0x3a);
        if (name == "xmas_tree") {
            if (value > 0 && !previous) {
                CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
                if (characters.GetCharacterSetByRaceNum(20032)->empty())
                    characters.SpawnMob(20032, 61, 0x6a2d4, 0x345e4, 0, false, -1, true);
            } else if (previous > 0 && !value) {
                DestroyRace(20032);
            }
        }
    } else if (name == "xmas_santa") {
        if (value == 1 && map_allow_find(61)) {
            quest::CQuestManager::instance().RequestSetEventFlag("xmas_santa", 2);
            CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
            if (characters.GetCharacterSetByRaceNum(20031)->empty())
                characters.SpawnMobRandomPosition(20031, 61);
        } else if (!value) {
            DestroyRace(20031);
        }
    }
}
}
