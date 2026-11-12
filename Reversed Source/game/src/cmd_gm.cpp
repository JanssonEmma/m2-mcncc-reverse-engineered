#include "fishing.h"
#include "mining.h"
#include "buffer_manager.h"
#include "item_manager.h"
#include "item.h"
#include "skill.h"
#include "regen.h"
#include "affect.h"
#include "vector.h"
#include "log.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "../../libthecore/include/utils.h"
#include "locale.hpp"
#include "p2p.h"
#include <cstdint>
#include "desc_client.h"
#include "db.h"
#include "config.h"
#include "cube.h"
#include "castle.h"
#include "questlua_forked.h"
#include "arena.h"
#include "party.h"
#include "building.h"
#include "guild.h"
#include "guild_manager.h"
#include <cmath>
// Partial cmd_gm recovery: complete original log_packet_func and SendLog only.
#include "char.h"
#include "battle.h"
#include "desc.h"
#include "desc_manager.h"
#include <algorithm>
#include <cstring>
#include <strings.h>
#include <string>
#include <vector>
#include <cctype>
#include <cstdlib>
#include "sectree_manager.h"
#include "sectree.h"
#include "questmanager.h"
#include "questpc.h"
#include "priv_manager.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
namespace
{
bool BuildNumber(const char* argument, std::int64_t& value, std::int64_t low, std::int64_t high)
{
    char* end;
    value = std::strtoll(argument, &end, 10);
    return end != argument && !*end && value >= low && value <= high;
}

bool BuildRotation(const char* argument, float& value)
{
    char* end;
    value = std::strtof(argument, &end);
    return end != argument && !*end && std::isfinite(value) &&
           static_cast<double>(value) >= INT32_MIN && static_cast<double>(value) <= INT32_MAX;
}
}

void do_build(CHARACTER* character, char* argument, int, int)
{
    char command[256];
    char* rest = one_argument(argument, command);
    const bool gm = character->GetGMLevel() != 0;
    const long map = character->GetMapIndex();
    building::CManager& manager = building::CManager::instance();
    building::CLand* land = manager.FindLand(map, character->GetX(), character->GetY());
    if (!land)
    {
        _sys_err("do_build", 0xa32, "%s trying to build on not buildable area.", character->GetName());
        return;
    }
    if (!command[0])
    {
        character->ChatPacket(1, "Invalid syntax: no command");
        return;
    }
    if (!gm)
    {
        CGuild* guild = character->GetGuild();
        if (!guild || guild->GetID() != land->GetData().dwGuildID)
        {
            _sys_err("do_build", 0xa42, "%s trying to build on not owned land.", character->GetName());
            return;
        }
        if (guild->GetMasterPID() != character->GetPlayerID())
        {
            _sys_err("do_build", 0xa49, "%s trying to build while not the guild master.", character->GetName());
            return;
        }
    }

    const auto notice = [character](const char* key) {
        character->ChatPacket(1, "%s", locale_find(key, g_iUseLocale));
    };
    switch (std::tolower(static_cast<unsigned char>(command[0])))
    {
        case 'c': {
            char arg[6][256];
            for (auto& token : arg)
                rest = one_argument(rest, token);
            std::int64_t vnum, x, y;
            float xRot, yRot, zRot;
            if (!BuildNumber(arg[0], vnum, 0, UINT32_MAX) ||
                !BuildNumber(arg[1], x, INT32_MIN, INT32_MAX) ||
                !BuildNumber(arg[2], y, INT32_MIN, INT32_MAX) ||
                !BuildRotation(arg[3], xRot) || !BuildRotation(arg[4], yRot) || !BuildRotation(arg[5], zRot))
            {
                character->ChatPacket(1, "Invalid syntax");
                return;
            }

            const building::TObjectProto* proto = manager.GetObjectProto(vnum);
            if (!proto)
            {
                notice("\301\270\300\347\307\317\301\366 \276\312\264\302 \260\307\271\260\300\324\264\317\264\331.");
                return;
            }
            if (proto->dwGroupVnum && land->FindObjectByGroup(proto->dwGroupVnum))
            {
                notice("\260\260\300\314 \301\366\300\273 \274\366 \276\370\264\302 \301\276\267\371\300\307 \260\307\271\260\300\314 \301\366\276\356\301\256 \300\326\275\300\264\317\264\331.");
                return;
            }
            if (proto->dwDependOnGroupVnum && !land->FindObjectByGroup(proto->dwDependOnGroupVnum))
            {
                notice("\260\307\274\263\277\241 \307\312\277\344\307\321 \260\307\271\260\300\314 \301\366\276\356\301\256 \300\326\301\366 \276\312\275\300\264\317\264\331.");
                return;
            }

            const bool pay = test_server || !gm;
            if (pay)
            {
                if (proto->dwPrice > 100000000)
                {
                    notice("\260\307\271\260 \272\361\277\353 \301\244\272\270 \300\314\273\363\300\270\267\316 \260\307\274\263 \300\333\276\367\277\241 \275\307\306\320\307\337\275\300\264\317\264\331.");
                    return;
                }
                if (character->GetGold() < static_cast<int>(proto->dwPrice))
                {
                    notice("\260\307\274\263 \272\361\277\353\300\314 \272\316\301\267\307\325\264\317\264\331.");
                    return;
                }
                const auto& materials = proto->kMaterials;
                for (unsigned index = 0; index < 5 && materials[index].dwItemVnum; ++index)
                {
                    const DWORD material = materials[index].dwItemVnum;
                    std::uint64_t required = materials[index].dwCount;
                    for (unsigned previous = 0; previous < index; ++previous)
                    {
                        if (materials[previous].dwItemVnum == material)
                            required += materials[previous].dwCount;
                    }
                    const int available = character->CountSpecifyItem(material);
                    if (available < 0 || static_cast<std::uint64_t>(available) < required)
                    {
                        notice("\300\332\300\347\260\241 \272\316\301\267\307\317\277\251 \260\307\274\263\307\322 \274\366 \276\370\275\300\264\317\264\331.");
                        return;
                    }
                }
            }

            const TMapRegion* region = SECTREE_MANAGER::instance().GetMapRegion(map);
            if (!region || x + region->sx < INT32_MIN || x + region->sx > INT32_MAX ||
                y + region->sy < INT32_MIN || y + region->sy > INT32_MAX)
                return;
            if (!land->RequestCreateObject(vnum, map, static_cast<long>(x), static_cast<long>(y),
                                           xRot, yRot, zRot, true))
            {
                if (test_server)
                    notice("\260\307\271\260\300\273 \301\366\300\273 \274\366 \276\370\264\302 \300\247\304\241\300\324\264\317\264\331.");
                return;
            }
            if (!pay)
                return;
            character->PointChange(POINT_GOLD, -static_cast<int>(proto->dwPrice), false, false);
            unsigned index = 0;
            for (const auto& material : proto->kMaterials)
            {
                if (!material.dwItemVnum)
                    break;
                sys_log(0, "BUILD: material %d %lu %lu", static_cast<int>(index++), material.dwItemVnum, material.dwCount);
                character->RemoveSpecifyItem(material.dwItemVnum, material.dwCount);
            }
            return;
        }
        case 'd': {
            char arg[256];
            one_argument(rest, arg);
            std::int64_t vid;
            if (!BuildNumber(arg, vid, 0, UINT32_MAX))
                character->ChatPacket(1, "Invalid syntax");
            else
                land->RequestDeleteObjectByVID(vid);
            return;
        }
        case 'r': {
            char arg[256];
            one_argument(rest, arg);
            std::int64_t id;
            if (BuildNumber(arg, id, 0, UINT32_MAX) && (gm || id == land->GetData().dwID))
                manager.ClearLand(id);
            return;
        }
        case 'e': {
            if (!gm)
                return;
            char arg[256];
            one_argument(rest, arg);
            std::int64_t id;
            if (BuildNumber(arg, id, 4, UINT32_MAX))
                land->RequestDeleteWallBlocks(id);
            return;
        }
        case 's': {
            if (!gm)
                return;
            char arg[6][256];
            for (auto& token : arg)
                rest = one_argument(rest, token);
            std::int64_t vnum, size;
            if (!BuildNumber(arg[0], vnum, 0, UINT32_MAX) ||
                (vnum != 14105 && vnum != 14115 && vnum != 14125))
            {
                sys_log(0, "BUILD_WALL: wrong wall set id %s", arg[0]);
                return;
            }
            if (BuildNumber(arg[1], size, 0, 127))
                land->RequestCreateWallBlocks(vnum, map, static_cast<char>(size),
                    std::strtol(arg[2], nullptr, 10) != 0, std::strtol(arg[3], nullptr, 10) != 0,
                    std::strtol(arg[5], nullptr, 10) != 0, std::strtol(arg[4], nullptr, 10) != 0);
            return;
        }
        case 'f':
            if (gm)
                land->RequestDeleteWall();
            return;
        case 'w': {
            if (!gm)
                return;
            char arg[256];
            one_argument(rest, arg);
            sys_log(0, "guild.wall.build map[%d] direction[%s]", static_cast<int>(map), arg);
            float rotation;
            switch (arg[0])
            {
                case 'n': rotation = 180; break;
                case 'e': rotation = 90; break;
                case 's': rotation = 0; break;
                case 'w': rotation = 270; break;
                default:
                    character->ChatPacket(1, "guild.wall.build unknown_direction[%s]", arg);
                    _sys_err("do_build", 0xb08, "guild.wall.build unknown_direction[%s]", arg);
                    return;
            }
            land->RequestCreateWall(map, rotation);
            return;
        }
        default:
            character->ChatPacket(1, "Invalid command %s", command);
            return;
    }
}

void do_show_arena_list(CHARACTER* character, char*, int, int)
{
    CArenaManager::instance().SendArenaMapListTo(character);
}

void do_end_all_duel(CHARACTER*, char*, int, int)
{
    CArenaManager::instance().EndAllDuel();
}

void do_end_duel(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    CHARACTER* player = CHARACTER_MANAGER::instance().FindPC(name);
    const char* message = !player ? "\301\270\300\347\307\317\301\366 \276\312\264\302 \304\263\270\257\305\315 \300\324\264\317\264\331." :
        (CArenaManager::instance().EndDuel(player->GetPlayerID()) ? "\264\353\267\303 \260\255\301\246 \301\276\267\341 \274\272\260\370" : "\264\353\267\303 \260\255\301\246 \301\276\267\341 \275\307\306\320");
    character->ChatPacket(1, "%s", LC_TEXT(message));
}

