#include "OXEvent.h"
#include "log.h"
#include "char.h"
#include "char_manager.h"
#include "constants.h"
#include <cstring>
#include <cstdio>
#include <cstdint>
#include "config.h"
#include "locale.hpp"
#include "locale_service.h"
#include "../../libthecore/include/utils.h"

void SendNoticeMap(const char* text, int map, bool bigFont);

namespace {
constexpr int oxMap = 113;
struct OXTimerInfo {
    bool answer;
};
void OXNotice(const char* text)
{
    SendNoticeMap(locale_find(text, g_iUseLocale), oxMap, true);
}
}
#include "questmanager.h"
#include "../../libthecore/include/log.h"

// 08087b40
void COXEventManager::ClearQuiz()
{
    for (auto& quizzes : m_vec_quiz)
        quizzes.clear();
    m_vec_quiz.clear();
}

bool COXEventManager::ShowQuizList(CHARACTER* character)
{
    int count = 0;
    for (const auto& quizzes : m_vec_quiz)
    {
        for (const auto& quiz : quizzes)
        {
            character->ChatPacket(1, "%d %s %s", static_cast<int>(static_cast<std::int8_t>(quiz.level)), quiz.Quiz,
                                  LC_TEXT(quiz.answer ? "\302\374" : "\260\305\301\376"));
            ++count;
        }
    }
    character->ChatPacket(1, LC_TEXT("\303\321 \304\373\301\356 \274\366: %d"), count);
    return true;
}

bool COXEventManager::LogWinner()
{
    for (const auto& attender : m_map_attender)
        LogManager::instance().CharLog(attender.second, 0, 0, 0, "OXEVENT", "LastManStanding");
    return true;
}

bool COXEventManager::Initialize()
{
    m_map_char.clear();
    m_map_attender.clear();
    m_vec_quiz.clear();
    SetStatus(OXEVENT_FINISH);
    return true;
}

void COXEventManager::Destroy()
{
    for (LPEVENT timer : m_timedEvents)
        event_cancel(&timer);
    m_timedEvents.clear();
    CloseEvent();
    Initialize();
}

void COXEventManager::SetStatus(OXEventStatus status)
{
    quest::CQuestManager::instance().RequestSetEventFlag("oxevent_status", status);
}

bool COXEventManager::CloseEvent()
{
    for (const auto& participant : m_map_char) {
        CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(participant.second);
        if (!character)
            continue;

        const BYTE empire = character->GetEmpire();
        const long* start = g_start_position[empire < 4 ? empire : 0];
        character->WarpSet(start[0], start[1], 0);
    }
    m_map_char.clear();
    return true;
}

OXEventStatus COXEventManager::GetStatus()
{
    const BYTE status = static_cast<BYTE>(quest::CQuestManager::instance().GetEventFlag("oxevent_status"));
    return status <= OXEVENT_QUIZ ? static_cast<OXEventStatus>(status) : OXEVENT_ERR;
}

bool COXEventManager::AddQuiz(unsigned char level, const char* question, bool answer)
{
    if (!question)
        return false;
    if (m_vec_quiz.size() <= level)
        m_vec_quiz.resize(static_cast<unsigned>(level) + 1);

    tag_Quiz quiz = {};
    quiz.level = static_cast<char>(level);
    std::strncpy(quiz.Quiz, question, sizeof(quiz.Quiz) - 1);
    quiz.answer = answer;
    m_vec_quiz[level].push_back(quiz);
    return true;
}


long oxevent_timer(LPEVENT event);

bool COXEventManager::Quiz(unsigned char level, int timeLimit)
{
    if (m_vec_quiz.empty())
        return false;
    if (level >= m_vec_quiz.size())
        level = static_cast<unsigned char>(m_vec_quiz.size() - 1);
    auto& entries = m_vec_quiz[level];
    if (entries.empty())
        return false;
    if (timeLimit < 0)
        timeLimit = 30;

    const int index = number_ex(0, entries.size() - 1, "OXEvent.cpp", 0xe7);
    const tag_Quiz& quiz = entries[index];
    OXNotice("\271\256\301\246\040\300\324\264\317\264\331\056");
    SendNoticeMap(quiz.Quiz, oxMap, true);
    OXNotice("\270\302\300\270\270\351\040\117\054\040\306\262\270\256\270\351\040\130\267\316\040\300\314\265\277\307\330\301\326\274\274\277\344");
    OXTimerInfo* info;
    const DWORD id = __AllocEventInfo(4, reinterpret_cast<void**>(&info));
    info->answer = quiz.answer;
    const std::uint32_t delayBits =
        static_cast<std::uint32_t>(timeLimit - 15) * static_cast<std::uint32_t>(passes_per_sec);
    std::int32_t delay;
    std::memcpy(&delay, &delayBits, sizeof(delay));
    m_timedEvents.insert(event_create_ex(oxevent_timer, id, delay,
                                        const_cast<char*>("OXEvent.cpp"), 0xf3));
    SetStatus(OXEVENT_QUIZ);
    entries.erase(entries.begin() + index);
    return true;
}

