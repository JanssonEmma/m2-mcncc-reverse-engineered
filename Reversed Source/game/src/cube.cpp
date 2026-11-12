#include "cube.h"
#include "char.h"
#include "config.h"
#include "item.h"
#include "locale.hpp"
#include "log.h"
#include "dev_log.h"
#include "utils.h"
#include "../../libthecore/include/utils.h"
#include <cstdint>
#include <iterator>
#include <limits>
#include <map>
#include "locale_service.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/typedef.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <memory>
#include <vector>

struct CUBE_VALUE {
    std::uint32_t vnum;
    int count;
};

namespace {
using CubeMaterialCounts = std::map<DWORD, std::uint64_t>;

// 080fbf80
bool FN_check_item_count(CItem** items, DWORD vnum, int required)
{
    std::uint32_t count = 0;
    for (int slot = 0; slot < 15; ++slot) {
        CItem* item = items[slot];
        if (item && !item->IsExchanging() && item->GetVnum() == vnum)
            count += static_cast<std::uint32_t>(item->GetCount());
    }
    return static_cast<std::int32_t>(count) >= required;
}

// 080fbfd0
void FN_remove_material(CItem** items, DWORD vnum, int needed)
{
    DWORD remaining = static_cast<std::uint32_t>(needed);
    for (int slot = 0; slot < 15; ++slot) {
        CItem*& item = items[slot];
        if (!item || item->IsExchanging() || item->GetVnum() != vnum)
            continue;
        const DWORD count = item->GetCount();
        const DWORD used = std::min(remaining, count);
        remaining -= used;
        CItem* stack = item;
        if (used == count)
            item = nullptr;
        stack->SetCount(count - used);
        if (used < count)
            break;
    }
}

}

struct CUBE_DATA {
    CUBE_DATA();

    std::vector<WORD> npcVnums;
    std::vector<CUBE_VALUE> materials;
    std::vector<CUBE_VALUE> rewards;
    int percent;

    bool can_make_item(CItem** items, WORD npcVnum);
    CUBE_VALUE* reward_value();
    void remove_material(CHARACTER* character);
};

// 080fc180 / 080fc1e0
CUBE_DATA::CUBE_DATA() : percent(0) {}

// 080fc240
bool CUBE_DATA::can_make_item(CItem** items, WORD npcVnum)
{
    if (!npcVnum || materials.empty() || rewards.empty() ||
        std::find(npcVnums.begin(), npcVnums.end(), npcVnum) == npcVnums.end())
        return false;

    CubeMaterialCounts needed;
    for (const CUBE_VALUE& material : materials) {
        const auto required = needed[material.vnum] += static_cast<std::uint32_t>(material.count);
        if (required > std::numeric_limits<int>::max() ||
            !FN_check_item_count(items, material.vnum, static_cast<int>(required)))
            return false;
    }
    return true;
}

// 080fc2f0
CUBE_VALUE* CUBE_DATA::reward_value()
{
    char source[] = "cube.cpp";
    const int size = static_cast<int>(rewards.size());
    number_ex(0, size, source, 0xad);
    return &rewards[number_ex(0, size - 1, source, 0xae)];
}

// 080fc370
void CUBE_DATA::remove_material(CHARACTER* character)
{
    for (const CUBE_VALUE& material : materials)
        FN_remove_material(character->GetCubeItem(), material.vnum, material.count);
}

namespace {

std::vector<std::unique_ptr<CUBE_DATA>> recipes;

// 080fc060
CUBE_DATA* FN_find_cube(CItem** items, WORD npcVnum)
{
    for (const auto& recipe : recipes) {
        if (recipe->can_make_item(items, npcVnum))
            return recipe.get();
    }
    return nullptr;
}

bool FN_check_cube_data(const CUBE_DATA& recipe)
{
    const auto valid = [](const CUBE_VALUE& value) {
        return value.vnum != 0 && value.count != 0;
    };
    return std::none_of(recipe.npcVnums.begin(), recipe.npcVnums.end(), [](WORD vnum) { return vnum == 0; }) &&
           std::all_of(recipe.materials.begin(), recipe.materials.end(), valid) &&
           std::all_of(recipe.rewards.begin(), recipe.rewards.end(), valid);
}
}