void do_duel(CHARACTER* character, char* argument, int, int)
{
    char firstName[256], secondName[256], setArg[256];
    one_argument(two_arguments(argument, firstName, secondName), setArg);
    const long requested = std::strtol(setArg, nullptr, 10);
    const int sets = requested < 0 ? 1 : static_cast<int>(std::min(requested, 5L));

    CHARACTER_MANAGER& characters = CHARACTER_MANAGER::instance();
    CHARACTER* first = characters.FindPC(firstName);
    CHARACTER* second = characters.FindPC(secondName);
    if (!first || !second || first == second)
    {
        character->ChatPacket(1, "%s", LC_TEXT("\264\353\267\303\300\332\260\241 \276\370\275\300\264\317\264\331."));
        return;
    }

    first->RemoveGoodAffect();
    second->RemoveGoodAffect();
    first->RemoveBadAffect();
    second->RemoveBadAffect();
    for (CHARACTER* player : {first, second})
    {
        if (CParty* party = player->GetParty())
        {
            if (party->GetMemberCount() == 2)
                CPartyManager::instance().DeleteParty(party);
            else
            {
                player->ChatPacket(1, "%s", LC_TEXT("<\306\304\306\274> \306\304\306\274\277\241\274\255 \263\252\260\241\274\314\275\300\264\317\264\331."));
                party->Quit(player->GetPlayerID());
            }
        }
    }

    const bool started = CArenaManager::instance().StartDuel(first, second, sets);
    character->ChatPacket(1, "%s", LC_TEXT(started ? "\264\353\267\303\300\314 \274\272\260\370\300\373\300\270\267\316 \275\303\300\333 \265\307\276\372\275\300\264\317\264\331." : "\264\353\267\303 \275\303\300\333\277\241 \271\256\301\246\260\241 \300\326\275\300\264\317\264\331."));
}

void do_horse_state(CHARACTER* character, char*, int, int)
{
    const int health = character->GetHorseHealth();
    const int maxHealth = character->GetHorseMaxHealth();
    const int stamina = character->GetHorseStamina();
    const int maxStamina = character->GetHorseMaxStamina();
    character->ChatPacket(1, "Horse Information:");
    character->ChatPacket(1, "    Level  %d", character->GetHorseLevel());
    character->ChatPacket(1, "    Health %d/%d (%d%%)", health, maxHealth,
                          maxHealth ? health * 100 / maxHealth : 0);
    character->ChatPacket(1, "    Stam   %d/%d (%d%%)", stamina, maxStamina,
                          maxStamina ? stamina * 100 / maxStamina : 0);
}

void do_horse_level(CHARACTER* character, char* argument, int, int)
{
    char name[256], value[256];
    two_arguments(argument, name, value);
    if (!name[0] || !value[0])
    {
        character->ChatPacket(1, "usage : /horse_level <name> <level>");
        return;
    }
    CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name);
    if (!target)
    {
        character->ChatPacket(1, "%s", LC_TEXT("\301\270\300\347\307\317\301\366 \276\312\264\302 \304\263\270\257\305\315 \300\324\264\317\264\331."));
        return;
    }
    const long requested = std::strtol(value, nullptr, 10);
    const int level = std::clamp(requested, 0L, static_cast<long>(HORSE_MAX_LEVEL));
    character->ChatPacket(1, "horse level set (%s: %d)", target->GetName(), level);
    target->SetHorseLevel(level);
    target->ComputePoints();
    target->SkillLevelPacket();
}

void do_horse_ride(CHARACTER* character, char*, int, int)
{
    if (character->IsHorseRiding())
        character->StopRiding();
    else
        character->StartRiding();
}

void do_horse_summon(CHARACTER* character, char*, int, int)
{
    character->HorseSummon(true, true);
}

void do_horse_unsummon(CHARACTER* character, char*, int, int)
{
    character->HorseSummon(false, true);
}

void do_horse_set_stat(CHARACTER* character, char* argument, int, int)
{
    char hp[256], sp[256];
    two_arguments(argument, hp, sp);
    if (!hp[0] || !sp[0])
    {
        character->ChatPacket(1, "Usage : /horse_set_stat <hp> <stamina>");
        return;
    }
    const int health = std::clamp(std::strtol(hp, nullptr, 10), 0L,
                                  static_cast<long>(character->GetHorseMaxHealth()));
    const int stamina = std::clamp(std::strtol(sp, nullptr, 10), 0L,
                                   static_cast<long>(character->GetHorseMaxStamina()));
    character->UpdateHorseHealth(health - character->GetHorseHealth(), true);
    character->UpdateHorseStamina(stamina - character->GetHorseStamina(), true);
}

void PrintItemLog();

void do_itemlog(CHARACTER* character, char*, int, int)
{
    PrintItemLog();
    character->ChatPacket(1, "%s", LC_TEXT("\276\306\300\314\305\333\267\316\261\327\260\241 syslog \277\241 \263\262\276\322\275\300\264\317\264\331"));
}

namespace
{
bool ReadGuildWarIds(char* argument, DWORD& first, DWORD& second)
{
    char firstArg[256], secondArg[256];
    two_arguments(argument, firstArg, secondArg);
    char* firstEnd;
    char* secondEnd;
    const auto id1 = std::strtoll(firstArg, &firstEnd, 10);
    const auto id2 = std::strtoll(secondArg, &secondEnd, 10);
    if (*firstEnd || *secondEnd || id1 <= 0 || id2 <= 0 || id1 > INT32_MAX || id2 > INT32_MAX)
        return false;
    first = static_cast<DWORD>(std::min(id1, id2));
    second = static_cast<DWORD>(std::max(id1, id2));
    return true;
}
}

void do_gwlist(CHARACTER* character, char*, int, int)
{
    character->ChatPacket(2, "%s", LC_TEXT("\307\366\300\347 \300\374\300\357\301\337\300\316 \261\346\265\345 \300\324\264\317\264\331"));
    CGuildManager::instance().ShowGuildWarList(character);
}

void do_stop_guild_war(CHARACTER* character, char* argument, int, int)
{
    DWORD first, second;
    if (!ReadGuildWarIds(argument, first, second))
        return;
    character->ChatPacket(0, "%d %d", static_cast<int>(first), static_cast<int>(second));
    CGuildManager::instance().RequestEndWar(first, second);
}

void do_cancel_guild_war(CHARACTER*, char* argument, int, int)
{
    DWORD first, second;
    if (ReadGuildWarIds(argument, first, second))
        CGuildManager::instance().RequestCancelWar(first, second);
}

void do_guild_state(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    CGuild* guild = CGuildManager::instance().FindGuildByName(name);
    if (!guild)
    {
        character->ChatPacket(1, "%s", LC_TEXT("%s: \301\270\300\347\307\317\301\366 \276\312\264\302 \261\346\265\345 \300\324\264\317\264\331."));
        return;
    }
    character->ChatPacket(1, "GuildID: %d", static_cast<int>(guild->GetID()));
    character->ChatPacket(1, "GuildMasterPID: %d", static_cast<int>(guild->GetMasterPID()));
    character->ChatPacket(1, "IsInWar: %d", static_cast<int>(guild->UnderAnyWar(3)));
}

namespace
{
void SetAdminQuestState(CHARACTER* character, quest::PC& player, const std::string& questName,
                        const char* stateName, const char* reportedArgument)
{
    if (questName.empty())
    {
        character->ChatPacket(1, "setting quest state flag failed");
        return;
    }
    auto& manager = quest::CQuestManager::instance();
    const int state = manager.GetQuestStateIndex(questName, stateName);
    player.SetFlag(questName + ".__status", state, false);
    player.ClearTimer();
    const auto current = player.quest_find(manager.GetQuestIndexByName(questName));
    if (current != player.quest_end())
        current->second.st = state;
    character->ChatPacket(1, "setting quest state flag %s %s %d",
                          questName.c_str(), reportedArgument, state);
}
}

void do_qf(CHARACTER* character, char* argument, int, int)
{
    char state[256];
    one_argument(argument, state);
    if (!state[0])
        return;
    quest::PC& player = *quest::CQuestManager::instance().GetPCForce(character->GetPlayerID());
    const std::string questName = player.GetCurrentQuestName();
    SetAdminQuestState(character, player, questName, state, state);
}

void do_set_state(CHARACTER* character, char* argument, int, int)
{
    char questName[256], state[256];
    two_arguments(argument, questName, state);
    if (!questName[0] || !state[0])
        return;
    quest::PC& player = *quest::CQuestManager::instance().GetPCForce(character->GetPlayerID());
    SetAdminQuestState(character, player, questName, state, questName);
}

void do_clear_quest(CHARACTER* character, char* argument, int, int)
{
    char questName[256];
    one_argument(argument, questName);
    if (questName[0])
        quest::CQuestManager::instance().GetPCForce(character->GetPlayerID())->ClearQuest(questName);
}

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

void do_priv_empire(CHARACTER* character, char* argument, int, int)
{
    char empireArg[256], typeArg[256], valueArg[256], durationArg[256];
    two_arguments(two_arguments(argument, empireArg, typeArg), valueArg, durationArg);
    if (empireArg[0] && typeArg[0] && valueArg[0] && durationArg[0])
    {
        const long empire = std::strtol(empireArg, nullptr, 10);
        const long type = std::strtol(typeArg, nullptr, 10);
        const int value = std::clamp(std::strtol(valueArg, nullptr, 10), 0L, 1000L);
        const long hours = std::strtol(durationArg, nullptr, 10);
        if (empire >= 0 && empire < 4 && type >= 1 && type <= 4 && hours >= 0 && hours <= INT32_MAX / 3600)
        {
            const int seconds = hours * 3600;
            sys_log(0, "_give_empire_privileage(empire=%d, type=%d, value=%d, duration=%d",
                    static_cast<int>(empire), static_cast<int>(type), value, seconds);
            CPrivManager::instance().RequestGiveEmpirePriv(empire, type, value, seconds);
            return;
        }
    }
    character->ChatPacket(1, "usage : priv_empire <empire> <type> <value> <duration>");
    character->ChatPacket(1, "  <empire>    0 - 3 (0==all)");
    character->ChatPacket(1, "  <type>      1:item_drop, 2:gold_drop, 3:gold10_drop, 4:exp");
    character->ChatPacket(1, "  <value>     percent");
    character->ChatPacket(1, "  <duration>  hour");
}

