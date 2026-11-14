#include "char.h"
#include "desc_manager.h"
#include "../../libthecore/include/main.h"
#include "party.h"
#include "arena.h"
#include "cube.h"
#include "affect.h"
#include <ctime>
#include <cstdio>
#include <cctype>
#include "config.h"
#include "dungeon.h"
#include "questlua_forked.h"
#include "sectree_manager.h"
#include "war_map.h"
#include "../../libthecore/include/log.h"
#include "constants.h"
#include "locale_service.h"
#include <cstring>
#include "char_manager.h"
#include "desc.h"
#include "desc_client.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include "guild.h"
#include "guild_manager.h"
#include "p2p.h"
#include "item_manager.h"
#include "questmanager.h"
#include "../../libthecore/include/hangul.h"
#include "locale.hpp"
#include "pvp.h"
#include "utils.h"

#include <cstdlib>

void do_fishing(CHARACTER* character, char* argument, int, int)
{
    char rotation[256];
    one_argument(argument, rotation);
    if (rotation[0]) {
        character->SetRotation(static_cast<float>(std::atof(rotation)));
        character->fishing();
    }
}

void do_safebox_close(CHARACTER* character, char*, int, int)
{
    character->CloseSafebox();
}

void do_safebox_password(CHARACTER* character, char* argument, int, int)
{
    char password[256];
    one_argument(argument, password);
    character->ReqSafeboxLoad(password);
}