bool Cube_load(const char* filename)
{
    if (!filename || !*filename)
        return false;
    std::unique_ptr<FILE, decltype(&std::fclose)> file(std::fopen(filename, "r"), std::fclose);
    if (!file)
        return false;

    std::unique_ptr<CUBE_DATA> current;
    char line[256];
    while (std::fgets(line, sizeof(line), file.get())) {
        if (line[0] == '#')
            continue;
        char* key = std::strtok(line, " \t\r\n");
        if (!key)
            continue;
        if (strcasecmp(key, "section") == 0) {
            current.reset(new CUBE_DATA);
            continue;
        }
        if (!current)
            continue;

        const char* first = std::strtok(nullptr, " \t\r\n");
        const char* second = std::strtok(nullptr, " \t\r\n");
        const int value = first ? std::atoi(first) : 0;
        const int count = second ? std::atoi(second) : 0;
        if (strcasecmp(key, "npc") == 0)
            current->npcVnums.push_back(static_cast<WORD>(value));
        else if (strcasecmp(key, "item") == 0)
            current->materials.push_back({static_cast<std::uint32_t>(value), count});
        else if (strcasecmp(key, "reward") == 0)
            current->rewards.push_back({static_cast<std::uint32_t>(value), count});
        else if (strcasecmp(key, "percent") == 0)
            current->percent = value;
        else if (strcasecmp(key, "end") == 0) {
            if (FN_check_cube_data(*current)) {
                recipes.push_back(std::move(current));
            } else {
                dev_log("cube.cpp", 0x159, "Cube_load", 0x800, const_cast<char*>("something wrong"));
                current.reset();
            }
        }
    }
    return true;
}

void Cube_init()
{
    const std::string filename = LocaleService_GetBasePath() + "/cube.txt";
    sys_log(0, "Cube_Init %s ", filename.c_str());
    recipes.clear();
    if (!Cube_load(filename.c_str()))
        _sys_err("Cube_init", 0x115, "<cube_init> fail");
}

void Cube_clean_item(CHARACTER* character)
{
    auto& items = character->m_pointsInstant.pCubeItems;
    std::fill(std::begin(items), std::end(items), nullptr);
}

namespace {
int CubeNpcDistance(CHARACTER* character, CHARACTER* npc)
{
    const auto dx = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(character->GetX()) - static_cast<std::uint32_t>(npc->GetX()));
    const auto dy = static_cast<std::int32_t>(
        static_cast<std::uint32_t>(character->GetY()) - static_cast<std::uint32_t>(npc->GetY()));
    return DISTANCE_APPROX(dx, dy);
}
}

