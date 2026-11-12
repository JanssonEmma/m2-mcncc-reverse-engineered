#include "fishing.h"
#include "char_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "packet.h"
#include <cmath>
#include "locale_service.h"
#include "locale.hpp"
#include "questmanager.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <limits>
#include <map>
#include <string>

void SendLog(const char* text);
#include "char.h"
#include "config.h"
#include "item.h"
#include "item_manager.h"
#include "log.h"
#include "../../libthecore/include/log.h"
#include "../../libthecore/include/utils.h"

namespace fishing {
bool RefinableRod(LPITEM rod)
{
    return rod && rod->GetType() == 13 && !rod->IsEquipped() &&
           rod->GetSockets()[0] == rod->GetValue(2);
}

int RealRefineRod(LPCHARACTER character, LPITEM rod)
{
    if (!character || !rod || rod->GetOwner() != character)
        return 2;

    auto& logs = LogManager::instance();
    const char* name = rod->GetName();
    if (!RefinableRod(rod)) {
        _sys_err("RealRefineRod", 0x351, "REFINE_ROD_HACK pid(%lu) item(%s:%d)",
                 character->GetPlayerID(), name ? name : "", static_cast<int>(rod->GetID()));
        logs.Query("INSERT INTO refinelog%s VALUES(%lu, '%s', %lu, %d, NOW(), 1, 'ROD_HACK')",
                   get_table_postfix(), character->GetPlayerID(), name ? name : "", rod->GetID(), -1);
        return 2;
    }

    const int stage = static_cast<int>(rod->GetValue(0) / 10);
    const int roll = number_ex(1, 100, const_cast<char*>("fishing.cpp"), 0x35c);
    const bool success = roll <= rod->GetValue(3);
    logs.Query("INSERT INTO refinelog%s VALUES(%lu, '%s', %lu, %d, NOW(), %d, 'ROD')",
               get_table_postfix(), character->GetPlayerID(), name, rod->GetID(), stage, success);
    const DWORD replacementVnum = success ? rod->GetProto()->dwRefinedVnum : rod->GetValue(4);
    auto& items = ITEM_MANAGER::instance();
    LPITEM replacement = items.CreateItem(replacementVnum, 1, 0, false, -1);
    if (!replacement)
        return 2;

    const BYTE cell = rod->GetCell();
    items.RemoveItem(rod, "REMOVE (REFINE FISH_ROD)");
    replacement->AddToCharacter(character, cell);
    logs.ItemLog(character->GetPlayerID(), character->GetX(), character->GetY(), replacement->GetID(),
                 success ? "REFINE FISH_ROD SUCCESS" : "REFINE FISH_ROD FAIL", replacement->GetName());
    return success ? 1 : 0;
}
}