void do_priv_guild(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    if (!name[0])
        return;
    CGuildManager& guilds = CGuildManager::instance();
    CGuild* guild = guilds.FindGuildByName(name);
    if (!guild)
        guild = guilds.FindGuild(static_cast<DWORD>(std::strtoul(name, nullptr, 10)));
    if (!guild)
    {
        character->ChatPacket(1, "%s", LC_TEXT("\261\327\267\261 \300\314\270\247 \266\307\264\302 \271\370\310\243\300\307 \261\346\265\345\260\241 \276\370\275\300\264\317\264\331."));
        return;
    }

    // This build's ADMIN_QUEST buffer for the guild privilege command is empty.
    auto& manager = quest::CQuestManager::instance();
    quest::PC* player = manager.GetPC(character->GetPlayerID());
    if (!player || !manager.GetLuaState())
        return;
    quest::QuestState state = manager.OpenState("ADMIN_QUEST", -2);
    if (luaL_loadbuffer(state.co, "", 0, "ADMIN_QUEST") != 0)
    {
        manager.CloseState(state);
        return;
    }
    player->SetQuest("ADMIN_QUEST", state);
    quest::QuestState& running = *player->GetRunningQuestState();
    if (!manager.RunState(running))
    {
        manager.CloseState(running);
        player->EndRunning();
    }
}

namespace
{
int ParseBlockDuration(const char* text)
{
    std::int64_t total = 0;
    std::int64_t value = 0;
    for (; *text; ++text)
    {
        if (*text >= '0' && *text <= '9')
        {
            value = value * 10 + (*text - '0');
            if (value > INT32_MAX)
                return -1;
            continue;
        }
        switch (*text)
        {
            case 'd': case 'D': value *= 86400; break;
            case 'h': case 'H': value *= 3600; break;
            case 'm': case 'M': value *= 60; break;
            case 's': case 'S': break;
            default: return -1;
        }
        total += value;
        if (total > INT32_MAX)
            return -1;
        value = 0;
    }
    return total + value <= INT32_MAX ? static_cast<int>(total + value) : -1;
}
}

void do_block_chat(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    const char* time = one_argument(argument, name);
    if (!name[0])
    {
        if (character)
            character->ChatPacket(1, "Usage: block_chat <name> <time> (0 to off)");
        return;
    }

    const std::int32_t duration = ParseBlockDuration(time);
    if (duration < 0)
    {
        if (character)
        {
            character->ChatPacket(1, "%s", "\300\337\270\370\265\310 \307\374\275\304\300\307 \275\303\260\243\300\324\264\317\264\331. h, m, s\270\246 \272\331\277\251\274\255 \301\366\301\244\307\330 \301\326\275\312\275\303\277\300.");
            character->ChatPacket(1, "%s", "\277\271) 10s, 10m, 1m 30s");
        }
        return;
    }

    if (CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name))
    {
        if (target == character)
            character->ChatPacket(1, "cannot block chatting myself");
        else
            target->AddAffect(223, 0, 0, 0, duration, 0, true, false);
        return;
    }

    BYTE packet[30] = {22};
    std::strncpy(reinterpret_cast<char*>(packet + 1), name, 24);
    std::memcpy(packet + 26, &duration, sizeof(duration));
    P2P_MANAGER& peers = P2P_MANAGER::instance();
    if (!peers.Find(name))
    {
        DESC* desc = character ? character->GetDesc() : nullptr;
        db_clientdesc->DBPacket(110, desc ? desc->GetHandle() : 0, packet + 1, 29);
        return;
    }

    peers.Send(packet, sizeof(packet));
    if (character)
        character->ChatPacket(1, "Block chat requested.");
}

void do_block_chat_list(CHARACTER* character, char*, int, int)
{
    DBManager::instance().ReturnQuery(10, character->GetPlayerID(), nullptr,
        "SELECT p.name, a.lDuration FROM affect%s as a, player%s as p WHERE a.bType = %d AND a.dwPID = p.id",
        get_table_postfix(), get_table_postfix(), 223);
}

namespace
{
CItem* GetToolCommandItem(CHARACTER* character, char* argument)
{
    char cellArg[256];
    one_argument(argument, cellArg);
    const long cell = std::strtol(cellArg, nullptr, 10);
    return cell >= 0 && cell < 90 ? character->GetItem(static_cast<BYTE>(cell)) : nullptr;
}
}

void do_refine_rod(CHARACTER* character, char* argument, int, int)
{
    if (CItem* item = GetToolCommandItem(character, argument))
        fishing::RealRefineRod(character, item);
}

void do_refine_pick(CHARACTER* character, char* argument, int, int)
{
    if (CItem* item = GetToolCommandItem(character, argument))
    {
        mining::CHEAT_MAX_PICK(character, item);
        mining::RealRefinePick(character, item);
    }
}

void do_max_pick(CHARACTER* character, char* argument, int, int)
{
    if (CItem* item = GetToolCommandItem(character, argument))
        mining::CHEAT_MAX_PICK(character, item);
}

void do_fishing_simul(CHARACTER* character, char* argument, int, int)
{
    char levelArg[256], probabilityArg[256], countArg[256];
    two_arguments(one_argument(argument, levelArg), probabilityArg, countArg);
    character->ChatPacket(1, "Usage: fishing_simul <level> <prob index> <count>");
    const int level = levelArg[0] ? std::clamp(std::strtoll(levelArg, nullptr, 10),
                                               static_cast<long long>(INT32_MIN), static_cast<long long>(INT32_MAX)) : 100;
    const long probability = probabilityArg[0] ? std::strtol(probabilityArg, nullptr, 10) : 0;
    const int count = countArg[0] ? std::clamp(std::strtol(countArg, nullptr, 10), 0L, 1000000L) : 1000;
    if (probability >= 0 && probability < 4)
        fishing::Simulation(level, count, static_cast<int>(probability), character);
}

void do_forgetme(CHARACTER* character, char*, int, int)
{
    character->ForgetMyAttacker();
}

void do_aggregate(CHARACTER* character, char*, int, int)
{
    character->AggregateMonster();
}

void do_attract_ranger(CHARACTER* character, char*, int, int)
{
    character->AttractRanger();
}

void do_pull_monster(CHARACTER* character, char*, int, int)
{
    character->PullMonster();
}

void do_observer(CHARACTER* character, char*, int, int)
{
    character->SetObserverMode(!character->IsObserverMode());
}

void do_effect(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    character->EffectPacket(std::atoi(arg));
}

void Command_ApplyAffect(CHARACTER* character, char* argument, const char* name, int commandAffect)
{
    char targetName[256];
    one_argument(argument, targetName);
    sys_log(0, "%s", targetName);
    if (!targetName[0])
    {
        character->ChatPacket(1, "Usage: %s <name>", name);
        return;
    }
    CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(targetName);
    if (!target)
    {
        character->ChatPacket(1, "%s is not in same map", targetName);
        return;
    }
    if (commandAffect == 0)
        SkillAttackAffect(target, 1000, 1, 210, 0, 0, 6, 30, "GM_STUN");
    else if (commandAffect == 1)
        SkillAttackAffect(target, 1000, 2, 211, POINT_MOV_SPEED, -30, 5, 30, "GM_SLOW");
    sys_log(0, "%s %s", targetName, name);
    character->ChatPacket(1, "%s %s", targetName, name);
}

void do_stun(CHARACTER* character, char* argument, int, int)
{
    Command_ApplyAffect(character, argument, "stun", 0);
}

void do_slow(CHARACTER* character, char* argument, int, int)
{
    Command_ApplyAffect(character, argument, "slow", 1);
}

void do_guildlevel(CHARACTER* character, char* argument, int, int)
{
    char level[256];
    one_argument(argument, level);
    if (!character->GetGuild()) {
        character->ChatPacket(1, "%s", LC_TEXT("<\261\346\265\345> \261\346\265\345\277\241 \274\323\307\330\300\326\301\366 \276\312\275\300\264\317\264\331."));
    } else if (level[0]) {
        (void)std::atoi(level);
    }
}

namespace {
int GMTargetDistance(CHARACTER& target, CHARACTER& origin)
{
    const auto dy = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(target.GetY()) - static_cast<std::uint32_t>(origin.GetY()));
    const auto dx = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(target.GetX()) - static_cast<std::uint32_t>(origin.GetX()));
    return DISTANCE_APPROX(dx, dy);
}
}

void do_weaken(CHARACTER* character, char* argument, int, int)
{
    SECTREE* tree = character->GetSectree();
    if (!tree)
        return;
    char arg[256];
    one_argument(argument, arg);
    const bool all = std::strcmp(arg, "all") == 0;
    auto weaken = [character, all](LPENTITY entity) {
        if (!entity->IsType(0))
            return;
        auto* target = static_cast<CHARACTER*>(entity);
        if (target->GetCharType() != CHAR_TYPE_PC && (all || GMTargetDistance(*target, *character) < 1000)) {
            const auto delta = static_cast<std::int32_t>(10U - static_cast<std::uint32_t>(target->GetHP()));
            target->PointChange(POINT_HP, delta, false, false);
        }
    };
    tree->ForEachAround(weaken);
}

void do_state(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    CHARACTER* target = character;
    if (arg[0] == '#')
        target = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(std::strtoul(arg + 1, nullptr, 10)));
    else if (arg[0]) {
        DESC* desc = DESC_MANAGER::instance().FindByCharacterName(arg);
        target = desc ? desc->GetCharacter() : nullptr;
    }
    if (!target)
        return;

    const int position = target->GetPosition();
    const char* state = position == POS_FIGHTING ? "in battle" : position == POS_DEAD ? "in dead" : "standing";
    character->ChatPacket(1, "%s state: %s%s%s", target->GetName(), state,
                          character->GetShop() ? ", shop" : "", character->GetExchange() ? ", exchange" : "");
    const auto reportY = static_cast<std::int32_t>(target->GetY());
    const auto reportZ = static_cast<std::int32_t>(target->GetZ());
    const auto reportX = static_cast<std::int32_t>(target->GetX());
    character->ChatPacket(1, "Coordinate %d x %d x %d", reportX, reportY, reportZ);
    character->ChatPacket(1, "Level      %d", target->GetLevel());

    const auto map = static_cast<std::uint32_t>(target->GetMapIndex());
    const auto y = static_cast<std::uint32_t>(target->GetY());
    const auto x = static_cast<std::uint32_t>(target->GetX());
    if (SECTREE* tree = SECTREE_MANAGER::instance().Get(map, x, y))
    {
        character->ChatPacket(1, "MapIndex %d", static_cast<std::int32_t>(target->GetMapIndex()));
        const auto attributeX = static_cast<std::int32_t>(target->GetX());
        const auto attributeY = static_cast<std::int32_t>(target->GetY());
        character->ChatPacket(1, "MapAttribute %08X",
            static_cast<unsigned>(tree->GetAttribute(attributeX, attributeY)));
    }

    CPrivManager& privileges = CPrivManager::instance();
    for (BYTE type = 0; type < 5; ++type)
    {
        if (!privileges.GetPriv(target, type))
            continue;
        const int empire = privileges.GetPrivByEmpire(target->GetEmpire(), type);
        const int guild = target->GetGuild() ? privileges.GetPrivByGuild(target->GetGuild()->GetID(), type) : 0;
        const int player = privileges.GetPrivByCharacter(target->GetPlayerID(), type);
        if (empire)
            character->ChatPacket(1, "%s for empire : %d", GetPrivName(type), empire);
        if (guild)
            character->ChatPacket(1, "%s for guild : %d", GetPrivName(type), guild);
        if (player)
            character->ChatPacket(1, "%s for player : %d", GetPrivName(type), player);
    }
}