// 08087bc0
EVENTFUNC(oxevent_timer)
{
    auto& manager = COXEventManager::instance();
    auto* info = static_cast<OXTimerInfo*>(event->info);
    static BYTE flag = 0;
    if (flag == 0) {
        OXNotice("\061\060\303\312\265\332\040\306\307\301\244\307\317\260\332\275\300\264\317\264\331\056");
        ++flag;
        return passes_per_sec * 10;
    }
    if (flag == 1) {
        OXNotice("\301\244\264\344\300\272");
        manager.CheckAnswer(info->answer);
        OXNotice(info->answer ? "\117\040\300\324\264\317\264\331" : "\130\040\300\324\264\317\264\331");
        if (LC_IsJapan())
            SendNoticeMap("\212\324\210\341\202\246\202\275\225\373\201\130\202\360\212\117\202\311\210\332\223\256\202\263\202\271\202\334\202\267\201\102", oxMap, true);
        else
            OXNotice("\065\303\312\040\265\332\040\306\262\270\256\275\305\040\272\320\265\351\300\273\040\271\331\261\371\300\270\267\316\040\300\314\265\277\040\275\303\305\260\260\332\275\300\264\317\264\331\056");
        ++flag;
        return passes_per_sec * 5;
    }
    manager.m_timedEvents.erase(event);
    if (flag != 2)
        return 0;

    manager.WarpToAudience();
    manager.SetStatus(OXEVENT_CLOSE);
    OXNotice("\264\331\300\275\040\271\256\301\246\040\301\330\272\361\307\330\301\326\274\274\277\344\056");
    flag = 0;
    return 0;
}

bool COXEventManager::CheckAnswer(bool answer)
{
    if (m_map_attender.empty())
        return true;
    m_map_miss.clear();
    const long minX = answer ? 0xdae58 : 0xd9eb8;
    const long maxX = answer ? 0xdbccc : 0xdad2c;
    for (auto it = m_map_attender.begin(); it != m_map_attender.end(); ) {
        const auto entry = it++;
        const DWORD pid = entry->second;
        CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(pid);
        if (!character) {
            m_map_char.erase(pid);
            m_map_attender.erase(entry);
            continue;
        }

        const long x = character->GetX();
        const long y = character->GetY();
        if (x < minX || x > maxX || y < 0x5974 || y > 0x6720) {
            character->EffectPacket(12);
            m_map_attender.erase(entry);
            m_map_miss.emplace(pid, pid);
            continue;
        }

        character->ChatPacket(1, "%s", locale_find("\301\244\264\344\300\324\264\317\264\331\041", g_iUseLocale));
        const char* cheer = number_ex(0, 1, "OXEvent.cpp", 299) == 1 ? "cheer1" : "cheer2";
        BYTE packet[9 + 254] = {4, 0, 0, 5};
        const auto vid = static_cast<std::uint32_t>(static_cast<DWORD>(character->GetVID()));
        const int length = std::snprintf(reinterpret_cast<char*>(packet + 9), 254, "%s %lu %lu",
                                         cheer, static_cast<unsigned long>(vid), 0ul);
        const auto size = static_cast<std::uint16_t>(9 + length);
        std::memcpy(packet + 1, &size, sizeof(size));
        character->PacketAround(packet, size);
        character->EffectPacket(11);
    }
    return true;
}

void COXEventManager::WarpToAudience()
{
    for (const auto& entry : m_map_miss) {
        if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(entry.second))
            character->Show(oxMap, 0xdad2c, 0x70e4, 0x7fffffff, false);
    }
    m_map_miss.clear();
}

bool COXEventManager::GiveItemToAttender(DWORD vnum, unsigned char count)
{
    for (const auto& entry : m_map_attender) {
        if (CHARACTER* character = CHARACTER_MANAGER::instance().FindByPID(entry.second)) {
            character->AutoGiveItem(vnum, count, -1, true);
            LogManager::instance().ItemLog(character->GetPlayerID(), 0, count, vnum, "OXEVENT_REWARD", "");
        }
    }
    return true;
}

bool COXEventManager::Enter(CHARACTER* character)
{
    if (GetStatus() == OXEVENT_FINISH) {
        sys_log(0, "OXEVENT : map finished. but char enter. %s", character->GetName());
        return false;
    }

    const long x = character->GetX();
    const long y = character->GetY();
    if (x == 0xdadf4 && y == 0x6018)
        return EnterAttender(character);
    if (x == 0xdad2c && y == 0x70e4)
        return EnterAudience(character);

    sys_log(0, "OXEVENT : wrong pos enter %ld %ld", x, y);
    return false;
}

bool COXEventManager::EnterAttender(CHARACTER* character)
{
    const DWORD pid = character->GetPlayerID();
    m_map_char.emplace(pid, pid);
    m_map_attender.emplace(pid, pid);
    return true;
}

bool COXEventManager::EnterAudience(CHARACTER* character)
{
    const DWORD pid = character->GetPlayerID();
    m_map_char.emplace(pid, pid);
    return true;
}