namespace fishing {
constexpr int fishCount = 37;
constexpr int probabilityCount = 4;

struct SFishInfo
{
    char name[64];
    std::uint32_t vnum;
    std::uint32_t dead_vnum;
    std::uint32_t grill_vnum;
    int prob[probabilityCount];
    int difficulty;
    int time_type;
    int length_range[3];
    int used_table[10];
};
static_assert(sizeof(SFishInfo) == 152, "fishing table record size");

SFishInfo fish_info[fishCount];
int g_prob_sum[probabilityCount];
int g_prob_accumulate[probabilityCount][fishCount];

bool operator<(const SFishInfo& lhs, const SFishInfo& rhs)
{
    return lhs.vnum < rhs.vnum;
}

namespace {
int ReadFishingNumber(const char* text)
{
    const long value = std::strtol(text, nullptr, 10);
    return static_cast<int>(std::max(static_cast<long>(std::numeric_limits<int>::min()),
        std::min(value, static_cast<long>(std::numeric_limits<int>::max()))));
}

void ReadFishingColumn(SFishInfo& fish, int index, const std::string& field)
{
    // The old trim helper looks one byte behind the start for empty input.
    char buffer[32] = {};
    char* column = buffer + 1;
    trim_and_lower(column, field.c_str(), 30);
    if (!*column)
        return;

    if (index == 0) {
        std::strncpy(fish.name, column, sizeof(fish.name) - 1);
        return;
    }

    const int value = ReadFishingNumber(column);
    switch (index) {
    case 1: fish.vnum = static_cast<std::uint32_t>(value); break;
    case 2: fish.dead_vnum = static_cast<std::uint32_t>(value); break;
    case 3: fish.grill_vnum = static_cast<std::uint32_t>(value); break;
    case 4: case 5: case 6: case 7:
        fish.prob[index - 4] = value;
        break;
    case 8: fish.difficulty = value; break;
    case 9: fish.time_type = value; break;
    case 10: case 11: case 12:
        fish.length_range[index - 10] = value;
        break;
    default:
        // In the original the first used result goes into index 1, leaving index 0 at zero.
        // Column 22 would write index 10 outside the record, so it has no valid destination.
        if (index >= 13 && index <= 21)
            fish.used_table[index - 12] = value;
        break;
    }
}

int SelectFish(int probability, int minimum, int sourceLine)
{
    if (probability < 0 || probability >= probabilityCount || g_prob_sum[probability] <= 0)
        return 0;
    const int roll = number_ex(minimum, g_prob_sum[probability],
                               const_cast<char*>("fishing.cpp"), sourceLine);
    const int* weights = g_prob_accumulate[probability];
    int first = 0;
    int count = fishCount;
    // The original search can receive nonmonotonic sums from signed weights.
    while (count > 0) {
        const int step = count / 2;
        const int middle = first + step;
        if (weights[middle] < roll) {
            first = middle + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first == fishCount ? 0 : first;
}
}

// 080821e0
void Initialize()
{
    const bool reload = fish_info[0].name[0] != '\0';
    const std::string path = LocaleService_GetBasePath() + "/fishing.txt";
    std::ifstream input(path);
    if (reload)
        SendLog("Reloading fish table.");
    if (!input) {
        SendLog("error! cannot open fishing.txt");
        if (reload)
            SendLog("  restoring to backup");
        return;
    }

    SFishInfo candidate[fishCount] = {};
    int row = 0;
    bool parseError = false;
    std::string line;
    while (row < fishCount && std::getline(input, line)) {
        if (!line.empty() && line[0] == '#')
            continue;
        auto tab = line.find('\t');
        if (tab == std::string::npos) {
            std::printf("Tab error on line: %s\n", line.c_str());
            SendLog("error! parsing fishing.txt");
            parseError = true;
            if (reload)
                SendLog("  restoring to backup");
            break;
        }

        int column = 0;
        std::size_t start = 0;
        while (tab != std::string::npos && column <= 22) {
            ReadFishingColumn(candidate[row], column++, line.substr(start, tab - start));
            start = tab + 1;
            tab = line.find('\t', start);
        }
        ++row;
    }

    if (!parseError || !reload)
        std::copy(candidate, candidate + fishCount, fish_info);

    for (const auto& fish : fish_info) {
        sys_log(0, "FISH: %-24s vnum %5lu prob %4d %4d %4d %4d len %d %d %d",
                fish.name, static_cast<unsigned long>(fish.vnum),
                fish.prob[0], fish.prob[1], fish.prob[2], fish.prob[3],
                fish.length_range[0], fish.length_range[1], fish.length_range[2]);
    }
    for (int probability = 0; probability < probabilityCount; ++probability) {
        std::uint32_t total = 0;
        for (int index = 0; index < fishCount; ++index) {
            total += static_cast<std::uint32_t>(fish_info[index].prob[probability]);
            g_prob_accumulate[probability][index] = static_cast<std::int32_t>(total);
        }
        g_prob_sum[probability] = g_prob_accumulate[probability][fishCount - 1];
        sys_log(0, "FISH: prob table %d %d", probability, g_prob_sum[probability]);
    }
}

int DetermineFishByProbIndex(int probability)
{
    return SelectFish(probability, 1, 0x149);
}

int GetProbIndexByMapIndex(int map)
{
    switch (map) {
    case 1: case 21: case 41:
        return 0;
    case 3: case 23: case 43:
        return 1;
    default:
        return -1;
    }
}

int DetermineFish(LPCHARACTER character)
{
    if (!character)
        return 0;
    int probability = GetProbIndexByMapIndex(character->GetMapIndex());
    if (probability < 0)
        return 0;

    auto& quests = quest::CQuestManager::instance();
    if (character->GetPremiumRemainSeconds(4) > 0 || character->IsEquipUniqueGroup(0x2719))
        probability = quests.GetEventFlag("manwoo") != 0 ? 3 : 2;

    int adjust = 0;
    if (quests.GetEventFlag("fish_miss_pct") != 0) {
        const int miss = std::max(0, std::min(quests.GetEventFlag("fish_miss_pct"), 200));
        const auto scaled = static_cast<std::int32_t>(
            static_cast<std::uint32_t>(100 - miss) * fish_info[0].prob[probability]);
        adjust = scaled / 100;
    }
    const int selected = SelectFish(probability, adjust + 1, 0x17f);
    const std::uint32_t vnum = fish_info[selected].vnum;
    if (LC_IsYMIR() && vnum - 0x11198U <= 12)
        return 0;
    if (g_iUseLocale && (vnum - 0xc358U < 2 || vnum == 0x13888U))
        return 0;
    return selected;
}
}

namespace fishing {
int aFishingTime[5][31] = {
    {0, 0, 0, 0, 0, 2, 4, 8, 12, 16, 20, 22, 25, 30, 50, 80, 50, 30, 25, 22, 20, 16, 12, 8, 4, 2, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 12, 16, 20, 22, 25, 30, 50, 80, 50, 30, 25, 22, 20},
    {20, 22, 25, 30, 50, 80, 50, 30, 25, 22, 20, 16, 12, 8, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {100, 0, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100},
    {20, 20, 20, 20, 20, 22, 24, 28, 32, 36, 40, 42, 45, 50, 70, 100, 70, 50, 45, 42, 40, 36, 32, 28, 24, 22, 20, 20, 20, 20, 20},
};

struct fishing_event_info
{
    std::uint32_t pid;
    int step;
    std::uint32_t hangTime;
    std::uint32_t fishId;
};
static_assert(sizeof(fishing_event_info) == 16, "fishing event payload size");

namespace {
const char practiceProgress[] = "\263\254\275\303\264\353\300\307 \274\366\267\303\265\265\260\241 \301\365\260\241\307\317\277\264\275\300\264\317\264\331! (%d/%d)";
const char practiceComplete[] = "\263\254\275\303\264\353\260\241 \303\326\264\353 \274\366\267\303\265\265\277\241 \265\265\264\336\307\317\277\264\275\300\264\317\264\331.";
const char practiceRefine[] = "\276\356\272\316\270\246 \305\353\307\330 \264\331\300\275 \267\271\272\247\300\307 \263\254\275\303\264\353\267\316 \276\367\261\327\267\271\300\314\265\345 \307\322 \274\366 \300\326\275\300\264\317\264\331.";
const char fishLengthNotice[] = "\300\314\271\370\277\241 \300\342\300\272 \271\260\260\355\261\342\300\307 \261\346\300\314\264\302 %.2fcm";
const char fishBoardFive[] = "\263\254\275\303\300\314\272\245\306\256\277\371\303\264\272\330\276\356";
const char fishBoardSix[] = "\263\254\275\303\300\314\272\245\306\256\300\327\276\356";

#pragma pack(push, 1)
struct FishingHighscorePacket
{
    std::uint32_t pid;
    std::int32_t length;
    char direction;
    char board[21];
};
#pragma pack(pop)
static_assert(sizeof(FishingHighscorePacket) == 30, "fishing highscore wire size");

void SendFishingAround(LPCHARACTER character, BYTE subheader, BYTE direction = 0)
{
    const TPacketGCFishing packet = {89, subheader, static_cast<std::uint32_t>(
        static_cast<DWORD>(character->GetVID())), direction};
    character->PacketAround(&packet, sizeof(packet));
}

void SendFishingItem(LPCHARACTER character, DWORD vnum)
{
    if (LPDESC descriptor = character->GetDesc()) {
        const TPacketGCFishing packet = {89, 5, static_cast<std::uint32_t>(vnum), 0};
        descriptor->Packet(&packet, sizeof(packet));
    }
}

int FishingRandom(int low, int high, int line)
{
    return number_ex(low, high, const_cast<char*>("fishing.cpp"), line);
}
}

void FishingReact(LPCHARACTER character)
{
    SendFishingAround(character, 2);
}

void FishingSuccess(LPCHARACTER character)
{
    SendFishingAround(character, 3);
}

void FishingFail(LPCHARACTER character)
{
    SendFishingAround(character, 4);
}

void FishingPractice(LPCHARACTER character)
{
    LPITEM rod = character->GetWear(4);
    if (!rod)
        return;
    if (rod->GetType() == 13 && rod->GetProto()->dwRefinedVnum &&
        static_cast<std::int32_t>(rod->GetSockets()[0]) < rod->GetValue(2) &&
        FishingRandom(1, rod->GetValue(1), 0x1b7) == 1) {
        const auto next = static_cast<std::int32_t>(static_cast<std::uint32_t>(rod->GetSockets()[0]) + 1u);
        rod->SetSocket(0, next);
        const int progress = static_cast<int>(rod->GetSockets()[0]);
        const int maximum = static_cast<int>(rod->GetValue(2));
        character->ChatPacket(1, locale_find(practiceProgress, g_iUseLocale), progress, maximum);
        if (static_cast<std::int32_t>(rod->GetSockets()[0]) == rod->GetValue(2)) {
            character->ChatPacket(1, locale_find(practiceComplete, g_iUseLocale));
            character->ChatPacket(1, locale_find(practiceRefine, g_iUseLocale));
        }
    }
    rod->SetSocket(2, 0);
}

bool PredictFish(LPCHARACTER character)
{
    return character->FindAffect(0xd0, 0) || character->GetPremiumRemainSeconds(4) > 0 ||
           character->IsEquipUniqueGroup(0x2719);
}

EVENTFUNC(fishing_event)
{
    auto* info = static_cast<fishing_event_info*>(event->info);
    LPCHARACTER character = CHARACTER_MANAGER::instance().FindByPID(info->pid);
    if (!character || character->m_pkFishingEvent != event)
        return 0;

    LPITEM rod = character->GetWear(4);
    if (!rod || rod->GetType() != 13) {
        character->m_pkFishingEvent = nullptr;
        return 0;
    }
    if (info->step != 0) {
        info->step = std::min(info->step + 1, 5);
        character->m_pkFishingEvent = nullptr;
        FishingFail(character);
        rod->SetSocket(2, 0);
        return 0;
    }

    info->step = 1;
    info->hangTime = static_cast<std::uint32_t>(get_dword_time());
    info->fishId = DetermineFish(character);
    FishingReact(character);
    if (PredictFish(character))
        SendFishingItem(character, fish_info[info->fishId].vnum);
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * 6u);
}

LPEVENT CreateFishingEvent(LPCHARACTER character)
{
    void* raw = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(fishing_event_info), &raw);
    *static_cast<fishing_event_info*>(raw) = {
        static_cast<std::uint32_t>(character->GetPlayerID()), 0, 0, 0,
    };
    const int seconds = FishingRandom(10, 40, 0x20c);
    const long double angle = static_cast<long double>(character->GetRotation()) / 5.0L;
    const BYTE direction = angle > -32769.0L && angle < 32768.0L
        ? static_cast<BYTE>(static_cast<std::int16_t>(angle))
        : BYTE{0};
    SendFishingAround(character, 0, direction);
    const auto delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * seconds);
    return event_create_ex(fishing_event, id, delay,
                            const_cast<char*>("fishing.cpp"), 0x215);
}

int GetFishingLevel(LPCHARACTER character)
{
    LPITEM rod = character->GetWear(4);
    if (!rod || rod->GetType() != 13)
        return 0;
    return static_cast<std::int32_t>(
        static_cast<std::uint32_t>(rod->GetValue(0)) + static_cast<std::uint32_t>(rod->GetSockets()[2]));
}

int Compute(DWORD fishId, DWORD milliseconds, DWORD* item, int level)
{
    if (!fishId)
        return -2;
    if (fishId >= fishCount) {
        _sys_err("Compute", 0x229, "Wrong FISH ID : %d", static_cast<int>(fishId));
        return -2;
    }
    const auto& fish = fish_info[fishId];
    if (milliseconds > 6000 || fish.time_type < 0 || fish.time_type >= 5)
        return -1;
    const DWORD step = std::min((milliseconds + 99) / 200, static_cast<DWORD>(30));
    if (aFishingTime[fish.time_type][step] < FishingRandom(1, 100, 0x232))
        return -1;
    if (FishingRandom(1, fish.difficulty, 0x234) > level)
        return -3;
    *item = fish.vnum;
    return 0;
}

void Simulation(int level, int count, int probability, CHARACTER* character)
{
    std::map<std::string, int> catches;
    int total = 0;
    for (int index = 0; index < count; ++index)
    {
        const DWORD fish = DetermineFishByProbIndex(probability);
        DWORD item = 0;
        char* const randomSource = const_cast<char*>("fishing.cpp");
        const int first = number_ex(2000, 4000, randomSource, 0x2e0);
        const int second = number_ex(2000, 4000, randomSource, 0x2e0);
        Compute(fish, (first + second) / 2, &item, level);
        if (item)
        {
            ++catches[fish_info[fish].name];
            ++total;
        }
    }
    for (const auto& entry : catches)
        character->ChatPacket(1, LC_TEXT("%s : %d \270\266\270\256"), entry.first.c_str(), entry.second);
    character->ChatPacket(1, LC_TEXT("%d \301\276\267\371 %d \270\266\270\256 \263\254\300\275"), static_cast<int>(catches.size()), total);
}

int GetFishLength(int fishId)
{
    const auto& range = fish_info[fishId].length_range;
    if (FishingRandom(0, 99, 0x241) == 0) {
        const double angle = std::asin(FishingRandom(0, 10000, 0x24d) / 10000.0);
        const auto span = static_cast<std::int32_t>(
            (static_cast<std::uint32_t>(range[2]) - range[1]) * 2u);
        const double extra = static_cast<double>(static_cast<long double>(angle) * span / 3.1415926535);
        const long double length = range[1] + static_cast<long double>(extra);
        if (length <= -2147483649.0L || length >= 2147483648.0L)
            return std::numeric_limits<std::int32_t>::min();
        return static_cast<std::int32_t>(length);
    }

    int roll = 0;
    for (int index = 0; index < 5; ++index)
        roll += FishingRandom(0, 2000, 0x246);
    const auto scaled = static_cast<std::int32_t>(
        (static_cast<std::uint32_t>(range[1]) - range[0]) * roll);
    return static_cast<std::int32_t>(static_cast<std::uint32_t>(range[0]) + scaled / 10000);
}

void Take(fishing_event_info* info, LPCHARACTER character)
{
    if (info->step <= 0) {
        SendFishingAround(character, 1);
        if (info->step != 0)
            FishingPractice(character);
        return;
    }

    const DWORD milliseconds = info->step == 1
        ? static_cast<std::uint32_t>(get_dword_time()) - info->hangTime : 7000;
    DWORD vnum = 0;
    const int status = info->step == 1 ? Compute(info->fishId, milliseconds, &vnum, GetFishingLevel(character)) : -1;
    if (status < -3 || status > 0) {
        FishingPractice(character);
        return;
    }
    const bool caught = status == 0 && vnum != 0;
    int length = 0;
    if (caught) {
        FishingSuccess(character);
        SendFishingItem(character, vnum);
        if (LPITEM fish = character->AutoGiveItem(vnum, 1, -1, false)) {
            fish->SetSocket(0, GetFishLength(info->fishId));
            length = fish->GetSockets()[0];
            if (test_server)
                character->ChatPacket(1, locale_find(fishLengthNotice, g_iUseLocale),
                                      static_cast<double>(static_cast<float>(length) / 100.0f));
            if (quest::CQuestManager::instance().GetEventFlag("fishevent") > 0 &&
                (info->fishId == 5 || info->fishId == 6)) {
                FishingHighscorePacket packet = {
                    static_cast<std::uint32_t>(character->GetPlayerID()), length, 0, {},
                };
                const char* board = locale_find(info->fishId == 5 ? fishBoardFive : fishBoardSix, g_iUseLocale);
                std::strncpy(packet.board, board, sizeof(packet.board) - 1);
                db_clientdesc->DBPacket(34, 0, &packet, sizeof(packet));
            }
        }
    }

    LogManager::instance().Query("INSERT INTO fish_log%s VALUES(NOW(), %lu, %d, %lu, %d, %d, %d, %d)",
        get_table_postfix(), character->GetPlayerID(), GetProbIndexByMapIndex(character->GetMapIndex()),
        static_cast<unsigned long>(info->fishId), GetFishingLevel(character),
        static_cast<int>(milliseconds), caught, length);
    if (!caught)
        FishingFail(character);
    FishingPractice(character);
}
}