// 080c4f60
void do_xmas(CHARACTER*, char* argument, int, int subcommand)
{
    static const char* const flags[] = {"xmas_boom", "xmas_snow", "xmas_santa"};
    if (subcommand < 0 || subcommand >= 3)
        return;

    char value[256];
    one_argument(argument, value);
    quest::CQuestManager::instance().RequestSetEventFlag(flags[subcommand], std::atoi(value));
}

void do_makeguild(CHARACTER* character, char* argument, int, int)
{
    if (character->GetGuild())
        return;
    char name[256];
    one_argument(argument, name);
    TGuildCreateParameter parameter = {};
    parameter.master = character;
    std::strncpy(parameter.name, name, sizeof(parameter.name) - 1);
    if (!check_name(parameter.name))
    {
        character->ChatPacket(1, "%s", LC_TEXT("\300\373\307\325\307\317\301\366 \276\312\300\272 \261\346\265\345 \300\314\270\247 \300\324\264\317\264\331."));
        return;
    }
    CGuildManager::instance().CreateGuild(parameter);
    character->ChatPacket(1, LC_TEXT("(%s) \261\346\265\345\260\241 \273\375\274\272\265\307\276\372\275\300\264\317\264\331. [\300\323\275\303]"), parameter.name);
}

void do_deleteguild(CHARACTER* character, char*, int, int)
{
    if (CGuild* guild = character->GetGuild())
        guild->RequestDisband(character->GetPlayerID());
}

void do_safebox_size(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    const long requested = std::strtol(arg, nullptr, 10);
    const BYTE size = requested >= 0 && requested < 4 ? static_cast<BYTE>(requested) : 0;
    character->ChatPacket(1, "Safebox size set to %d", size);
    character->ChangeSafeboxSize(size);
}

void do_break_marriage(CHARACTER* character, char* argument, int, int)
{
    char first[256], second[256];
    two_arguments(argument, first, second);
    char* endFirst;
    char* endSecond;
    const long long firstId = std::strtoll(first, &endFirst, 10);
    const long long secondId = std::strtoll(second, &endSecond, 10);
    if (*endFirst || *endSecond || firstId <= 0 || secondId <= 0 || firstId > UINT32_MAX || secondId > UINT32_MAX)
        return;
    const std::uint32_t packet[] = {static_cast<std::uint32_t>(firstId), static_cast<std::uint32_t>(secondId)};
    character->ChatPacket(1, LC_TEXT("\307\303\267\271\300\314\276\356 %d \277\315 \307\303\267\271\300\314\276\356  %d\270\246 \306\304\310\245\275\303\305\265\264\317\264\331.."), static_cast<int>(packet[0]), static_cast<int>(packet[1]));
    db_clientdesc->DBPacket(116, 0, packet, sizeof(packet));
}

void do_reset(CHARACTER* character, char*, int, int)
{
    character->PointChange(POINT_HP, character->GetMaxHP() - character->GetHP(), false, false);
    character->PointChange(POINT_SP, character->GetMaxSP() - character->GetSP(), false, false);
    character->Save();
}

void do_greset(CHARACTER* character, char*, int, int)
{
    if (CGuild* guild = character->GetGuild())
        guild->RestoreGuildPower();
}

void do_advance(CHARACTER* character, char* argument, int, int)
{
    char name[256], level[256];
    two_arguments(argument, name, level);
    if (!name[0] || !level[0])
    {
        character->ChatPacket(1, "Syntax: advance <name> <level>");
        return;
    }
    CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name);
    if (!target)
    {
        character->ChatPacket(1, "%s not exist", name);
        return;
    }
    target->ResetPoint(MINMAX(0, std::atoi(level), 99));
}

// Dormant building-effect tests at 080c1d00–080c2ca2.
LPCHARACTER chHori, chForge, chLib, chTemple, chTraining, chTree, chPortal, chBall;

namespace {
constexpr DWORD buildingTestAffect = 213;
constexpr DWORD buildingTestUnique = 8;
constexpr DWORD buildingTestConstruction = 9;
constexpr DWORD buildingTestUpgrade = 11;
constexpr int buildingTestLibraryX = 307900;
constexpr int buildingTestLibraryY = 744500;
DWORD buildingTestHoriVID = 0;
DWORD buildingTestLibraryVID = 0;

void ApplyBuildingTestAffect(CHARACTER* target, DWORD flag)
{
    if (target)
        target->AddAffect(buildingTestAffect, POINT_NONE, 0, flag, 65535, 0, true, false);
}

LPCHARACTER SpawnBuildingTest(CHARACTER* character, DWORD vnum, int x, int y)
{
    return CHARACTER_MANAGER::instance().SpawnMobRange(
        vnum, character->GetMapIndex(), x, y, x, y, true, false, false);
}

void SpawnBuildingTestAttackers(CHARACTER* character, int x, int y, float distance,
                                int groupLine, int vnumLine, int stoneLine)
{
    char source[] = "cmd_gm.cpp";
    for (int i = 0; i < 35; ++i) {
        const bool stone = i >= 30;
        const int rotation = number_ex(0, 359, source, stone ? stoneLine : groupLine);
        float dx, dy;
        GetDeltaByDegree(rotation, distance, &dx, &dy);
        const DWORD vnum = stone ? 8009 : number_ex(701, 706, source, vnumLine);
        if (CHARACTER* attacker = SpawnBuildingTest(
                character, vnum, x + static_cast<int>(dx), y + static_cast<int>(dy)))
            attacker->SetAggressive();
    }
}

void SetBuildingTestLibrary(CHARACTER* character, DWORD vnum)
{
    chLib = SpawnBuildingTest(character, vnum, buildingTestLibraryX, buildingTestLibraryY);
    buildingTestLibraryVID = chLib ? static_cast<DWORD>(chLib->GetVID()) : 0;
    ApplyBuildingTestAffect(chLib, buildingTestUnique);
}

void ReplaceBuildingTestLibrary(CHARACTER* character, DWORD vnum)
{
    auto& manager = CHARACTER_MANAGER::instance();
    if (CHARACTER* library = manager.Find(buildingTestLibraryVID))
        manager.DestroyCharacter(library);
    SetBuildingTestLibrary(character, vnum);
}
}

void do_b1(CHARACTER* character, char*, int, int)
{
    chHori = SpawnBuildingTest(character, 14017, 304222, 742858);
    buildingTestHoriVID = chHori ? static_cast<DWORD>(chHori->GetVID()) : 0;
    ApplyBuildingTestAffect(chHori, buildingTestConstruction);
    ApplyBuildingTestAffect(chHori, buildingTestUnique);
    SpawnBuildingTestAttackers(character, 304222, 742858, 800, 0x614, 0x618, 0x624);
}

void do_b2(CHARACTER*, char*, int, int)
{
    if (CHARACTER* target = CHARACTER_MANAGER::instance().Find(buildingTestHoriVID))
        target->RemoveAffect(buildingTestAffect);
}

void do_b3(CHARACTER* character, char*, int, int)
{
    chForge = SpawnBuildingTest(character, 14003, 307500, 746300);
    ApplyBuildingTestAffect(chForge, buildingTestUnique);
    SetBuildingTestLibrary(character, 14007);
    chTemple = SpawnBuildingTest(character, 14004, 307700, 741600);
    ApplyBuildingTestAffect(chTemple, buildingTestUnique);
    chTraining = SpawnBuildingTest(character, 14010, 307100, 739500);
    ApplyBuildingTestAffect(chTraining, buildingTestUnique);
    chTree = SpawnBuildingTest(character, 14013, 300800, 741600);
    ApplyBuildingTestAffect(chTree, buildingTestUnique);
    chPortal = SpawnBuildingTest(character, 14001, 300900, 744500);
    ApplyBuildingTestAffect(chPortal, buildingTestUnique);
    chBall = SpawnBuildingTest(character, 14012, 302500, 746600);
    ApplyBuildingTestAffect(chBall, buildingTestUnique);
}

void do_b4(CHARACTER* character, char*, int, int)
{
    ApplyBuildingTestAffect(CHARACTER_MANAGER::instance().Find(buildingTestLibraryVID),
                            buildingTestUpgrade);
    SpawnBuildingTestAttackers(character, buildingTestLibraryX, buildingTestLibraryY,
                               1200, 0x65b, 0x65f, 0x66b);
}

void do_b5(CHARACTER* character, char*, int, int)
{
    ReplaceBuildingTestLibrary(character, 14008);
}

void do_b6(CHARACTER*, char*, int, int)
{
    ApplyBuildingTestAffect(CHARACTER_MANAGER::instance().Find(buildingTestLibraryVID),
                            buildingTestUpgrade);
}

void do_b7(CHARACTER* character, char*, int, int)
{
    ReplaceBuildingTestLibrary(character, 14009);
}

void do_book(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    CSkillManager& manager = CSkillManager::instance();
    const CSkillProto* skill = std::isdigit(static_cast<unsigned char>(arg[0]))
        ? manager.Get(static_cast<DWORD>(std::strtoul(arg, nullptr, 10))) : manager.Get(arg);
    if (!skill)
    {
        character->ChatPacket(1, "There is no such a skill.");
        return;
    }
    if (CItem* item = character->AutoGiveItem(50300, 1, -1, true))
        item->SetSocket(0, skill->dwVnum);
}

