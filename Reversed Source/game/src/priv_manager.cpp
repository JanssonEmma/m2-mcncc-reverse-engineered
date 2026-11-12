#include "priv_manager.h"
#include "constants.h"
#include "char.h"
#include "guild.h"
#include "guild_manager.h"
#include "desc_client.h"
#include "locale.hpp"
#include "log.h"
#include "utils.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <cstdio>

void SendLog(const char* text);
void SendNotice(const char* text);
#include <algorithm>

int CPrivManager::GetPrivByCharacter(DWORD pid, BYTE type)
{
    if (type >= 5)
        return 0;
    const auto& entries = m_aPrivChar[type];
    const auto found = entries.find(pid);
    return found != entries.end() ? found->second : 0;
}

int CPrivManager::GetPriv(CHARACTER* character, BYTE type)
{
    if (type >= 5)
        return 0;
    int value = GetPrivByCharacter(character->GetPlayerID(), type);
    if (value < 0 && !character->IsEquipUniqueItem(0x111a4))
        return value;

    value = std::max(value, GetPrivByEmpire(0, type));
    value = std::max(value, GetPrivByEmpire(character->GetEmpire(), type));
    if (CGuild* guild = character->GetGuild())
        value = std::max(value, GetPrivByGuild(guild->GetID(), type));
    return value;
}

namespace {

const char guildPrivExpired[] = "%s \261\346\265\345\300\307 %s\300\314 \301\244\273\363\300\270\267\316 \265\271\276\306\277\324\275\300\264\317\264\331.";
const char guildPrivGranted[] = "%s \261\346\265\345\300\307 %s\300\314 %d%% \301\365\260\241\307\337\275\300\264\317\264\331!";
const char empirePrivExpired[] = "%s\300\307 %s\300\314 \301\244\273\363\300\270\267\316 \265\271\276\306\277\324\275\300\264\317\264\331.";
const char empirePrivGranted[] = "%s\300\307 %s\300\314 %d%% \301\365\260\241\307\337\275\300\264\317\264\331!";

#pragma pack(push, 1)
struct RequestEmpirePrivilege { BYTE type; std::int32_t value; BYTE empire; std::int32_t duration; };
struct RequestGuildPrivilege { BYTE type; std::int32_t value; std::uint32_t guild; std::int32_t duration; };
struct RequestCharacterPrivilege { BYTE type; std::int32_t value; std::uint32_t pid; };
#pragma pack(pop)
static_assert(sizeof(RequestEmpirePrivilege) == 10, "empire privilege request wire size");
static_assert(sizeof(RequestGuildPrivilege) == 13, "guild privilege request wire size");
static_assert(sizeof(RequestCharacterPrivilege) == 9, "character privilege request wire size");
}

CPrivManager::CPrivManager() : m_aakPrivEmpireData{} {}
CPrivManager::~CPrivManager() = default;

const char* GetPrivName(int type)
{
    return type >= 0 && type < 5 ? locale_find(c_apszPrivNames[type], g_iUseLocale) : "";
}

const char* GetEmpireName(int empire)
{
    return empire >= 0 && empire < 4 ? locale_find(c_apszEmpireNames[empire], g_iUseLocale) : "";
}

// 08188c80
void CPrivManager::RequestRemoveGuildPriv(DWORD, BYTE) {}

// 08188c90
void CPrivManager::RequestRemoveEmpirePriv(BYTE, BYTE) {}

void CPrivManager::RequestGiveEmpirePriv(BYTE empire, BYTE type, int value, time_t duration)
{
    const RequestEmpirePrivilege packet = {type, value, empire, static_cast<std::int32_t>(duration)};
    db_clientdesc->DBPacket(48, 0, &packet, sizeof(packet));
}

void CPrivManager::RequestGiveGuildPriv(DWORD guild, BYTE type, int value, time_t duration)
{
    const RequestGuildPrivilege packet = {type, value, static_cast<std::uint32_t>(guild),
                                         static_cast<std::int32_t>(duration)};
    db_clientdesc->DBPacket(49, 0, &packet, sizeof(packet));
}

