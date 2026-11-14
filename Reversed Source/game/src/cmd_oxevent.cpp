#include "OXEvent.h"
#include "char.h"
#include "config.h"
#include "locale.hpp"
#include "locale_service.h"
#include "p2p.h"
#include "questmanager.h"
#include "utils.h"
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

void SendNotice(const char* text);

namespace
{
void BroadcastOXNotice(const char* text)
{
    const std::uint32_t textLength = static_cast<std::uint32_t>(std::strlen(text) + 1);
    std::vector<BYTE> packet(5 + textLength);
    packet[0] = 4;
    std::memcpy(packet.data() + 1, &textLength, sizeof(textLength));
    std::memcpy(packet.data() + 5, text, textLength);
    P2P_MANAGER::instance().Send(packet.data(), packet.size());
    SendNotice(text);
}

}

EVENTFUNC(end_oxevent)
{
    (void)event;
    COXEventManager::instance().CloseEvent();
    return 0;
}

void do_oxevent_open(CHARACTER* character, char*, int, int)
{
    COXEventManager& manager = COXEventManager::instance();
    const auto status = manager.GetStatus();
    if (status != OXEVENT_FINISH)
    {
        character->ChatPacket(1, LC_TEXT("OX \300\314\272\245\306\256\260\241 \300\314\271\314 \301\370\307\340\301\337\300\324\264\317\264\331(flag: %d)"), status);
        return;
    }

    COXEventManager::instance().ClearQuiz();
    char script[256];
    const int length = std::snprintf(script, 255, "%s/oxquiz.lua", LocaleService_GetBasePath().c_str());
    lua_State* state = quest::CQuestManager::instance().GetLuaState();
    if (length < 0 || length >= 255 || !state || lua_dofile(state, script) != 0)
    {
        character->ChatPacket(1, "%s", LC_TEXT("OX \271\256\301\246 \275\272\305\251\270\263\306\256 \267\316\265\371 \275\307\306\320"));
        return;
    }

    character->ChatPacket(1, "%s", LC_TEXT("OX \271\256\301\246 \275\272\305\251\270\263\306\256 \267\316\265\371 \274\272\260\370"));
    if (LC_IsJapan())
    {
        BroadcastOXNotice("\226{\223\372\202\340\202l\202d\202s\202h\202m\202Q\202\311\202\262\216Q\211\301\202\242\202\275\202\276\202\253\201A\202\240\202\350\202\252\202\306\202\244\202\262\202\264\202\242\202\334\202\267\201B");
        BroadcastOXNotice("\201\233\201~\203C\203x\203\223\203g\202\252\216n\202\334\202\350\202\334\202\265\202\275\201B");
        BroadcastOXNotice("\211\244\223\224\202\360\222\312\202\266\202\304\223\374\217\352\202\305\202\253\202\334\202\267\201B");
        BroadcastOXNotice("\221\275\202\255\202\314\202\262\216Q\211\301\202\360\202\250\221\322\202\277\202\265\202\304\202\250\202\350\202\334\202\267\201B");
    }
    else
    {
        BroadcastOXNotice(LC_TEXT("\276\310\263\347\307\317\274\274\277\344. \270\336\306\2762 \300\324\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("OX \300\314\272\245\306\256\260\241 \275\303\300\333\265\307\276\372\275\300\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("\300\273\265\316\301\366\270\246 \305\353\307\317\277\251 \300\324\300\345 \260\241\264\311\307\325\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("\270\271\300\272 \302\374\277\251 \272\316\305\271\265\345\270\263\264\317\264\331."));
    }
    COXEventManager::instance().SetStatus(OXEVENT_OPEN);
}

void do_oxevent_door(CHARACTER* character, char*, int, int)
{
    COXEventManager& manager = COXEventManager::instance();
    const auto status = manager.GetStatus();
    if (status != OXEVENT_OPEN)
        character->ChatPacket(1, LC_TEXT("OX\300\314\272\245\306\256\260\241 \277\255\267\301 \300\326\301\366 \276\312\260\305\263\252 \271\256\301\246 \303\342\301\246\301\337\300\324\264\317\264\331.(Flag: %d)"), status);
    COXEventManager::instance().SetStatus(OXEVENT_CLOSE);
    character->ChatPacket(1, "%s", LC_TEXT("\264\365 \300\314\273\363 OX\300\314\272\245\306\256 \302\374\260\241\300\332\270\246 \271\336\301\366 \276\312\275\300\264\317\264\331."));
}

void do_oxevent_close(CHARACTER* character, char* argument, int, int)
{
    COXEventManager& manager = COXEventManager::instance();
    const auto status = manager.GetStatus();
    if (status != OXEVENT_CLOSE)
    {
        character->ChatPacket(1, LC_TEXT("OX\300\314\272\245\306\256\260\241 \264\335\310\367\301\366 \276\312\276\322\260\305\263\252, \304\373\301\356 \303\342\301\246\301\337\300\324\264\317\264\331.(flag: %d)"), status);
        return;
    }

    char arg[256];
    one_argument(argument, arg);
    if (std::atoi(arg) == 1)
    {
        BroadcastOXNotice(LC_TEXT("\276\310\263\347\307\317\274\274\277\344. \270\336\306\2762 \300\324\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("\300\341\275\303 \310\304 OX \300\314\272\245\306\256\270\246 \264\331\275\303 \275\303\300\333\307\317\260\332\275\300\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("\307\366\300\347 \300\314\272\245\306\256 \302\374\260\241\300\332 \272\320\265\351\300\272 \270\266\300\273\267\316 \300\314\265\277 \265\307\260\332\275\300\264\317\264\331."));
    }
    else if (LC_IsJapan())
    {
        BroadcastOXNotice("\201\233\201~\203C\203x\203\223\203g\202\360\217I\227\271\202\265\202\334\202\267\201B");
        BroadcastOXNotice("\203C\203x\203\223\203g\211\357\217\352\202\311\202\242\202\351\225\373\201X\202\315\202\265\202\316\202\347\202\255\202\265\202\304\221\272\202\311\210\332\223\256\202\265\202\334\202\267\201B");
    }
    else
    {
        BroadcastOXNotice(LC_TEXT("\276\310\263\347\307\317\274\274\277\344. \270\336\306\2762 \300\324\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("OX \300\314\272\245\306\256\270\246 \270\266\304\241\260\331\275\300\264\317\264\331."));
        BroadcastOXNotice(LC_TEXT("\300\341\275\303 \310\304 \270\266\300\273\267\316 \300\314\265\277\265\307\260\332\275\300\264\317\264\331."));
    }
    COXEventManager::instance().SetStatus(OXEVENT_FINISH);
    void* data;
    const DWORD id = __AllocEventInfo(sizeof(int), &data);
    const auto delay = static_cast<std::int32_t>(static_cast<std::uint32_t>(passes_per_sec) * 5U);
    event_create_ex(end_oxevent, id, delay, const_cast<char*>("cmd_oxevent.cpp"), 0x66);
}

void do_oxevent_end_force(CHARACTER* character, char*, int, int)
{
    character->ChatPacket(1, "%s", LC_TEXT("OX\300\314\272\245\306\256\270\246 \260\255\301\246 \301\276\267\341 \275\303\305\265\264\317\264\331."));
    COXEventManager::instance().CloseEvent();
    COXEventManager::instance().SetStatus(OXEVENT_FINISH);
}

void do_oxevent_quiz(CHARACTER* character, char* argument, int, int)
{
    COXEventManager& manager = COXEventManager::instance();
    const auto status = manager.GetStatus();
    if (status != OXEVENT_CLOSE)
    {
        character->ChatPacket(1, LC_TEXT("OX\300\314\272\245\306\256 \270\312\300\273 \270\325\300\372 \264\335\276\306 \301\326\274\274\277\344.(Flag: %d)"), status);
        return;
    }
    char level[256], seconds[256];
    two_arguments(argument, level, seconds);
    COXEventManager& quizManager = COXEventManager::instance();
    const int duration = std::atoi(seconds);
    const auto quizLevel = static_cast<BYTE>(std::atoi(level));
    const bool started = quizManager.Quiz(quizLevel, duration);
    character->ChatPacket(1, "%s", LC_TEXT(started ? "\271\256\301\246\270\246 \303\342\301\246\307\317\277\264\275\300\264\317\264\331." : "\271\256\301\246\270\246 \303\342\301\246\307\317\301\366 \270\370\307\337\275\300\264\317\264\331."));
}

void do_oxevent_show_quiz(CHARACTER* character, char*, int, int)
{
    character->ChatPacket(1, "===== OX QUIZ LIST =====");
    COXEventManager::instance().ShowQuizList(character);
    character->ChatPacket(1, "===== OX QUIZ LIST END =====");
}

void do_oxevent_log(CHARACTER* character, char*, int, int)
{
    const bool logged = COXEventManager::instance().LogWinner();
    character->ChatPacket(1, "%s", LC_TEXT(logged ? "OX\300\314\272\245\306\256\300\307 \263\252\270\323\301\366 \300\316\277\370 \261\342\267\317\300\273 \275\307\306\320\307\337\275\300\264\317\264\331." : "OX\300\314\272\245\306\256\300\307 \263\252\270\323\301\366 \300\316\277\370\300\273 \261\342\267\317\307\317\277\264\275\300\264\317\264\331."));
}

void do_oxevent_get_attender(CHARACTER* character, char*, int, int)
{
    const int count = static_cast<int>(COXEventManager::instance().GetAttenderCount());
    character->ChatPacket(1, LC_TEXT("\307\366\300\347 \263\262\300\272 \302\374\260\241\300\332\274\366 : %d"), count);
}

void do_oxevent_give_item(CHARACTER* character, char* argument, int, int)
{
    char item[256], quantity[256];
    two_arguments(argument, item, quantity);
    const DWORD vnum = static_cast<DWORD>(std::atoi(item));
    const BYTE count = static_cast<BYTE>(std::atoi(quantity));
    COXEventManager::instance().GiveItemToAttender(vnum, count);
    const auto attendees = static_cast<std::uint32_t>(COXEventManager::instance().GetAttenderCount());
    const auto total = static_cast<std::int32_t>(attendees * count);
    character->ChatPacket(1, LC_TEXT("OX \273\363\307\260\300\273 \301\366\261\336\307\317\277\264\275\300\264\317\264\331. ID: %d \274\366\267\256: %d"), static_cast<int>(vnum),
                          static_cast<int>(total));
}