void do_respawn(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (strcasecmp(arg, "all") == 0)
    {
        character->ChatPacket(1, "Respaw everywhere");
        regen_reset(0, 0);
    }
    else
    {
        character->ChatPacket(1, "Respaw around");
        regen_reset(character->GetX(), character->GetY());
    }
}

void do_socket_item(CHARACTER* character, char* argument, int, int)
{
    char name[256], sockets[256];
    two_arguments(argument, name, sockets);
    if (!name[0])
        return;
    DWORD vnum = static_cast<DWORD>(std::strtoul(name, nullptr, 10));
    if (!vnum && !ITEM_MANAGER::instance().GetVnum(name, vnum))
    {
        character->ChatPacket(1, "#%d item not exist by that vnum.", static_cast<int>(vnum));
        return;
    }
    CItem* item = character->AutoGiveItem(vnum, 1, -1, true);
    if (!item)
    {
        character->ChatPacket(1, "#%d cannot create item.", static_cast<int>(vnum));
        return;
    }
    const long requested = std::strtol(sockets, nullptr, 10);
    const int count = requested == 0 || requested > 2 ? 3 : static_cast<int>(std::max(requested, 0L));
    for (int index = 0; index < count; ++index)
        item->SetSocket(index, 1);
}

void do_threeway_war_info(CHARACTER* character, char*, int, int)
{
    const int map = GetSungziMapIndex();
    character->ChatPacket(1, "%s", LC_TEXT("\260\242\301\246\261\271 \301\370\307\340 \301\244\272\270"));
    character->ChatPacket(1, LC_TEXT("\274\261\305\303 \270\312 \301\244\272\270 \274\272\301\366 %d \305\353\267\316 %d %d %d"), map, GetPassMapIndex(1), GetPassMapIndex(2), GetPassMapIndex(3));

    int population[4] = {};
    if (SECTREE_MAP* sectors = SECTREE_MANAGER::instance().GetMap(map))
    {
        for (const auto& entry : *sectors)
        {
            entry.second->ForEachEntity([&population](LPENTITY entity) {
                if (entity->IsType(0) && entity->GetDesc())
                {
                    const BYTE empire = static_cast<CHARACTER*>(entity)->GetEmpire();
                    if (empire < 4)
                        ++population[empire];
                }
            });
        }
    }
    for (int empire = 1; empire <= 3; ++empire)
    {
        character->ChatPacket(1, LC_TEXT("%s killcount %d mobkillcount %d bosskillcount %d \274\272\301\366 \301\370\300\324\274\366 %d"), GetEmpireName(empire),
                              get_kill_count_per_empire(empire), get_mob_kill_count_per_empire(empire),
                              get_boss_kill_count(), population[empire]);
    }
}

// 080c7160, present in the executable without a command-table entry.
void do_threeway_war_regen(CHARACTER* character, char*, int, int)
{
    const long mapIndex = character->GetMapIndex();
    std::string filename;
    SECTREE_MANAGER::instance().GetMap(mapIndex);
}

void do_threeway_war_myinfo(CHARACTER* character, char*, int, int)
{
    character->ChatPacket(1, "%s", LC_TEXT("\263\252\300\307 \273\357\260\305\270\256 \301\370\307\340\301\244\272\270"));
    character->ChatPacket(1, "Deadcount %d KillingCount %d", character->m_iKillCount, character->m_iKillingCount);
}

void do_siege(CHARACTER*, char*, int, int)
{
    const BYTE header = 25;
    P2P_MANAGER::instance().Send(&header, sizeof(header));
    castle_siege(number_ex(1, 3, const_cast<char*>("cmd_gm.cpp"), 0xd89));
}

void do_frog(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    const long empire = std::strtol(arg, nullptr, 10);
    if (empire < 1 || empire > 3)
    {
        character->ChatPacket(1, "Usage: empire(1,2,3)");
        return;
    }
    const long map = character->GetMapIndex();
    if (map < 181 || map > 183)
    {
        character->ChatPacket(1, "You must spawn frog in castle");
        return;
    }
    castle_spawn_frog(static_cast<int>(empire));
    castle_save();
}

void do_private(CHARACTER* character, char* argument, int, int)
{
    char mapText[256];
    one_argument(argument, mapText);
    if (!mapText[0])
    {
        character->ChatPacket(1, "Usage: private <map index>");
        return;
    }

    const int baseIndex = std::atoi(mapText);
    auto& manager = SECTREE_MANAGER::instance();
    const long privateIndex = manager.CreatePrivateMap(baseIndex);
    if (!privateIndex)
    {
        character->ChatPacket(1, "Can't find map by index %d", baseIndex);
        return;
    }

    character->SaveExitLocation();
    const PIXEL_POSITION& spawn = manager.GetMap(privateIndex)->m_setting.posSpawn;
    character->WarpSet(spawn.x, spawn.y, privateIndex);
}

void do_inventory(CHARACTER* character, char* argument, int, int)
{
    char first[256], second[256];
    two_arguments(argument, first, second);
    if (!first[0])
    {
        character->ChatPacket(1, "Usage: inventory <start_index> <count>");
        return;
    }

    const int lastCell = 90;
    const int start = second[0] ? std::max(0, std::atoi(first)) : 0;
    const int count = std::min(lastCell, std::atoi(second[0] ? second : first));
    for (int index = start, shown = 0; index < lastCell && shown < count; ++index, ++shown)
    {
        CItem* item = character->GetItem(static_cast<BYTE>(index));
        const char* name = item ? item->GetName() : nullptr;
        character->ChatPacket(1, "inventory [%d] = %s", index, name ? name : "NONE");
    }
}


#include <ctime>

void do_test(CHARACTER* character, char*, int, int)
{
    if (CItem* weapon = character->GetWear(4))
        weapon->ApplyAddon(0);
}

void do_detaillog(CHARACTER* character, char*, int, int)
{
    character->ToggleDetailLog();
}

void do_monsterlog(CHARACTER* character, char*, int, int)
{
    character->ToggleMonsterLog();
}

bool PCBangPlayTimeWriter_RequestUpdateIPList(const char* who, bool force);
std::string PCBangPlayTimeWriter_GetInfo();
bool PCBangPlayTimeWriter_SetEnvironmentTime(const char* who, const std::string& type, time_t value);

void do_pcbang_env(CHARACTER* character, char* argument, int, int)
{
    char type[256], value[256];
    two_arguments(argument, type, value);
    if (PCBangPlayTimeWriter_SetEnvironmentTime(character->GetName(), type, std::atoi(value)))
        character->ChatPacket(1, "pcbang_env %s = %s", type, value);
    else
    {
        character->ChatPacket(1, "FAILURE!! pcbang_env UNKNOWN_TYPE(%s) = %s", type, value);
        character->ChatPacket(1, "You can see /pcbang_info");
    }
}

void do_pcbang_info(CHARACTER* character, char*, int, int)
{
    const std::string info = PCBangPlayTimeWriter_GetInfo();
    character->ChatPacket(1, "%s", info.c_str());
}

void do_pcbang_update(CHARACTER* character, char*, int, int)
{
    PCBangPlayTimeWriter_RequestUpdateIPList(character->GetName(), true);
    character->ChatPacket(1, "pcbang_update");
}


void do_set(CHARACTER* character, char* argument, int, int)
{
    char name[256], field[256], value[256];
    one_argument(two_arguments(argument, name, field), value);
    if (!name[0] || !field[0] || !value[0])
    {
        character->ChatPacket(1, "Usage: set <name> <field> <value>");
        return;
    }

    CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name);
    if (!target)
    {
        character->ChatPacket(1, "%s not exist", name);
        return;
    }

    static const struct
    {
        const char* name;
        BYTE point;
    } fields[] = {
        {"gold", POINT_GOLD}, {"race", POINT_NONE}, {"sex", POINT_NONE},
        {"exp", POINT_EXP}, {"max_hp", POINT_MAX_HP}, {"max_sp", POINT_MAX_SP},
        {"skill", POINT_SKILL}, {"alignment", POINT_NONE}, {"align", POINT_NONE}
    };
    const auto length = std::strlen(field);
    for (const auto& entry : fields)
    {
        if (std::strncmp(field, entry.name, length) != 0)
            continue;
        if (entry.point == POINT_NONE && entry.name[0] != 'a')
            return;

        if (entry.point == POINT_GOLD)
            DBManager::instance().SendMoneyLog(6, 3, std::atoi(value));

        const int amount = std::atoi(value);
        if (entry.point != POINT_NONE) {
            target->PointChange(entry.point, amount, true, false);
        } else {
            const auto current = static_cast<std::uint32_t>(character->GetRealAlignment());
            const auto delta = static_cast<std::int32_t>(static_cast<std::uint32_t>(amount) - current);
            target->UpdateAlignment(delta);
        }
        const int reported = std::atoi(value);
        character->ChatPacket(1, "%s's %s set to [%d]", target->GetName(), entry.name, reported);
        return;
    }
}


void do_save_attribute_to_image(CHARACTER* character, char* argument, int, int)
{
    char index[256], filename[256];
    two_arguments(argument, index, filename);
    if (!index[0] || !filename[0])
    {
        character->ChatPacket(1, "Syntax: /saveati <map_index> <filename>");
        return;
    }
    const bool saved = SECTREE_MANAGER::instance().SaveAttributeToImage(
        std::atoi(index), filename, nullptr);
    character->ChatPacket(1, "%s", saved ? "Save done." : "Save failed.");
}


struct log_packet_func
{
    const char* m_str;
    log_packet_func(const char* str):m_str(str) {}
    void operator()(DESC* d) {
        if(d->GetCharacter() && d->GetCharacter()->GetGMLevel())
            d->GetCharacter()->ChatPacket(2,"%s",m_str);
    }
};
void SendLog(const char* c_pszBuf)
{
    const DESC_MANAGER::DESC_SET& clients=DESC_MANAGER::instance().GetClientSet();
    std::for_each(clients.begin(),clients.end(),log_packet_func(c_pszBuf));
}

void SendNotice(const char* text)
{
    const DESC_MANAGER::DESC_SET& clients=DESC_MANAGER::instance().GetClientSet();
    for (DESC_MANAGER::DESC_SET::const_iterator it=clients.begin();it!=clients.end();++it) {
        DESC* desc=*it;
        if (desc->GetCharacter()) desc->GetCharacter()->ChatPacket(2,"%s",text);
    }
}

