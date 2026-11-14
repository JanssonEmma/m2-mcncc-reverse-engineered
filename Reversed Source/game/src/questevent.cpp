#include "questevent.h"
#include "questmanager.h"
#include "char_manager.h"
#include "config.h"
#include "../../libthecore/include/log.h"
#include <cfenv>
#include <cmath>
#include <climits>
#include <cstring>
namespace quest {
namespace {
int TimerPulses(double seconds)
{
    const double pulses = std::rint(passes_per_sec * seconds);
    if (pulses >= INT_MIN && pulses <= INT_MAX)
        return static_cast<int>(pulses);
    std::feraiseexcept(FE_INVALID);
    return INT_MIN;
}

char* CopyTimerName(const char* name)
{
    const auto bytes = std::strlen(name) + 1;
    char* copy = new char[bytes];
    std::memcpy(copy, name, bytes);
    return copy;
}

}

EVENTFUNC(quest_server_timer_event)
{
    auto* info = static_cast<quest_server_event_info*>(event->info);
    auto& quests = CQuestManager::instance();
    const auto arg = info->arg;
    const auto npcId = info->npc_id;
    const bool ran = quests.ServerTimer(npcId, arg);
    if (event->is_force_to_end)
        return 0;
    if (!ran)
        return passes_per_sec / 2 + 1;
    if (info->time_cycle)
        return info->time_cycle;

    {
        const std::string name(info->name);
        quests.ClearServerTimerNotCancel(name, info->arg);
    }
    delete[] info->name;
    info->name = nullptr;
    return 0;
}

EVENTFUNC(quest_timer_event)
{
    auto* info = static_cast<quest_event_info*>(event->info);
    auto& quests = CQuestManager::instance();
    if (CHARACTER_MANAGER::instance().FindByPID(info->player_id)) {
        auto& timerQuests = CQuestManager::instance();
        const auto npcId = info->npc_id;
        const auto playerId = info->player_id;
        const bool ran = timerQuests.Timer(playerId, npcId);
        if (event->is_force_to_end)
            return 0;
        if (!ran)
            return passes_per_sec / 2 + 1;
        if (info->time_cycle)
            return info->time_cycle;
    }

    if (PC* player = quests.GetPC(info->player_id))
        player->RemoveTimerNotCancel(info->name);
    else
        _sys_err("quest_timer_event", 0x41, "quest::PC pointer null. player_id: %lu",
                 static_cast<unsigned long>(info->player_id));
    delete[] info->name;
    info->name = nullptr;
    return 0;
}

LPEVENT quest_create_server_timer_event(const char* name, double seconds, unsigned npc, bool loop, unsigned arg)
{
    const int pulses = TimerPulses(seconds);
    quest_server_event_info* info;
    const DWORD id = __AllocEventInfo(sizeof(*info), reinterpret_cast<void**>(&info));
    info->time_cycle = loop ? pulses : 0;
    info->npc_id = npc;
    info->arg = arg;
    info->name = CopyTimerName(name);
    return event_create_ex(quest_server_timer_event, id, pulses, const_cast<char*>("questevent.cpp"), 0x5a);
}

LPEVENT quest_create_timer_event(const char* name, unsigned player, double seconds, unsigned npc, bool loop)
{
    const int pulses = TimerPulses(seconds);
    quest_event_info* info;
    const DWORD id = __AllocEventInfo(sizeof(*info), reinterpret_cast<void**>(&info));
    info->time_cycle = loop ? pulses : 0;
    info->player_id = player;
    info->npc_id = npc;
    info->name = CopyTimerName(name);
    return event_create_ex(quest_timer_event, id, pulses, const_cast<char*>("questevent.cpp"), 0x6e);
}

void CancelServerTimerEvent(LPEVENT* event)
{
    if (*event) {
        auto* info = static_cast<quest_server_event_info*>(FindEventInfo((*event)->dwInfoID));
        if (info) {
            delete[] info->name;
            info->name = nullptr;
        }
    }
    event_cancel(event);
}

// 081ad980. Deliberate old numeric registry lookup, not modern dynamic_cast.
// ppEvent and *ppEvent must be non-null even though event_cancel accepts null.
void CancelTimerEvent(LPEVENT* ppEvent) {
    quest_event_info* info=static_cast<quest_event_info*>(FindEventInfo((*ppEvent)->dwInfoID));
    if(info) {
        if(info->name) delete[] info->name;
        info->name=0;
    }
    event_cancel(ppEvent);
}
}