void CPrivManager::RequestGiveCharacterPriv(DWORD pid, BYTE type, int value)
{
    const RequestCharacterPrivilege packet = {type, value, static_cast<std::uint32_t>(pid)};
    db_clientdesc->DBPacket(54, 0, &packet, sizeof(packet));
}

void CPrivManager::RemoveGuildPriv(DWORD guild, BYTE type)
{
    if (type < 5)
        m_aPrivGuild[type][guild] = {0, 0};
}

void CPrivManager::RemoveEmpirePriv(BYTE empire, BYTE type)
{
    if (SPrivEmpireData* entry = GetPrivByEmpireEx(empire, type))
        *entry = {0, 0};
}

void CPrivManager::RemoveCharacterPriv(DWORD pid, BYTE type)
{
    if (type < 5)
        m_aPrivChar[type].erase(pid);
}

CPrivManager::SPrivEmpireData* CPrivManager::GetPrivByEmpireEx(BYTE empire, BYTE type)
{
    return type < 5 && empire < 4 ? &m_aakPrivEmpireData[type][empire] : nullptr;
}

const CPrivManager::SPrivGuildData* CPrivManager::GetPrivByGuildEx(DWORD guild, BYTE type) const
{
    if (type >= 5)
        return nullptr;
    const auto& privileges = m_aPrivGuild[type];
    const auto entry = privileges.find(guild);
    return entry != privileges.end() ? &entry->second : nullptr;
}

int CPrivManager::GetPrivByEmpire(BYTE empire, BYTE type)
{
    const auto* entry = GetPrivByEmpireEx(empire, type);
    return entry ? entry->m_value : 0;
}

int CPrivManager::GetPrivByGuild(DWORD guild, BYTE type)
{
    const auto* entry = GetPrivByGuildEx(guild, type);
    return entry ? entry->value : 0;
}

void CPrivManager::GiveEmpirePriv(BYTE empire, BYTE type, int value, BYTE log, time_t endTime)
{
    auto* entry = GetPrivByEmpireEx(empire, type);
    if (!entry)
        return;
    sys_log(0, "Set Empire Priv: empire(%d) type(%d) value(%d) duration_sec(%d)",
            empire, type, value, static_cast<int>(endTime - get_global_time()));
    *entry = {value, endTime};
    char notice[100];
    const char* format = locale_find(value ? empirePrivGranted : empirePrivExpired, g_iUseLocale);
    std::snprintf(notice, sizeof(notice), format, GetEmpireName(empire), GetPrivName(type), value);
    if (empire == 0)
        SendLog(notice);
    else
        SendNotice(notice);
    if (log)
        LogManager::instance().CharLog(0, empire, type, value, "EMPIRE_PRIV", "");
}

void CPrivManager::GiveGuildPriv(DWORD guild, BYTE type, int value, BYTE log, time_t endTime)
{
    if (type >= 5)
        return;
    sys_log(0, "Set Guild Priv: guild_id(%lu) type(%d) value(%d) duration_sec(%d)",
            guild, type, value, static_cast<int>(endTime - get_global_time()));
    m_aPrivGuild[type][guild] = {value, endTime};
    if (const CGuild* entry = CGuildManager::instance().FindGuild(guild)) {
        char notice[100];
        const char* format = locale_find(value ? guildPrivGranted : guildPrivExpired, g_iUseLocale);
        std::snprintf(notice, sizeof(notice), format, entry->GetName(), GetPrivName(type), value);
        SendNotice(notice);
        if (log)
            LogManager::instance().CharLog(0, guild, type, value, "GUILD_PRIV", "");
    }
}

void CPrivManager::GiveCharacterPriv(DWORD pid, BYTE type, int value, BYTE log)
{
    if (type >= 5)
        return;
    sys_log(0, "Set Character Priv %lu %d %d", pid, type, value);
    m_aPrivChar[type][pid] = value;
    if (log)
        LogManager::instance().CharLog(pid, 0, type, value, "CHARACTER_PRIV", "");
}