namespace {
struct GotoInfo {
    std::string name;
    BYTE empire;
    int map;
    DWORD x, y;
};

std::vector<GotoInfo> gs_vec_gotoInfo;
}

void CHARACTER_AddGotoInfo(const std::string& name, BYTE empire, int map, DWORD x, DWORD y)
{
    gs_vec_gotoInfo.push_back({name, empire, map, x, y});
    sys_log(0, "AddGotoInfo(name=%s, empire=%d, mapIndex=%d, pos=(%lu, %lu))",
            name.c_str(), empire, map, x, y);
}

bool FindInString(const char* prefix, const char* aliases)
{
    const auto length = std::strlen(prefix);
    for (const char* next = std::strchr(aliases, '|'); next; next = std::strchr(aliases, '|')) {
        const auto segmentLength = std::min<std::size_t>(64, next - aliases);
        if (length <= segmentLength && !strncasecmp(prefix, aliases, length))
            return true;
        aliases = next + 1;
    }
    return !strncasecmp(prefix, aliases, length);
}

bool CHARACTER_GoToName(CHARACTER* character, BYTE empire, int map, const char* name)
{
    for (const GotoInfo& entry : gs_vec_gotoInfo) {
        if ((map ? map != entry.map : !FindInString(name, entry.name.c_str()))
            || (entry.empire && entry.empire != empire))
            continue;
        const auto x = static_cast<std::int32_t>(static_cast<std::uint32_t>(entry.x) * 100U);
        const auto y = static_cast<std::int32_t>(static_cast<std::uint32_t>(entry.y) * 100U);
        character->ChatPacket(1, "You warp to ( %d , %d )", x, y);
        character->WarpSet(x, y, 0);
        character->Stop();
        return true;
    }
    return false;
}

void do_goto(CHARACTER* character, char* argument, int, int)
{
    char input[256] = {};
    if (argument)
        std::strncpy(input, argument, sizeof(input) - 1);
    char first[256];
    char second[256];
    two_arguments(input, first, second);
    if (!*first && !*second) {
        character->ChatPacket(1, "Usage: goto <x meter> <y meter>");
        return;
    }
    if (std::isdigit(static_cast<unsigned char>(*first)) && std::isdigit(static_cast<unsigned char>(*second))) {
        auto x = static_cast<std::uint32_t>(std::atoi(first));
        auto y = static_cast<std::uint32_t>(std::atoi(second));
        PIXEL_POSITION base;
        if (SECTREE_MANAGER::instance().GetMapBasePosition(character->GetX(), character->GetY(), base)) {
            x += static_cast<std::int32_t>(base.x) / 100;
            y += static_cast<std::int32_t>(base.y) / 100;
        }
        character->ChatPacket(1, "You goto ( %ld , %ld )",
                              static_cast<long>(static_cast<std::int32_t>(x)),
                              static_cast<long>(static_cast<std::int32_t>(y)));
        character->Show(character->GetMapIndex(), static_cast<std::int32_t>(x * 100U),
                        static_cast<std::int32_t>(y * 100U), 0, false);
        character->Stop();
        return;
    }
    const int map = *first == '#' ? std::atoi(first + 1) : 0;
    const BYTE empire = std::isdigit(static_cast<unsigned char>(*second))
        ? static_cast<BYTE>(std::max(1, std::min(3, std::atoi(second)))) : character->GetEmpire();
    if (CHARACTER_GoToName(character, empire, map, first))
        character->ChatPacket(1, "Cannot find map command syntax: /goto <mapname> [empire]");
}

void do_event_flag(CHARACTER* character, char* argument, int, int)
{
    char flag[257];
    char valueText[257];
    two_arguments(argument, flag, valueText);
    if (!*flag || !*valueText)
        return;

    int value = std::atoi(valueText);
    const char* rateFlags[] = {"mob_item", "mob_exp", "mob_gold", "mob_dam", "mob_gold_pct",
        "mob_item_buyer", "mob_exp_buyer", "mob_gold_buyer", "mob_gold_pct_buyer"};
    for (const char* rate : rateFlags) {
        if (!std::strcmp(flag, rate)) {
            value = MINMAX(0, value, 1000);
            break;
        }
    }
    quest::CQuestManager::instance().RequestSetEventFlag(flag, value);
    character->ChatPacket(1, "RequestSetEventFlag %s %d", flag, value);
    sys_log(0, "RequestSetEventFlag %s %d", flag, value);
}

void do_get_event_flag(CHARACTER* character, char*, int, int)
{
    quest::CQuestManager::instance().SendEventFlagList(character);
}

void SendNoticeMap(const char* text, int map, bool bigFont)
{
    for (DESC* desc : DESC_MANAGER::instance().GetClientSet()) {
        CHARACTER* character = desc->GetCharacter();
        if (character && static_cast<std::int32_t>(character->GetMapIndex()) == map)
            character->ChatPacket(bigFont ? 8 : 2, "%s", text);
    }
}

void BroadcastNotice(const char* text)
{
    BYTE header[5] = {4};
    const std::int32_t size = std::strlen(text) + 1;
    std::memcpy(header + 1, &size, sizeof(size));
    TEMP_BUFFER buffer;
    buffer.write(header, sizeof(header));
    buffer.write(text, size);
    const int packetSize = buffer.size();
    const void* packetData = buffer.read_peek();
    P2P_MANAGER::instance().Send(packetData, packetSize);
    SendNotice(text);
}

void do_notice(CHARACTER*, char* text, int, int)
{
    BroadcastNotice(text);
}

void do_map_notice(CHARACTER* character, char* text, int, int)
{
    SendNoticeMap(text, character->GetMapIndex(), false);
}

void do_big_notice(CHARACTER* character, char* text, int, int)
{
    character->ChatPacket(8, "%s", text);
}

namespace {
quest::PC* FindCommandQuestPC(CHARACTER* character, const char* name)
{
    CHARACTER* target = name[0] ? CHARACTER_MANAGER::instance().FindPC(name) : character;
    if (!target) {
        character->ChatPacket(1, "There is no such character.");
        return nullptr;
    }
    return quest::CQuestManager::instance().GetPC(target->GetPlayerID());
}
}

void do_getqf(CHARACTER* character, char* argument, int, int)
{
    char target[256];
    one_argument(argument, target);
    if (quest::PC* player = FindCommandQuestPC(character, target))
        player->SendFlagList(character);
}

void do_setqf(CHARACTER* character, char* argument, int, int)
{
    char name[256], value[256], target[256];
    one_argument(two_arguments(argument, name, value), target);
    if (!name[0]) {
        character->ChatPacket(1, "Syntax: setqf <flagname> <value> [<character name>]");
        return;
    }
    if (quest::PC* player = FindCommandQuestPC(character, target)) {
        const int number = std::atoi(value);
        player->SetFlag(name, number, false);
        character->ChatPacket(1, "Quest flag set: %s %d", name, number);
    }
}

void do_delqf(CHARACTER* character, char* argument, int, int)
{
    char name[256], target[256];
    two_arguments(argument, name, target);
    if (!name[0]) {
        character->ChatPacket(1, "Syntax: delqf <flagname> [<character name>]");
        return;
    }
    if (quest::PC* player = FindCommandQuestPC(character, target)) {
        const bool removed = player->DeleteFlag(name);
        character->ChatPacket(1, "%s", removed ? "Delete success." : "Delete failed. Quest flag does not exist.");
    }
}

void do_affect_remove(CHARACTER* character, char* argument, int, int)
{
    char typeText[256], pointText[256];
    two_arguments(argument, typeText, pointText);
    if (!typeText[0] || !pointText[0]) {
        character->ChatPacket(1, "Syntax: /affect_remove <type> <point>");
        character->ChatPacket(1, "-- Affect List -------------------------------");
        character->ChatPacket(1, "Type Point Modif Duration Flag");
        for (const CAffect* effect : character->m_list_pkAffect) {
            character->ChatPacket(1, "%4d %5d %5d %8d %lu", static_cast<std::int32_t>(effect->dwType),
                                  effect->bApplyOn, static_cast<std::int32_t>(effect->lApplyValue),
                                  static_cast<std::int32_t>(effect->lDuration),
                                  static_cast<unsigned long>(static_cast<std::uint32_t>(effect->dwFlag)));
        }
        return;
    }

    const BYTE point = static_cast<BYTE>(std::atoi(pointText));
    const auto type = static_cast<std::uint32_t>(std::atoi(typeText));
    bool removed = false;
    while (CAffect* effect = character->FindAffect(type, point)) {
        if (!character->RemoveAffect(effect))
            break;
        removed = true;
    }
    character->ChatPacket(1, "%s", removed ? "Affect successfully removed." : "Not affected by that type and point.");
}

void do_user(CHARACTER* character, char*, int, int)
{
    char row[128]{};
    std::size_t length = 0;
    int count = 0;
    for (DESC* desc : DESC_MANAGER::instance().GetClientSet()) {
        CHARACTER* player = desc->GetCharacter();
        if (!player)
            continue;
        const int written = std::snprintf(row + length, sizeof(row) - length, "%-16s ", player->GetName());
        if (written >= 0)
            length += std::min(static_cast<std::size_t>(written), sizeof(row) - length - 1);
        if (++count % 4 == 0) {
            character->ChatPacket(1, "%s", row);
            row[0] = '\0';
            length = 0;
        }
    }
    if (count % 4)
        character->ChatPacket(1, "%s", row);
    character->ChatPacket(1, "Total %d", count);
}

void do_flush(CHARACTER* character, char* argument, int, int)
{
    char id[256];
    one_argument(argument, id);
    if (!id[0]) {
        character->ChatPacket(1, "usage : /flush player_id");
        return;
    }
    const std::uint32_t pid = std::strtoul(id, nullptr, 10);
    if (db_clientdesc) {
        db_clientdesc->DBPacketHeader(42, 0, sizeof(pid));
        db_clientdesc->Packet(&pid, sizeof(pid));
    }
}

void do_reset_subskill(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    if (!name[0]) {
        character->ChatPacket(1, "Usage: reset_subskill <name>");
        return;
    }
    if (CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name)) {
        target->ClearSubSkill();
        character->ChatPacket(1, "Subskill of [%s] was reset", target->GetName());
    }
}