namespace fishing {
void UseFish(LPCHARACTER character, LPITEM item)
{
    const std::uint32_t offset = static_cast<std::uint32_t>(item->GetVnum()) - fish_info[2].vnum;
    if (offset >= 35)
        return;

    const auto& fish = fish_info[offset + 2];
    const int roll = FishingRandom(1, 10000, 0x2f8);
    item->SetCount(item->GetCount() - 1);
    DWORD result;
    if (roll > 4000) {
        result = fish.dead_vnum;
    } else if (roll > 2000) {
        result = 27799;
    } else {
        static const int s_acc_prob[] = {1000, 1500, 1800, 1900, 1950, 1980, 1990, 1995, 1999, 2000};
        const auto index = std::lower_bound(s_acc_prob, s_acc_prob + 10, roll) - s_acc_prob;
        result = fish.used_table[index];
        switch (result) {
        case 0:
        case 2:
        case 3:
            character->ChatPacket(1, locale_find("\260\355\261\342\260\241 \310\347\300\373\265\265 \276\370\300\314 \273\347\266\363\301\375\264\317\264\331.", g_iUseLocale));
            return;
        case 1:
            character->ChatPacket(1, locale_find("\271\350 \274\323\277\241\274\255 \301\266\260\263\260\241 \263\252\277\324\275\300\264\317\264\331.", g_iUseLocale));
            result = 27987;
            break;
        case 4:
            character->ChatPacket(1, locale_find("\271\350 \274\323\277\241\274\255 \301\366\267\267\300\314\260\241 \263\252\277\324\275\300\264\317\264\331.", g_iUseLocale));
            result = 27801;
            break;
        }
    }
    character->AutoGiveItem(result, 1, -1, true);
}

void Grill(LPCHARACTER character, LPITEM item)
{
    const auto vnum = static_cast<std::uint32_t>(item->GetVnum());
    int index;
    if (vnum - 27803U < 28)
        index = vnum - 27800U;
    else if (vnum - 27833U < 28)
        index = vnum - 27830U;
    else
        return;

    const BYTE count = static_cast<BYTE>(item->GetCount());
    character->ChatPacket(1, locale_find("%s\270\246 \261\270\277\374\275\300\264\317\264\331.", g_iUseLocale), item->GetName());
    item->SetCount(0);
    character->AutoGiveItem(fish_info[index].grill_vnum, count, -1, true);
}
}