void Cube_open(CHARACTER* character)
{
    CHARACTER* npc = character->GetQuestNPC();
    if (!npc) {
        if (test_server)
            dev_log("cube.cpp", 0xdc, "Cube_open", 0x800, const_cast<char*>("cube_npc is NULL"));
        return;
    }
    if (character->IsCubeOpen()) {
        character->ChatPacket(1, "%s", locale_find("\300\314\271\314\040\301\246\301\266\303\242\300\314\040\277\255\267\301\300\326\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    if (character->GetExchange() || character->GetMyShop() || character->GetShopOwner() || character->IsOpenSafebox()) {
        character->ChatPacket(1, "%s", locale_find("\264\331\270\245\040\260\305\267\241\301\337\050\303\242\260\355\054\261\263\310\257\054\273\363\301\241\051\277\241\264\302\040\273\347\277\353\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    const int distance = CubeNpcDistance(character, npc);
    if (distance >= 1000) {
        sys_log(1, "CUBE: TOO_FAR: %s distance %d", character->GetName(), distance);
        return;
    }
    auto& state = character->m_pointsInstant;
    Cube_clean_item(character);
    state.pCubeNpc = npc;
    character->ChatPacket(5, "cube open");
}

void Cube_close(CHARACTER* character)
{
    if (!character->IsCubeOpen())
        return;
    auto& state = character->m_pointsInstant;
    Cube_clean_item(character);
    state.pCubeNpc = nullptr;
    character->ChatPacket(5, "cube close");
    dev_log("cube.cpp", 0x100, "Cube_close", 0x800, const_cast<char*>("<CUBE> close (%s)"), character->GetName());
}

void Cube_add_item(CHARACTER* character, int cube, int inventory)
{
    if (!character->IsCubeOpen() || cube < 0 || cube >= 15 || inventory < 0 || inventory >= 90)
        return;
    CItem* item = character->GetItem(static_cast<BYTE>(inventory));
    if (!item || item->IsExchanging())
        return;
    auto& items = character->m_pointsInstant.pCubeItems;
    for (CItem*& selected : items) {
        if (selected == item) {
            selected = nullptr;
            break;
        }
    }
    items[cube] = item;
    if (test_server)
        character->ChatPacket(1, "cube[%d]: inventory[%d]: %s added", cube, inventory, item->GetName());
}

void Cube_delete_item(CHARACTER* character, int cube)
{
    if (!character->IsCubeOpen() || cube < 0 || cube >= 15)
        return;
    CItem*& selected = character->m_pointsInstant.pCubeItems[cube];
    CItem* item = selected;
    if (!item)
        return;
    selected = nullptr;
    if (test_server)
        character->ChatPacket(1, "cube[%d]: cube[%d]: %s deleted", cube, item->GetCell(), item->GetName());
}

void Cube_show_list(CHARACTER* character)
{
    if (!character->IsCubeOpen())
        return;
    const auto& items = character->m_pointsInstant.pCubeItems;
    for (int i = 0; i < 15; ++i) {
        if (CItem* item = items[i])
            character->ChatPacket(1, "cube[%d]: inventory[%d]: %s", i, item->GetCell(), item->GetName());
    }
}

namespace {
void FN_cube_print(const CUBE_DATA& recipe, int index)
{
    dev_log("cube.cpp", 0x169, "FN_cube_print", 0x800, const_cast<char*>("--------------------------------"));
    dev_log("cube.cpp", 0x16a, "FN_cube_print", 0x800, const_cast<char*>("CUBE_DATA[%d]"), index);
    for (std::size_t i = 0; i < recipe.npcVnums.size(); ++i)
        dev_log("cube.cpp", 0x16e, "FN_cube_print", 0x800, const_cast<char*>("\tNPC_VNUM[%d] = %d"),
                static_cast<int>(i), static_cast<int>(recipe.npcVnums[i]));
    for (std::size_t i = 0; i < recipe.materials.size(); ++i)
        dev_log("cube.cpp", 0x172, "FN_cube_print", 0x800, const_cast<char*>("\tITEM[%d]   = (%d, %d)"),
                static_cast<int>(i), static_cast<int>(recipe.materials[i].vnum), recipe.materials[i].count);
    for (std::size_t i = 0; i < recipe.rewards.size(); ++i)
        dev_log("cube.cpp", 0x176, "FN_cube_print", 0x800, const_cast<char*>("\tREWARD[%d] = (%d, %d)"),
                static_cast<int>(i), static_cast<int>(recipe.rewards[i].vnum), recipe.rewards[i].count);
    dev_log("cube.cpp", 0x178, "FN_cube_print", 0x800, const_cast<char*>("\tPERCENT = %d"), recipe.percent);
    dev_log("cube.cpp", 0x179, "FN_cube_print", 0x800, const_cast<char*>("--------------------------------"));
}
}

void Cube_print()
{
    for (std::size_t i = 0; i < recipes.size(); ++i)
        FN_cube_print(*recipes[i], static_cast<int>(i));
}

bool Cube_make(CHARACTER* character)
{
    if (!character->IsCubeOpen()) {
        character->ChatPacket(1, "%s", locale_find("\301\246\301\266\303\242\300\314\040\277\255\267\301\300\326\301\366\040\276\312\275\300\264\317\264\331", g_iUseLocale));
        return false;
    }
    CHARACTER* npc = character->GetQuestNPC();
    if (!npc || npc != character->m_pointsInstant.pCubeNpc ||
        CubeNpcDistance(character, npc) >= 1000)
        return false;

    if (CUBE_DATA* recipe = FN_find_cube(character->GetCubeItem(), npc->GetRaceNum())) {
        char* file = const_cast<char*>("cube.cpp");
        const CUBE_VALUE reward = *recipe->reward_value();
        if (reward.count <= 0 || reward.count > 255)
            return false;
        recipe->remove_material(character);

        if (recipe->percent < number_ex(1, 100, file, 0x1ad)) {
            character->ChatPacket(5, "cube fail");
            LogManager::instance().CubeLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                           reward.vnum, 0, 0, false);
            return false;
        }
        character->ChatPacket(5, "cube success %d %d", static_cast<int>(reward.vnum), reward.count);
        CItem* item = character->AutoGiveItem(reward.vnum, static_cast<BYTE>(reward.count), -1, true);
        if (!item) {
            character->ChatPacket(5, "cube fail");
            return false;
        }
        LogManager::instance().CubeLog(character->GetPlayerID(), character->GetX(), character->GetY(),
                                       reward.vnum, item->GetID(), reward.count, true);
        return true;
    }
    character->ChatPacket(1, "%s", locale_find("\301\246\301\266\040\300\347\267\341\260\241\040\272\316\301\267\307\325\264\317\264\331", g_iUseLocale));
    return false;
}