void do_stat_plus_amount(CHARACTER* character, char* argument, int, int subcommand)
{
    char text[256];
    one_argument(argument, text);
    if (!text[0])
        return;
    const auto remaining = static_cast<std::int32_t>(character->GetPoint(26));
    const int requested = std::atoi(text);
    const char* error = nullptr;
    if (character->IsPolymorphed())
        error = "\265\320\260\251\040\301\337\277\241\264\302\040\264\311\267\302\300\273\040\277\303\270\261\040\274\366\040\276\370\275\300\264\317\264\331\056";
    else if (remaining < 1)
        error = "\263\262\300\272\040\275\272\305\310\040\306\367\300\316\306\256\260\241\040\276\370\275\300\264\317\264\331\056";
    else if (remaining < requested)
        error = "\263\262\300\272\040\275\272\305\310\040\306\367\300\316\306\256\260\241\040\300\373\275\300\264\317\264\331\056";
    else if (requested < 0)
        error = "\260\252\300\273\040\300\337\270\370\040\300\324\267\302\307\317\277\264\275\300\264\317\264\331\056";
    else if (subcommand < 12 || subcommand > 15)
        error = "\270\355\267\311\276\356\300\307\040\274\255\272\352\040\304\277\270\307\265\345\260\241\040\300\337\270\370\040\265\307\276\372\275\300\264\317\264\331\056";
    if (error) {
        character->ChatPacket(1, "%s", locale_find(error, g_iUseLocale));
        return;
    }

    const BYTE point = static_cast<BYTE>(subcommand);
    const auto current = static_cast<std::uint32_t>(character->GetPoint(point));
    int change = requested;
    if (static_cast<std::int32_t>(current + static_cast<std::uint32_t>(requested)) > 90)
        change = static_cast<std::int32_t>(90U - static_cast<std::uint32_t>(character->GetPoint(point)));
    if (!change)
        return;

    const auto delta = static_cast<std::uint32_t>(change);
    const auto realPoint = static_cast<std::uint32_t>(character->GetRealPoint(point));
    character->SetRealPoint(point, static_cast<std::int32_t>(realPoint + delta));
    const auto livePoint = static_cast<std::uint32_t>(character->GetPoint(point));
    character->SetPoint(point, static_cast<std::int32_t>(livePoint + delta));
    character->ComputePoints();
    character->PointChange(point, 0, false, false);
    character->PointChange(26, static_cast<std::int32_t>(0U - delta), false, false);
    character->ComputePoints();
}

void do_who(CHARACTER* character, char*, int, int)
{
    int total, local;
    int* empire;
    DESC_MANAGER::instance().GetUserCount(total, &empire, local);
    character->ChatPacket(1, "Total [%d] %d / %d / %d (this server %d)",
                          total, empire[1], empire[2], empire[3], local);
}

void do_disconnect(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    if (!name[0]) {
        character->ChatPacket(1, "ex) /dc <player name>");
        return;
    }
    DESC_MANAGER& manager = DESC_MANAGER::instance();
    if (DESC* desc = manager.FindByCharacterName(name); desc && desc->GetCharacter()) {
        manager.DestroyDesc(desc, true);
        return;
    }
    character->ChatPacket(1, "%s: no such a player.", name);
}

void do_transfer(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    if (!name[0]) {
        character->ChatPacket(1, "Usage: transfer <name>");
        return;
    }
    if (CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(name)) {
        if (target != character) {
            const auto map = static_cast<std::int32_t>(character->GetMapIndex());
            const auto y = static_cast<std::int32_t>(character->GetY());
            const auto x = static_cast<std::int32_t>(character->GetX());
            target->WarpSet(x, y, map);
        } else {
            character->ChatPacket(1, "Transfer me?!?");
        }
        return;
    }
    P2P_MANAGER& peers = P2P_MANAGER::instance();
    const CCI* remote = peers.Find(name);
    if (!remote) {
        character->ChatPacket(1, "There is no character by that name");
        return;
    }
    if (remote->bChannel != g_bChannel) {
        character->ChatPacket(1, "Target is in %d channel (my channel %d)", remote->bChannel, g_bChannel);
        return;
    }
    char packet[34]{};
    packet[0] = 16;
    std::strncpy(packet + 1, name, 24);
    const std::int32_t x = character->GetX(), y = character->GetY();
    std::memcpy(packet + 26, &x, sizeof(x));
    std::memcpy(packet + 30, &y, sizeof(y));
    peers.Send(packet, sizeof(packet));
    character->ChatPacket(1, "Transfer requested.");
}

namespace {
void SpawnCommandMob(CHARACTER* character, char* argument, const char* usage,
                     int limit, int firstLine, void (CHARACTER::*configure)())
{
    char name[256], countText[256];
    two_arguments(argument, name, countText);
    if (!name[0]) {
        character->ChatPacket(1, "%s", usage);
        return;
    }
    CMobManager& mobs = CMobManager::instance();
    const CMob* mob = std::isdigit(static_cast<unsigned char>(name[0]))
        ? mobs.Get(static_cast<DWORD>(std::strtoul(name, nullptr, 10))) : mobs.Get(name, true);
    if (!mob || !mob->m_table.dwVnum) {
        character->ChatPacket(1, "No such mob by that vnum");
        return;
    }
    const int count = std::min(limit, countText[0] ? std::atoi(countText) : 1);
    CHARACTER_MANAGER& manager = CHARACTER_MANAGER::instance();
    char* source = const_cast<char*>("cmd_gm.cpp");
    for (int i = 0; i < count; ++i) {
        const auto ey = static_cast<std::int32_t>(static_cast<std::uint32_t>(character->GetY()) +
            static_cast<std::uint32_t>(number_ex(200, 750, source, firstLine + 3)));
        const auto ex = static_cast<std::int32_t>(static_cast<std::uint32_t>(character->GetX()) +
            static_cast<std::uint32_t>(number_ex(200, 750, source, firstLine + 2)));
        const auto sy = static_cast<std::int32_t>(static_cast<std::uint32_t>(character->GetY()) -
            static_cast<std::uint32_t>(number_ex(200, 750, source, firstLine + 1)));
        const auto sx = static_cast<std::int32_t>(static_cast<std::uint32_t>(character->GetX()) -
            static_cast<std::uint32_t>(number_ex(200, 750, source, firstLine)));
        CHARACTER* spawned = manager.SpawnMobRange(mob->m_table.dwVnum, character->GetMapIndex(),
                                                   sx, sy, ex, ey, true, mob->m_table.bType == 2, false);
        if (spawned && configure)
            (spawned->*configure)();
    }
}

}

void do_mob(CHARACTER* character, char* argument, int, int)
{
    SpawnCommandMob(character, argument, "Usage: mob <mob vnum>", test_server ? 40 : 20, 0x328, nullptr);
}

void do_mob_aggresive(CHARACTER* character, char* argument, int, int)
{
    SpawnCommandMob(character, argument, "Usage: mob <mob vnum>", 20, 0x2e8, &CHARACTER::SetAggressive);
}

void do_mob_coward(CHARACTER* character, char* argument, int, int)
{
    SpawnCommandMob(character, argument, "Usage: mc <vnum>", 20, 0x298, &CHARACTER::SetCoward);
}

void do_mob_map(CHARACTER* character, char* argument, int, int)
{
    char id[256];
    one_argument(argument, id);
    if (!id[0]) {
        character->ChatPacket(1, "Syntax: mm <vnum>");
        return;
    }
    const DWORD vnum = std::strtoul(id, nullptr, 10);
    CHARACTER* spawned = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(vnum, character->GetMapIndex());
    if (spawned)
        character->ChatPacket(1, "%s spawned in %ldx%ld", spawned->GetName(), spawned->GetX(), spawned->GetY());
    else
        character->ChatPacket(1, "Spawn failed.");
}

void do_purge(CHARACTER* character, char* argument, int, int)
{
    char option[256];
    one_argument(argument, option);
    const bool all = std::strcmp(option, "all") == 0;
    SECTREE* sector = character->GetSectree();
    if (!sector)
        return;
    auto purge = [character, all](CEntity* entity) {
        if (!entity->IsType(0))
            return;
        CHARACTER* target = static_cast<CHARACTER*>(entity);
        const int distance = GMTargetDistance(*target, *character);
        if (!all && distance >= 1000)
            return;
        sys_log(0, "PURGE: %s %d", target->GetName(), distance);
        if (target->GetCharType() != CHAR_TYPE_PC)
            CHARACTER_MANAGER::instance().DestroyCharacter(target);
    };
    sector->ForEachAround(purge);
}

void do_item_purge(CHARACTER* character, char*, int, int)
{
    for (int slot = 0; slot <= 108; ++slot) {
        const auto cell = static_cast<BYTE>(slot);
        if (CItem* item = character->GetItem(cell)) {
            ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
            character->SyncQuickslot(1, cell, 0xff);
        }
    }
}

void do_group(CHARACTER* character, char* argument, int, int)
{
    char id[256];
    one_argument(argument, id);
    if (!id[0]) {
        character->ChatPacket(1, "Usage: group <group vnum>");
        return;
    }
    const DWORD vnum = std::strtoul(id, nullptr, 10);
    const long map = character->GetMapIndex();
    const int x = character->GetX(), y = character->GetY();
    if (test_server)
        sys_log(0, "COMMAND GROUP SPAWN %lu at %ld %d %d", static_cast<unsigned long>(vnum), map, x, y);
    CHARACTER_MANAGER::instance().SpawnGroup(vnum, map,
        static_cast<std::int32_t>(static_cast<std::uint32_t>(x) - 500U),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(y) - 500U),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(x) + 500U),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(y) + 500U),
        nullptr, false, nullptr);
}

void do_group_random(CHARACTER* character, char* argument, int, int)
{
    char id[256];
    one_argument(argument, id);
    if (!id[0]) {
        character->ChatPacket(1, "Usage: grrandom <group vnum>");
        return;
    }
    const DWORD vnum = std::strtoul(id, nullptr, 10);
    const int x = character->GetX(), y = character->GetY();
    CHARACTER_MANAGER::instance().SpawnGroupGroup(vnum, character->GetMapIndex(),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(x) - 500U),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(y) - 500U),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(x) + 500U),
        static_cast<std::int32_t>(static_cast<std::uint32_t>(y) + 500U),
        nullptr, false, nullptr);
}