void do_safebox_change_password(CHARACTER* character, char* argument, int, int)
{
    char oldPassword[256];
    char newPassword[256];
    two_arguments(argument, oldPassword, newPassword);
    if (!oldPassword[0] || !newPassword[0] || std::strlen(oldPassword) > 6 || std::strlen(newPassword) > 6) {
        character->ChatPacket(1, "%s", locale_find("\074\303\242\260\355\076\040\300\337\270\370\265\310\040\276\317\310\243\270\246\040\300\324\267\302\307\317\274\314\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    DESC* desc = character->GetDesc();
    if (!desc || !db_clientdesc)
        return;
    const std::uint32_t account = desc->GetAccountTable().id;
    char packet[18]{};
    std::memcpy(packet, &account, sizeof(account));
    std::memcpy(packet + 4, oldPassword, std::strlen(oldPassword));
    std::memcpy(packet + 11, newPassword, std::strlen(newPassword));
    db_clientdesc->DBPacket(16, desc->GetHandle(), packet, sizeof(packet));
}

void do_mall_password(CHARACTER* character, char* argument, int, int)
{
    char password[256];
    one_argument(argument, password);
    character->ReqMallLoad(password);
}

void do_mall_close(CHARACTER* character, char*, int, int)
{
    character->CloseMallFromClient();
}

void do_mobile(CHARACTER* character, char* argument, int, int)
{
    char number[256];
    one_argument(argument, number);
    if (character->GetLevel() < 20) {
        character->ChatPacket(1, "%s", locale_find("\267\271\272\247\040\062\060\040\300\314\307\317\264\302\040\307\332\265\345\306\371\040\271\370\310\243\270\246\040\265\356\267\317\307\322\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    character->RegisterMobile(number);
}

void do_mobile_auth(CHARACTER* character, char* argument, int, int)
{
    char code[256];
    one_argument(argument, code);
    character->AuthMobile(code);
}

static constexpr std::size_t HairNoticeSize = 256;

// 080acd20
static const char* FN_point_string(int point)
{
    static const struct { BYTE point; const char* format; } descriptions[] = {
        {6, "\303\326\264\353\040\273\375\270\355\267\302\040\053\045\144"},
        {8, "\303\326\264\353\040\301\244\275\305\267\302\040\053\045\144"},
        {10, "\303\326\264\353\040\301\366\261\270\267\302\040\053\045\144"},
        {12, "\261\331\267\302\040\053\045\144"},
        {13, "\303\274\267\302\040\053\045\144"},
        {14, "\271\316\303\270\040\053\045\144"},
        {15, "\301\366\264\311\040\053\045\144"},
        {17, "\260\370\260\335\274\323\265\265\040\053\045\144"},
        {19, "\300\314\265\277\274\323\265\265\040\045\144"},
        {21, "\304\360\305\270\300\323\040\055\045\144"},
        {22, "\270\266\271\375\040\260\370\260\335\267\302\040\053\045\144"},
        {23, "\270\266\271\375\040\271\346\276\356\267\302\040\053\045\144"},
        {32, "\273\375\270\355\267\302\040\310\270\272\271\040\053\045\144"},
        {33, "\301\244\275\305\267\302\040\310\270\272\271\040\053\045\144"},
        {37, "\265\266\260\370\260\335\040\045\144"},
        {38, "\275\272\305\317\040\053\045\144"},
        {39, "\275\275\267\316\277\354\040\053\045\144"},
        {40, "\045\144\045\045\040\310\256\267\374\267\316\040\304\241\270\355\305\270\040\260\370\260\335"},
        {41, "\045\144\045\045\040\310\256\267\374\267\316\040\260\374\305\353\040\260\370\260\335"},
        {43, "\300\316\260\243\267\371\040\270\363\275\272\305\315\040\305\270\260\335\304\241\040\053\045\144\045\045"},
        {44, "\265\277\271\260\267\371\040\270\363\275\272\305\315\040\305\270\260\335\304\241\040\053\045\144\045\045"},
        {45, "\277\365\261\315\301\267\040\305\270\260\335\304\241\040\053\045\144\045\045"},
        {46, "\271\320\261\263\267\371\040\305\270\260\335\304\241\040\053\045\144\045\045"},
        {47, "\275\303\303\274\267\371\040\305\270\260\335\304\241\040\053\045\144\045\045"},
        {48, "\276\307\270\266\267\371\040\305\270\260\335\304\241\040\053\045\144\045\045"},
        {53, "\270\363\275\272\305\315\277\241\260\324\040\260\255\307\324\040\053\045\144\045\045"},
        {54, "\271\253\273\347\277\241\260\324\040\260\255\307\324\040\053\045\144\045\045"},
        {55, "\300\332\260\264\277\241\260\324\040\260\255\307\324\040\053\045\144\045\045"},
        {56, "\274\366\266\363\277\241\260\324\040\260\255\307\324\040\053\045\144\045\045"},
        {57, "\271\253\264\347\277\241\260\324\040\260\255\307\324\040\053\045\144\045\045"},
        {59, "\271\253\273\347\260\370\260\335\277\241\040\045\144\045\045\040\300\372\307\327"},
        {60, "\300\332\260\264\260\370\260\335\277\241\040\045\144\045\045\040\300\372\307\327"},
        {61, "\274\366\266\363\260\370\260\335\277\241\040\045\144\045\045\040\300\372\307\327"},
        {62, "\271\253\264\347\260\370\260\335\277\241\040\045\144\045\045\040\300\372\307\327"},
        {63, "\305\270\260\335\304\241\040\045\144\045\045\040\270\246\040\273\375\270\355\267\302\300\270\267\316\040\310\355\274\366"},
        {64, "\305\270\267\302\304\241\040\045\144\045\045\040\270\246\040\301\244\275\305\267\302\300\270\267\316\040\310\355\274\366"},
        {65, "\045\144\045\045\040\310\256\267\374\267\316\040\305\270\260\335\275\303\040\273\363\264\353\040\300\374\275\305\267\302\040\274\322\270\360"},
        {66, "\045\144\045\045\040\310\256\267\374\267\316\040\307\307\307\330\275\303\040\301\244\275\305\267\302\040\310\270\272\271"},
        {67, "\271\260\270\256\305\270\260\335\275\303\040\272\355\267\260\040\310\256\267\374\040\045\144\045\045"},
        {68, "\310\260\040\260\370\260\335\040\310\270\307\307\040\310\256\267\374\040\045\144\045\045"},
        {69, "\307\321\274\325\260\313\040\271\346\276\356\040\045\144\045\045"},
        {70, "\276\347\274\325\260\313\040\271\346\276\356\040\045\144\045\045"},
        {71, "\265\316\274\325\260\313\040\271\346\276\356\040\045\144\045\045"},
        {72, "\271\346\277\357\040\271\346\276\356\040\045\144\045\045"},
        {73, "\272\316\303\244\040\271\346\276\356\040\045\144\045\045"},
        {74, "\310\260\260\370\260\335\040\300\372\307\327\040\045\144\045\045"},
        {75, "\310\255\277\260\040\300\372\307\327\040\045\144\045\045"},
        {76, "\300\374\261\342\040\300\372\307\327\040\045\144\045\045"},
        {77, "\270\266\271\375\040\300\372\307\327\040\045\144\045\045"},
        {78, "\271\331\266\367\040\300\372\307\327\040\045\144\045\045"},
        {79, "\301\367\301\242\040\305\270\260\335\304\241\040\271\335\273\347\040\310\256\267\374\040\072\040\045\144\045\045"},
        {80, "\300\372\301\326\040\265\307\265\271\270\256\261\342\040\310\256\267\374\040\045\144\045\045"},
        {81, "\265\266\040\300\372\307\327\040\045\144\045\045"},
        {82, "\045\144\045\045\040\310\256\267\374\267\316\040\300\373\305\360\304\241\275\303\040\301\244\275\305\267\302\040\310\270\272\271"},
        {83, "\045\144\045\045\040\310\256\267\374\267\316\040\300\373\305\360\304\241\275\303\040\260\346\307\350\304\241\040\303\337\260\241\040\273\363\275\302"},
        {84, "\045\144\045\045\040\310\256\267\374\267\316\040\300\373\305\360\304\241\275\303\040\265\267\040\062\271\350\040\265\345\267\323"},
        {85, "\045\144\045\045\040\310\256\267\374\267\316\040\300\373\305\360\304\241\275\303\040\276\306\300\314\305\333\040\062\271\350\040\265\345\267\323"},
        {86, "\271\260\276\340\040\273\347\277\353\275\303\040\045\144\045\045\040\274\272\264\311\040\301\365\260\241"},
        {87, "\045\144\045\045\040\310\256\267\374\267\316\040\300\373\305\360\304\241\275\303\040\273\375\270\355\267\302\040\310\270\272\271"},
        {95, "\260\370\260\335\267\302\040\053\045\144"},
        {96, "\271\346\276\356\267\302\040\053\045\144"},
        {114, "\260\370\260\335\267\302\040\053\045\144\045\045"},
        {115, "\271\346\276\356\267\302\040\053\045\144\045\045"},
        {116, "\260\346\307\350\304\241\040\045\144\045\045"},
        {117, "\276\306\300\314\305\333\040\265\345\267\323\300\262\040\045\056\061\146\271\350"},
        {118, "\265\267\040\265\345\267\323\300\262\040\045\056\061\146\271\350"},
        {119, "\303\326\264\353\040\273\375\270\355\267\302\040\053\045\144\045\045"},
        {120, "\303\326\264\353\040\301\244\275\305\267\302\040\053\045\144\045\045"},
        {121, "\275\272\305\263\040\265\245\271\314\301\366\040\045\144\045\045"},
        {122, "\306\362\305\270\040\265\245\271\314\301\366\040\045\144\045\045"},
        {123, "\275\272\305\263\040\265\245\271\314\301\366\040\300\372\307\327\040\045\144\045\045"},
        {124, "\306\362\305\270\040\265\245\271\314\301\366\040\300\372\307\327\040\045\144\045\045"},
    };
    for (const auto& description : descriptions) {
        if (description.point == point)
            return locale_find(description.format, g_iUseLocale);
    }
    return nullptr;
}

// 080ad0a0
static bool FN_hair_affect_string(CHARACTER* character, char* text)
{
    if (!character || !text)
        return false;
    const CAffect* effect = character->FindAffect(0x202, 0);
    if (!effect)
        return false;
    const auto expiry = static_cast<std::int32_t>(character->GetQuestFlag("hair.limit_time"));
    if (static_cast<std::int32_t>(get_global_time()) > expiry)
        return false;

    const char* format = FN_point_string(effect->bApplyOn);
    if (!format)
        return false;
    const auto value = static_cast<std::int32_t>(effect->lApplyValue);
    const int length = (effect->bApplyOn == 117 || effect->bApplyOn == 118)
        ? std::snprintf(text, HairNoticeSize, format, static_cast<double>(value))
        : std::snprintf(text, HairNoticeSize, format, value);
    if (length < 0 || length >= static_cast<int>(HairNoticeSize))
        return false;

    const time_t expires = expiry;
    struct tm date{};
    if (!localtime_r(&expires, &date))
        return false;
    std::snprintf(text + length, HairNoticeSize - length,
                  locale_find("\040\050\270\270\267\341\300\317\040\072\040\045\144\263\342\040\045\144\277\371\040\045\144\300\317\051", g_iUseLocale),
                  date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
    return true;
}

void do_hair(CHARACTER* character, char*, int, int)
{
    char text[HairNoticeSize];
    if (FN_hair_affect_string(character, text))
        character->ChatPacket(1, "%s", text);
}

void SendNotice(const char* text);

namespace {
struct SendDisconnectFunc {
    void operator()(DESC* desc) const
    {
        if (CHARACTER* character = desc->GetCharacter()) {
            if (!character->GetGMLevel())
                character->ChatPacket(5, "quit");
        }
    }
};

struct DisconnectFunc {
    void operator()(DESC* desc) const
    {
        if (desc->GetType() == 1 || desc->IsPhase(9))
            return;
        if (CHARACTER* character = desc->GetCharacter())
            character->Disconnect();
        desc->SetPhase(0);
    }
};
}

EVENTFUNC(shutdown_event)
{
    int& seconds = *static_cast<int*>(event->info);
    if (seconds < 0) {
        sys_log(0, "shutdown_event sec %d", seconds);
        --seconds;
        if (seconds == -10) {
            const auto& clients = DESC_MANAGER::instance().GetClientSet();
            std::for_each(clients.begin(), clients.end(), DisconnectFunc{});
        }
        return seconds < -10 ? 0 : passes_per_sec;
    }
    if (seconds == 0) {
        const auto& clients = DESC_MANAGER::instance().GetClientSet();
        std::for_each(clients.begin(), clients.end(), SendDisconnectFunc{});
        g_bNoMoreClient = true;
    } else {
        char notice[64];
        std::snprintf(notice, sizeof(notice), locale_find("\274\313\264\331\277\356\300\314\040\045\144\303\312\040\263\262\276\322\275\300\264\317\264\331\056", g_iUseLocale), seconds);
        SendNotice(notice);
    }
    --seconds;
    return passes_per_sec;
}

void Shutdown(int seconds)
{
    if (g_bNoMoreClient) {
        thecore_shutdown();
        return;
    }
    CWarMapManager::instance().OnShutdown();
    char notice[64];
    std::snprintf(notice, sizeof(notice), locale_find("\045\144\303\312\040\310\304\040\260\324\300\323\300\314\040\274\313\264\331\277\356\040\265\313\264\317\264\331\056", g_iUseLocale), seconds);
    SendNotice(notice);
    void* data = nullptr;
    const DWORD id = __AllocEventInfo(sizeof(int), &data);
    if (!data)
        return;
    *static_cast<int*>(data) = seconds;
    event_create_ex(shutdown_event, id, 1, const_cast<char*>("cmd_general.cpp"), 0xe8);
}

void do_shutdown(CHARACTER*, char*, int, int)
{
    const BYTE packet = 5;
    P2P_MANAGER::instance().Send(&packet, sizeof(packet));
    Shutdown(10);
}

void do_inputall(CHARACTER* character, char*, int, int)
{
    character->ChatPacket(1, "%s", locale_find("\270\355\267\311\276\356\270\246\040\270\360\265\316\040\300\324\267\302\307\317\274\274\277\344\056", g_iUseLocale));
}

void do_cube(CHARACTER* character, char* argument, int, int)
{
    if (!character->CanDoCube())
        return;
    char command[256], first[256], second[256];
    one_argument(two_arguments(argument, command, first), second);
    const auto digit = [](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); };
    switch (std::tolower(static_cast<unsigned char>(command[0]))) {
    case 'a':
        if (digit(first[0]) && digit(second[0]))
            Cube_add_item(character, std::atoi(first), std::atoi(second));
        break;
    case 'c': Cube_close(character); break;
    case 'd':
        if (digit(first[0]))
            Cube_delete_item(character, std::atoi(first));
        break;
    case 'l': Cube_show_list(character); break;
    case 'm':
        if (first[0])
            while (Cube_make(character)) {}
        else
            Cube_make(character);
        break;
    case 'o': Cube_open(character); break;
    case 0:
        character->ChatPacket(1, "Usage: cube open");
        character->ChatPacket(1, "       cube close");
        character->ChatPacket(1, "       cube add <inveltory_index>");
        character->ChatPacket(1, "       cube delete <cube_index>");
        character->ChatPacket(1, "       cube list");
        character->ChatPacket(1, "       cube cancel");
        character->ChatPacket(1, "       cube make [all]");
        break;
    }
}

void do_mount(CHARACTER*, char*, int, int)
{
    // This command has an empty body in the 2009 game.
}

void do_unmount(CHARACTER* character, char*, int, int)
{
    character->RemoveAffect(0xdd);
    if (character->IsHorseRiding()) {
        character->StopRiding();
        quest::CQuestManager::instance().Unmount(character->GetPlayerID());
    }
}

void do_user_horse_ride(CHARACTER* character, char*, int, int)
{
    if (character->IsObserverMode())
        return;
    if (character->IsHorseRiding()) {
        character->StopRiding();
    } else if (character->IsHorseVisible()) {
        character->StartRiding();
    } else {
        character->ChatPacket(1, "%s", locale_find("\270\273\300\273\040\270\325\300\372\040\274\322\310\257\307\330\301\326\274\274\277\344\056", g_iUseLocale));
    }
}

void do_user_horse_back(CHARACTER* character, char*, int, int)
{
    const char* notice;
    if (character->IsHorseVisible()) {
        character->HorseSummon(false, false);
        notice = "\270\273\300\273\040\265\271\267\301\272\270\263\302\275\300\264\317\264\331\056";
    } else {
        notice = character->IsHorseRiding() ? "\270\273\277\241\274\255\040\270\325\300\372\040\263\273\267\301\276\337\040\307\325\264\317\264\331\056" : "\270\273\300\273\040\270\325\300\372\040\274\322\310\257\307\330\301\326\274\274\277\344\056";
    }
    character->ChatPacket(1, "%s", locale_find(notice, g_iUseLocale));
}

void do_user_horse_feed(CHARACTER* character, char*, int, int)
{
    if (character->GetMyShop())
        return;
    if (!character->IsHorseVisible()) {
        const char* notice = character->IsHorseRiding() ? "\270\273\300\273\040\305\272\040\273\363\305\302\277\241\274\255\264\302\040\270\324\300\314\270\246\040\301\331\040\274\366\040\276\370\275\300\264\317\264\331\056" : "\270\273\300\273\040\270\325\300\372\040\274\322\310\257\307\330\301\326\274\274\277\344\056";
        character->ChatPacket(1, "%s", locale_find(notice, g_iUseLocale));
        return;
    }

    const DWORD food = character->GetHorseGrade() + 0xc385;
    if (character->CountSpecifyItem(food) < 1) {
        const TItemTable* proto = ITEM_MANAGER::instance().GetTable(food);
        const char* name = proto ? proto->szLocaleName : "";
        character->ChatPacket(1, locale_find("\045\163\040\276\306\300\314\305\333\300\314\040\307\312\277\344\307\325\264\317\264\331", g_iUseLocale), name);
        return;
    }
    character->RemoveSpecifyItem(food, 1);
    character->FeedHorse();
    const char* suffix = "";
    if (g_iUseLocale == 0) {
        const TItemTable* proto = ITEM_MANAGER::instance().GetTable(food);
        if (proto)
            suffix = locale_find(under_han(proto->szLocaleName) ? "\300\273" : "\270\246", g_iUseLocale);
    }
    const TItemTable* proto = ITEM_MANAGER::instance().GetTable(food);
    const char* name = proto ? proto->szLocaleName : "";
    character->ChatPacket(1, locale_find("\270\273\277\241\260\324\040\045\163\045\163\040\301\326\276\372\275\300\264\317\264\331\056", g_iUseLocale), name, suffix);
}

void do_close_shop(CHARACTER* character, char*, int, int)
{
    if (character->GetMyShop())
        character->CloseMyShop();
}

namespace
{
struct CommandStat
{
    const char* name;
    BYTE point;
    int TJobInitialPoints::* initial;
};

const CommandStat* FindCommandStat(CHARACTER& character, char* argument)
{
    char name[256];
    one_argument(argument, name);
    if (!name[0])
        return nullptr;
    if (character.IsPolymorphed())
    {
        character.ChatPacket(1, "%s", locale_find("\265\320\260\251 \301\337\277\241\264\302 \264\311\267\302\300\273 \277\303\270\261 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return nullptr;
    }
    static constexpr CommandStat stats[] = {
        {"st", POINT_ST, &TJobInitialPoints::st},
        {"dx", POINT_DX, &TJobInitialPoints::dx},
        {"ht", POINT_HT, &TJobInitialPoints::ht},
        {"iq", POINT_IQ, &TJobInitialPoints::iq},
    };
    for (const CommandStat& stat : stats)
    {
        if (std::strcmp(stat.name, name) == 0)
            return &stat;
    }
    return nullptr;
}
}

void do_stat(CHARACTER* character, char* argument, int, int)
{
    const CommandStat* stat = FindCommandStat(*character, argument);
    if (!stat || character->GetPoint(POINT_STAT) <= 0)
        return;

    const long value = character->GetRealPoint(stat->point);
    if (value >= 90)
        return;
    if (LC_IsJapan())
    {
        const int level = character->GetLevel();
        if (value >= (level < 6 ? 10 : level * 2))
            return;
    }

    character->SetRealPoint(stat->point, value + 1);
    character->SetPoint(stat->point, character->GetPoint(stat->point) + 1);
    character->ComputePoints();
    character->PointChange(stat->point, 0, false, false);
    if (stat->point == POINT_IQ)
        character->PointChange(POINT_MAX_HP, 0, false, false);
    else if (stat->point == POINT_HT)
        character->PointChange(POINT_MAX_SP, 0, false, false);
    character->PointChange(POINT_STAT, -1, false, false);
    character->ComputePoints();
}

void do_stat_minus(CHARACTER* character, char* argument, int, int)
{
    const CommandStat* stat = FindCommandStat(*character, argument);
    if (!stat || character->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
        return;

    const BYTE job = character->GetJob();
    const long value = character->GetRealPoint(stat->point);
    if (job >= JOB_MAX_NUM || value <= JobInitialPoints[job].*(stat->initial))
        return;

    character->SetRealPoint(stat->point, value - 1);
    character->SetPoint(stat->point, character->GetPoint(stat->point) - 1);
    character->ComputePoints();
    character->PointChange(stat->point, 0, false, false);
    if (stat->point == POINT_HT)
        character->PointChange(POINT_MAX_HP, 0, false, false);
    else if (stat->point == POINT_IQ)
        character->PointChange(POINT_MAX_SP, 0, false, false);
    character->PointChange(POINT_STAT, 1, false, false);
    character->PointChange(POINT_STAT_RESET_COUNT, -1, false, false);
    character->ComputePoints();
}

extern int g_nPortalLimitTime;

void do_restart(CHARACTER* character, char*, int, int subcommand)
{
    if (!character->IsDead())
    {
        character->ChatPacket(5, "CloseRestartWindow");
        character->StartRecoveryEvent();
        return;
    }
    if (subcommand != 0 && subcommand != 1)
        return;

    const bool town = subcommand == 0;
    const int remaining = character->m_pkDeadEvent && passes_per_sec > 0
        ? event_time(character->m_pkDeadEvent) / passes_per_sec : 0;
    CWarMap* const war = character->GetWarMap();
    const bool usualDelay = !war || war->GetType() == 2;
    const long mapIndex = character->GetMapIndex();
    const auto wait = [character](const char* key, int seconds) {
        character->ChatPacket(1, locale_find(key, g_iUseLocale), seconds);
    };
    if (town)
    {
        if (character->IsHack(true, true, g_nPortalLimitTime) &&
            (usualDelay || !IsSungziMapIndex(mapIndex)))
        {
            wait("\276\306\301\367 \300\347\275\303\300\333 \307\322 \274\366 \276\370\275\300\264\317\264\331.(%d\303\312 \263\262\300\275)", remaining + g_nPortalLimitTime - 180);
            return;
        }
        if (remaining > 173)
        {
            wait("\276\306\301\367 \270\266\300\273\277\241\274\255 \300\347\275\303\300\333 \307\322 \274\366 \276\370\275\300\264\317\264\331. (%d \303\312 \263\262\300\275)", remaining - 173);
            return;
        }
    }
    else if (usualDelay && !test_server)
    {
        if (character->IsHack(true, true, g_nPortalLimitTime) && !IsSungziMapIndex(mapIndex))
        {
            wait("\276\306\301\367 \300\347\275\303\300\333 \307\322 \274\366 \276\370\275\300\264\317\264\331.(%d\303\312 \263\262\300\275 )", remaining + g_nPortalLimitTime - 180);
            return;
        }
        if (remaining > 170)
        {
            wait("\276\306\301\367 \300\347\275\303\300\333 \307\322 \274\366 \276\370\275\300\264\317\264\331.(%d\303\312 \263\262\300\275)", remaining - 170);
            return;
        }
    }

    DESC* desc = character->GetDesc();
    if (!desc)
        return;
    character->ChatPacket(5, "CloseRestartWindow");
    desc->SetPhase(5);
    character->SetPosition(POS_STANDING);
    character->StartRecoveryEvent();

    const BYTE empire = character->GetEmpire();
    const BYTE startIndex = empire >= 1 && empire <= 3 ? empire : 0;
    const auto pointDelta = [](int target, int current) {
        const std::uint32_t bits = static_cast<std::uint32_t>(target) - static_cast<std::uint32_t>(current);
        std::int32_t result;
        std::memcpy(&result, &bits, sizeof(result));
        return result;
    };
    const auto restoreVitals = [character, pointDelta]() {
        character->PointChange(POINT_HP, pointDelta(character->GetMaxHP(), character->GetHP()), false, false);
        character->PointChange(POINT_SP, pointDelta(character->GetMaxSP(), character->GetSP()), false, false);
    };

    const bool sungzi = IsSungziMapIndex(mapIndex);
    if ((sungzi || IsForkedMapIndex(mapIndex)) &&
        quest::CQuestManager::instance().GetEventFlag("threeway_war") == 1)
    {
        if (!sungzi)
        {
            character->WarpSet(g_start_position[startIndex][0], g_start_position[startIndex][1], 0);
            character->ReviveInvisible(5);
            restoreVitals();
            return;
        }
        if (character->GetKillCount() > 0)
        {
            character->Show(mapIndex, GetSungziStartX(empire), GetSungziStartY(empire), 0x7fffffff, false);
            restoreVitals();
            character->ReviveInvisible(5);
            if (test_server)
                sys_log(0, "\273\357\260\305\270\256 \274\272\301\366 \272\316\307\322 HP %d SP %d ", character->GetHP(), character->GetSP());
            return;
        }
        character->ChatPacket(1, "%s", locale_find("\274\272\301\366\277\241\274\255 \272\316\310\260 \261\342\310\270\270\246 \270\360\265\316 \300\322\276\372\275\300\264\317\264\331! \270\266\300\273\267\316 \300\314\265\277\307\325\264\317\264\331!", g_iUseLocale));
        character->WarpSet(g_start_position[startIndex][0], g_start_position[startIndex][1], 0);
        restoreVitals();
        character->ReviveInvisible(5);
        return;
    }

    if (CDungeon* dungeon = character->GetDungeon())
        dungeon->UseRevive(character);
    const DWORD opponent = war && !character->IsObserverMode() ? war->GetGuildOpponent(character) : 0;
    PIXEL_POSITION position;
    if (!opponent)
    {
        if (town)
        {
            sys_log(0, "do_restart: restart town");
            if (!SECTREE_MANAGER::instance().GetRecallPositionByEmpire(mapIndex, empire, position))
            {
                position.x = g_start_position[startIndex][0];
                position.y = g_start_position[startIndex][1];
            }
            character->WarpSet(position.x, position.y, 0);
            character->PointChange(POINT_HP, pointDelta(50, character->GetHP()), false, false);
            character->DeathPenalty(1);
            return;
        }
        sys_log(0, "do_restart: restart here");
        character->ViewReencode();
        character->PointChange(POINT_HP, pointDelta(50, character->GetHP()), false, false);
        character->DeathPenalty(0);
    }
    else
    {
        if (town)
        {
            sys_log(0, "do_restart: restart town");
            const BYTE team = opponent <= character->GetGuild()->GetID();
            if (CWarMapManager::instance().GetStartPosition(mapIndex, team, position))
                character->Show(mapIndex, position.x, position.y, 0x7fffffff, false);
            else
                character->ExitToSavedLocation();
        }
        else
        {
            sys_log(0, "do_restart: restart here");
            character->ViewReencode();
        }
        restoreVitals();
    }
    character->ReviveInvisible(5);
}

void do_set_walk_mode(CHARACTER* character, char*, int, int)
{
    character->SetWalkingMode(true);
}

void do_set_run_mode(CHARACTER* character, char*, int, int)
{
    character->SetWalkingMode(false);
}

void do_pkmode(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (!arg[0])
        return;
    const int mode = std::atoi(arg);
    if (mode != 3)
        character->SetPKMode(static_cast<BYTE>(mode));
}

void do_setblockmode(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (arg[0])
        character->SetBlockMode(static_cast<BYTE>(std::atoi(arg)));
}

namespace
{
struct TimedExitInfo
{
    std::uint32_t pid;
    int subcommand;
    int seconds;
};

}

long timed_event(LPEVENT event)
{
    auto& info = *static_cast<TimedExitInfo*>(event->info);
    CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(info.pid);
    if (!character || character->m_pkTimedEvent != event)
        return 0;

    if (info.seconds > 0)
    {
        character->ChatPacket(1, locale_find("%d\303\312 \263\262\276\322\275\300\264\317\264\331.", g_iUseLocale), info.seconds);
        --info.seconds;
        return passes_per_sec;
    }

    character->m_pkTimedEvent = nullptr;
    DESC* desc = character->GetDesc();
    switch (info.subcommand)
    {
        case 0:
            if (desc)
                desc->SetPhase(0);
            break;
        case 1:
            character->ChatPacket(5, "quit");
            break;
        case 2:
            if (desc)
            {
                character->Disconnect();
                desc->SetPhase(3);
            }
            break;
    }
    return 0;
}

void do_console(CHARACTER* character, char*, int, int)
{
    character->ChatPacket(5, "ConsoleEnable");
}

void do_cmd(CHARACTER* character, char*, int, int subcommand)
{
    if (character->m_pkTimedEvent)
    {
        character->ChatPacket(1, "%s", locale_find("\303\353\274\322 \265\307\276\372\275\300\264\317\264\331.", g_iUseLocale));
        event_cancel(&character->m_pkTimedEvent);
        return;
    }

    const char* const notices[] = {"\267\316\261\327\300\316 \310\255\270\351\300\270\267\316 \265\271\276\306 \260\251\264\317\264\331. \300\341\275\303\270\270 \261\342\264\331\270\256\274\274\277\344.", "\260\324\300\323\300\273 \301\276\267\341 \307\325\264\317\264\331. \300\341\275\303\270\270 \261\342\264\331\270\256\274\274\277\344.", "\304\263\270\257\305\315\270\246 \300\374\310\257 \307\325\264\317\264\331. \300\341\275\303\270\270 \261\342\264\331\270\256\274\274\277\344."};
    if (subcommand < 0 || subcommand >= 3)
        return;
    character->ChatPacket(1, "%s", locale_find(notices[subcommand], g_iUseLocale));

    if (character->IsHack(false, true, 10) && !IsSungziMapIndex(character->GetMapIndex()))
    {
        CWarMap* war = character->GetWarMap();
        if (!war || war->GetType() == 2)
            return;
    }

    void* data;
    const DWORD id = __AllocEventInfo(sizeof(TimedExitInfo), &data);
    auto* info = static_cast<TimedExitInfo*>(data);
    info->pid = character->GetPlayerID();
    info->subcommand = subcommand;
    info->seconds = character->GetPosition() == POS_FIGHTING ? 10 : 3;
    character->m_pkTimedEvent = event_create_ex(timed_event, id, 1,
                                               const_cast<char*>("cmd_general.cpp"), 0x159);
}

void do_ungroup(CHARACTER* character, char*, int, int)
{
    CParty* party = character->GetParty();
    if (!party)
        return;
    CPartyManager& manager = CPartyManager::instance();
    if (!manager.IsEnablePCParty())
        character->ChatPacket(1, "%s", locale_find("<\306\304\306\274> \274\255\271\366 \271\256\301\246\267\316 \306\304\306\274 \260\374\267\303 \303\263\270\256\270\246 \307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
    else if (character->GetDungeon())
        character->ChatPacket(1, "%s", locale_find("<\306\304\306\274> \264\370\300\374 \276\310\277\241\274\255\264\302 \306\304\306\274\277\241\274\255 \263\252\260\245 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
    else if (party->GetMemberCount() == 2)
        manager.DeleteParty(party);
    else
    {
        character->ChatPacket(1, "%s", locale_find("<\306\304\306\274> \306\304\306\274\277\241\274\255 \263\252\260\241\274\314\275\300\264\317\264\331.", g_iUseLocale));
        party->Quit(character->GetPlayerID());
    }
}

void do_observer_exit(CHARACTER* character, char*, int, int)
{
    if (!character->IsObserverMode())
        return;
    if (character->GetWarMap())
        character->SetWarMap(nullptr);

    CArena* arena = character->GetArena();
    if (arena || character->GetArenaObserverMode())
    {
        character->SetArenaObserverMode(false);
        if (arena)
            arena->RemoveObserver(character->GetPlayerID());
        character->SetArena(nullptr);
        const BYTE empire = character->GetEmpire();
        const long* position = arena_return_position[empire >= 1 && empire <= 3 ? empire : 0];
        character->WarpSet(position[0], position[1], 0);
    }
    else
        character->ExitToSavedLocation();
    character->SetObserverMode(false);
}

void do_view_equip(CHARACTER* character, char* argument, int, int)
{
    if (!character->GetGMLevel())
        return;
    char arg[256];
    one_argument(argument, arg);
    if (!arg[0])
        return;
    CHARACTER* target = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(std::atoi(arg)));
    if (target && target->GetDesc())
        target->SendEquipment(character);
}

void do_party_request(CHARACTER* character, char* argument, int, int)
{
    if (character->GetArena())
    {
        character->ChatPacket(1, "%s", locale_find("\264\353\267\303\300\345\277\241\274\255 \273\347\277\353\307\317\275\307 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return;
    }
    if (character->GetParty())
    {
        character->ChatPacket(1, "%s", locale_find("\300\314\271\314 \306\304\306\274\277\241 \274\323\307\330 \300\326\300\270\271\307\267\316 \260\241\300\324\275\305\303\273\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.", g_iUseLocale));
        return;
    }

    char arg[256];
    one_argument(argument, arg);
    if (!arg[0])
        return;
    CHARACTER* leader = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(std::atoi(arg)));
    if (leader && !character->RequestToParty(leader))
        character->ChatPacket(5, "PartyRequestDenied");
}

void do_party_request_accept(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (arg[0])
    {
        if (CHARACTER* member = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(std::atoi(arg))))
            character->AcceptToParty(member);
    }
}

void do_party_request_deny(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (arg[0])
    {
        if (CHARACTER* member = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(std::atoi(arg))))
            character->DenyToParty(member);
    }
}

namespace
{
void GuildWarNotice(CHARACTER* character, const char* key)
{
    character->ChatPacket(1, "%s", locale_find(key, g_iUseLocale));
}

bool GuildMasterOnline(CGuild* guild)
{
    return guild->GetMasterCharacter() || P2P_MANAGER::instance().FindByPID(guild->GetMasterPID());
}
}

void do_war(CHARACTER* character, char* argument, int, int)
{
    CGuild* guild = character->GetGuild();
    if (!guild)
        return;
    if (guild->UnderAnyWar(3))
    {
        GuildWarNotice(character, "<\261\346\265\345> \300\314\271\314 \264\331\270\245 \300\374\300\357\277\241 \302\374\300\374 \301\337 \300\324\264\317\264\331.");
        return;
    }

    char name[256], mode[256];
    two_arguments(argument, name, mode);
    if (!name[0])
        return;
    if (guild->GetMasterPID() != character->GetPlayerID())
    {
        GuildWarNotice(character, "<\261\346\265\345> \261\346\265\345\300\374\277\241 \264\353\307\321 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
        return;
    }

    CGuild* opponent = CGuildManager::instance().FindGuildByName(name);
    if (!opponent)
    {
        GuildWarNotice(character, "<\261\346\265\345> \261\327\267\261 \261\346\265\345\260\241 \276\370\275\300\264\317\264\331.");
        return;
    }

    const int requestedType = mode[0] ? std::atoi(mode) : 0;
    if (requestedType < 0)
        return;
    const BYTE type = static_cast<BYTE>(requestedType > 2 ? 0 : requestedType);
    const DWORD opponentId = opponent->GetID();
    switch (guild->GetGuildWarState(opponentId))
    {
        case 0: {
            if (opponent->UnderAnyWar(3))
            {
                GuildWarNotice(character, "<\261\346\265\345> \273\363\264\353\271\346 \261\346\265\345\260\241 \300\314\271\314 \300\374\300\357 \301\337 \300\324\264\317\264\331.");
                return;
            }
            const int price = guild->GetWarPrice(type);
            if (guild->GetGuildGold() < price)
            {
                GuildWarNotice(character, "<\261\346\265\345> \300\374\272\361\260\241 \272\316\301\267\307\317\277\251 \261\346\265\345\300\374\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.");
                return;
            }
            if (opponent->GetGuildGold() < price)
            {
                GuildWarNotice(character, "<\261\346\265\345> \273\363\264\353\271\346 \261\346\265\345\300\307 \300\374\272\361\260\241 \272\316\301\267\307\317\277\251 \261\346\265\345\300\374\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.");
                return;
            }
            break;
        }
        case 1:
            GuildWarNotice(character, "\300\314\271\314 \274\261\300\374\306\367\260\355 \301\337\300\316 \261\346\265\345\300\324\264\317\264\331.");
            return;
        case 3:
            if (opponent->UnderAnyWar(3))
            {
                GuildWarNotice(character, "<\261\346\265\345> \273\363\264\353\271\346 \261\346\265\345\260\241 \300\314\271\314 \300\374\300\357 \301\337 \300\324\264\317\264\331.");
                guild->RequestRefuseWar(opponentId);
                return;
            }
            break;
        case 7:
            return;
        case 9:
            GuildWarNotice(character, "<\261\346\265\345> \300\314\271\314 \300\374\300\357\300\314 \277\271\276\340\265\310 \261\346\265\345 \300\324\264\317\264\331.");
            return;
        default:
            GuildWarNotice(character, "<\261\346\265\345> \300\314\271\314 \300\374\300\357 \301\337\300\316 \261\346\265\345\300\324\264\317\264\331.");
            guild->RequestRefuseWar(opponentId);
            return;
    }

    if (!guild->CanStartWar(type))
    {
        if (!guild->GetLadderPoint())
        {
            GuildWarNotice(character, "<\261\346\265\345> \267\271\264\365 \301\241\274\366\260\241 \270\360\300\332\266\363\274\255 \261\346\265\345\300\374\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.");
            sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
        }
        else if (guild->GetMemberCount() < 8)
        {
            character->ChatPacket(1, locale_find("<\261\346\265\345> \261\346\265\345\300\374\300\273 \307\317\261\342 \300\247\307\330\274\261 \303\326\274\322\307\321 %d\270\355\300\314 \300\326\276\356\276\337 \307\325\264\317\264\331.", g_iUseLocale), 8);
            sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", 8);
        }
        else
            sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
        return;
    }

    if (!opponent->CanStartWar(0))
    {
        if (!opponent->GetLadderPoint())
            GuildWarNotice(character, "<\261\346\265\345> \273\363\264\353\271\346 \261\346\265\345\300\307 \267\271\264\365 \301\241\274\366\260\241 \270\360\300\332\266\363\274\255 \261\346\265\345\300\374\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.");
        else if (opponent->GetMemberCount() < 8)
            GuildWarNotice(character, "<\261\346\265\345> \273\363\264\353\271\346 \261\346\265\345\300\307 \261\346\265\345\277\370 \274\366\260\241 \272\316\301\267\307\317\277\251 \261\346\265\345\300\374\300\273 \307\322 \274\366 \276\370\275\300\264\317\264\331.");
        return;
    }

    if (!GuildMasterOnline(guild) || !GuildMasterOnline(opponent))
    {
        GuildWarNotice(character, "<\261\346\265\345> \273\363\264\353\271\346 \261\346\265\345\300\307 \261\346\265\345\300\345\300\314 \301\242\274\323\301\337\300\314 \276\306\264\325\264\317\264\331.");
        guild->RequestRefuseWar(opponentId);
        return;
    }
    guild->RequestDeclareWar(opponentId, type);
}

void do_nowar(CHARACTER* character, char* argument, int, int)
{
    CGuild* guild = character->GetGuild();
    if (!guild)
        return;
    char name[256];
    one_argument(argument, name);
    if (!name[0])
        return;
    if (guild->GetMasterPID() != character->GetPlayerID())
    {
        GuildWarNotice(character, "<\261\346\265\345> \261\346\265\345\300\374\277\241 \264\353\307\321 \261\307\307\321\300\314 \276\370\275\300\264\317\264\331.");
        return;
    }
    if (CGuild* opponent = CGuildManager::instance().FindGuildByName(name))
        guild->RequestRefuseWar(opponent->GetID());
    else
        GuildWarNotice(character, "<\261\346\265\345> \261\327\267\261 \261\346\265\345\260\241 \276\370\275\300\264\317\264\331.");
}

void do_stat_reset(CHARACTER* character, char*, int, int)
{
    character->PointChange(POINT_STAT_RESET_COUNT, 12 - character->GetPoint(POINT_STAT_RESET_COUNT), false, false);
}

void do_guildskillup(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (!arg[0])
        return;
    CGuild* guild = character->GetGuild();
    if (!guild) {
        character->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\261\346\265\345\277\241\040\274\323\307\330\300\326\301\366\040\276\312\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    TGuildMember* member = guild->GetMember(character->GetPlayerID());
    if (!member || member->grade != 1) {
        character->ChatPacket(1, "%s", locale_find("\074\261\346\265\345\076\040\261\346\265\345\040\275\272\305\263\040\267\271\272\247\300\273\040\272\257\260\346\307\322\040\261\307\307\321\300\314\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    guild->SkillLevelUp(static_cast<DWORD>(std::atoi(arg)));
}

void do_skillup(CHARACTER* character, char* argument, int, int)
{
    char arg[256];
    one_argument(argument, arg);
    if (arg[0])
        character->SkillLevelUp(static_cast<DWORD>(std::atoi(arg)), 0);
}

void do_pvp(CHARACTER* character, char* argument, int, int)
{
    if (character->GetArena()) {
        character->ChatPacket(1, "%s", locale_find("\264\353\267\303\300\345\277\241\274\255\040\273\347\277\353\307\317\275\307\040\274\366\040\276\370\275\300\264\317\264\331\056", g_iUseLocale));
        return;
    }
    char arg[256];
    one_argument(argument, arg);
    CHARACTER* target = CHARACTER_MANAGER::instance().Find(static_cast<DWORD>(std::atoi(arg)));
    if (!target || target->GetCharType() != CHAR_TYPE_PC)
        return;
    if (target->GetArena()) {
        target->ChatPacket(1, "%s", locale_find("\273\363\264\353\271\346\300\314\040\264\353\267\303\301\337\300\324\264\317\264\331\056", g_iUseLocale));
        return;
    }
    CPVPManager::instance().Insert(character, target);
}