void do_warp(CHARACTER* character, char* argument, int, int)
{
    char first[256], second[256];
    two_arguments(argument, first, second);
    if (!first[0]) {
        character->ChatPacket(1, "Usage: warp <character name> | <x meter> <y meter> [z]");
        return;
    }

    long x = 0, y = 0;
    if (std::isdigit(static_cast<unsigned char>(first[0])) &&
        std::isdigit(static_cast<unsigned char>(second[0]))) {
        x = std::strtol(first, nullptr, 10);
        y = std::strtol(second, nullptr, 10);
        if (x < 0 || x > INT32_MAX / 100 || y < 0 || y > INT32_MAX / 100)
            return;
    } else {
        static const struct { const char* name; int x, y; } towns[] = {
        {"\277\265\276\310\300\276\274\272", 4743, 9548},
        {"\300\323\301\366\260\356", 3235, 9086},
        {"\300\332\276\347\307\366", 3531, 8829},
        {"\301\266\276\310\300\276\274\272", 638, 1664},
        {"\275\302\267\346\260\356", 1745, 1909},
        {"\272\271\301\244\307\366", 1455, 2400},
        {"\306\362\271\253\300\276\274\272", 9599, 2692},
        {"\271\346\273\352\260\356", 8036, 2984},
        {"\271\332\266\363\307\366", 8639, 2460},
        {"\274\255\307\321\273\352", 4350, 2143},
        };
        const auto town = std::find_if(std::begin(towns), std::end(towns), [&first](const auto& entry) {
            return !std::strcmp(entry.name, first);
        });
        if (town != std::end(towns)) {
            x = town->x;
            y = town->y;
        } else if (CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(first)) {
            x = static_cast<std::int32_t>(target->GetX()) / 100;
            y = static_cast<std::int32_t>(target->GetY()) / 100;
        } else {
            const CCI* peer = P2P_MANAGER::instance().Find(first);
            if (!peer) {
                character->ChatPacket(1, "There is no one by that name");
                return;
            }
            if (peer->bChannel != g_bChannel) {
                character->ChatPacket(1, "Target is in %d channel (my channel %d)", peer->bChannel, g_bChannel);
                return;
            }
            PIXEL_POSITION center;
            if (!SECTREE_MANAGER::instance().GetCenterPositionOfMap(peer->bMapIndex, center)) {
                character->ChatPacket(1, "Cannot find map (index %d)", peer->bMapIndex);
                return;
            }
            character->ChatPacket(1, "You warp to ( %ld , %ld )", static_cast<long>(center.x), static_cast<long>(center.y));
            character->WarpSet(center.x, center.y, 0);
            return;
        }
    }
    const auto worldX = static_cast<std::int32_t>(static_cast<std::uint32_t>(x) * 100U);
    const auto worldY = static_cast<std::int32_t>(static_cast<std::uint32_t>(y) * 100U);
    character->ChatPacket(1, "You warp to ( %ld , %ld )",
                          static_cast<long>(worldX), static_cast<long>(worldY));
    character->WarpSet(worldX, worldY, 0);
    character->Stop();
}

void do_change_attr(CHARACTER* character, char*, int, int)
{
    if (CItem* weapon = character->GetWear(4))
        weapon->ChangeAttribute(nullptr);
}

void do_add_attr(CHARACTER* character, char*, int, int)
{
    if (CItem* weapon = character->GetWear(4))
        weapon->AddAttribute();
}

void do_add_socket(CHARACTER* character, char*, int, int)
{
    if (CItem* weapon = character->GetWear(4))
        weapon->AddSocket();
}

namespace {
void PlaceGMItem(CHARACTER* character, CItem* item)
{
    const int slot = character->GetEmptyInventory(item->GetSize());
    if (slot < 0 || !item->AddToCharacter(character, static_cast<BYTE>(slot))) {
        ITEM_MANAGER::instance().DestroyItem(item);
        character->ChatPacket(1, "Not enough inventory space.");
        return;
    }
    LogManager::instance().ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                   item->GetID(), "GM", item->GetName());
}

void SetCommandSkill(CHARACTER* caller, CHARACTER* target, const char* name, const char* level)
{
    CSkillManager& skills = CSkillManager::instance();
    CSkillProto* skill = std::isdigit(static_cast<unsigned char>(name[0]))
        ? skills.Get(static_cast<std::uint32_t>(std::atoi(name))) : skills.Get(name);
    if (!skill) {
        caller->ChatPacket(1, "No such a skill by that name.");
        return;
    }
    target->SetSkillLevel(skill->dwVnum, static_cast<BYTE>(std::atoi(level)));
    target->ComputePoints();
    target->SkillLevelPacket();
}
}

void do_setskill(CHARACTER* character, char* argument, int, int)
{
    char name[256], level[256];
    two_arguments(argument, name, level);
    if (!name[0] || !std::isdigit(static_cast<unsigned char>(level[0]))) {
        character->ChatPacket(1, "Syntax: setskill <name> <lev>");
        return;
    }
    SetCommandSkill(character, character, name, level);
}

void do_setskillother(CHARACTER* character, char* argument, int, int)
{
    char player[256], name[256], level[256];
    one_argument(two_arguments(argument, player, name), level);
    if (!player[0] || !name[0] || !std::isdigit(static_cast<unsigned char>(level[0]))) {
        character->ChatPacket(1, "Syntax: setskillother <target> <skillname> <lev>");
        return;
    }
    CHARACTER* target = CHARACTER_MANAGER::instance().FindPC(player);
    if (!target) {
        character->ChatPacket(1, "There is no such character.");
        return;
    }
    SetCommandSkill(character, target, name, level);
}

void do_set_skill_group(CHARACTER* character, char* argument, int, int)
{
    char text[256];
    one_argument(argument, text);
    const int group = text[0] ? std::atoi(text) : 0;
    character->SetSkillGroup(static_cast<BYTE>(group));
    character->ClearSkill();
    character->ChatPacket(1, "skill group to %d.", group);
}

void do_set_skill_point(CHARACTER* character, char* argument, int, int)
{
    char text[256];
    one_argument(argument, text);
    const int points = text[0] ? std::atoi(text) : 0;
    character->SetRealPoint(28, points);
    character->SetPoint(28, static_cast<std::int32_t>(character->GetRealPoint(28)));
    character->PointChange(28, 0, false, false);
}

void do_cooltime(CHARACTER* character, char*, int, int)
{
    character->DisableCooltime();
}

void do_invisibility(CHARACTER* character, char*, int, int)
{
    if (character->IsAffectFlag(2))
        character->RemoveAffect(0xcb);
    else
        character->AddAffect(0xcb, 0, 0, 2, 0x1fffffff, 0, true, false);
}

void do_mount_test(CHARACTER* character, char* argument, int, int)
{
    char text[256];
    one_argument(argument, text);
    if (text[0])
        character->MountVnum(static_cast<DWORD>(std::atoi(text)));
}

void do_polymorph(CHARACTER* character, char* argument, int, int)
{
    char race[256], maintain[256];
    two_arguments(argument, race, maintain);
    if (race[0])
        character->SetPolymorph(static_cast<DWORD>(std::atoi(race)), std::atoi(maintain) > 0);
}

void do_polymorph_item(CHARACTER* character, char* argument, int, int)
{
    char race[256];
    one_argument(argument, race);
    if (!race[0])
        return;
    CItem* item = ITEM_MANAGER::instance().CreateItem(0x111d8, 1, 0, true, -1);
    if (!item) {
        character->ChatPacket(1, "#%d item not exist by that vnum.", 0x111d7);
        return;
    }
    item->SetSocket(0, std::atoi(race));
    PlaceGMItem(character, item);
}

void do_item(CHARACTER* character, char* argument, int, int)
{
    char name[256], countText[256];
    two_arguments(argument, name, countText);
    if (!name[0]) {
        character->ChatPacket(1, "Usage: item <item vnum>");
        return;
    }
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    DWORD vnum = 0;
    const bool numeric = std::isdigit(static_cast<unsigned char>(name[0])) != 0;
    if (numeric)
        vnum = std::strtoul(name, nullptr, 10);
    const int count = countText[0] ? MINMAX(1, std::atoi(countText), 200) : 1;
    CItem* item = numeric || items.GetVnum(name, vnum) ? items.CreateItem(vnum, count, 0, true, -1) : nullptr;
    if (!item) {
        character->ChatPacket(1, "#%d item not exist by that vnum.", static_cast<int>(vnum));
        return;
    }
    PlaceGMItem(character, item);
}

void do_level(CHARACTER* character, char* argument, int, int)
{
    char level[256];
    one_argument(argument, level);
    if (!level[0]) {
        character->ChatPacket(1, "Syntax: level <level>");
        return;
    }
    character->ResetPoint(MINMAX(1, std::atoi(level), 99));
    character->ClearSkill();
    character->ClearSubSkill();
}

void do_kill(CHARACTER* character, char* argument, int, int)
{
    char name[256];
    one_argument(argument, name);
    if (!name[0]) {
        character->ChatPacket(1, "ex) /kill <player name>");
        return;
    }
    DESC* desc = DESC_MANAGER::instance().FindByCharacterName(name);
    if (CHARACTER* target = desc ? desc->GetCharacter() : nullptr) {
        target->Dead(nullptr, false);
        return;
    }
    character->ChatPacket(1, "%s: no such a player", name);
}

void do_reload(CHARACTER* character, char* argument, int, int)
{
    char option[256];
    one_argument(argument, option);
    const auto reloadUsers = [character]() {
        character->ChatPacket(1, "Reloading state_user_count.");
        LoadStateUserCount();
    };
    const auto reloadProto = [character]() {
        character->ChatPacket(1, "Reloading prototype tables,");
        if (db_clientdesc)
            db_clientdesc->DBPacket(43, 0, nullptr, 0);
    };
    const auto reloadStrings = [character]() {
        character->ChatPacket(1, "Reloading notice string.");
        DBManager::instance().LoadDBString();
    };
    switch (std::tolower(static_cast<unsigned char>(option[0]))) {
    case 0:
        reloadUsers();
        reloadProto();
        reloadStrings();
        break;
    case 'a':
        character->ChatPacket(1, "Reloading Admin infomation.");
        if (db_clientdesc)
            db_clientdesc->DBPacket(115, 0, nullptr, 0);
        sys_log(0, "Reloading admin infomation.");
        break;
    case 'c': Cube_init(); break;
    case 'f': fishing::Initialize(); break;
    case 'p': reloadProto(); break;
    case 'q':
        character->ChatPacket(1, "Reloading quest.");
        quest::CQuestManager::instance().Reload();
        break;
    case 's': reloadStrings(); break;
    case 'u': reloadUsers(); break;
    }
}

// 080c6e80
void do_roulette(CHARACTER*, char*, int, int)
{
}
