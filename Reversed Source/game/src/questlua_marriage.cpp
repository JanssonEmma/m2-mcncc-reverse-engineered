#include "char.h"
#include "char_manager.h"
#include "marriage.h"
#include "questmanager.h"
#include "utils.h"
#include "wedding.h"
#include <cstdint>
#include <cfenv>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

extern int g_nPortalLimitTime;

namespace quest {
namespace {
LPCHARACTER CurrentMarriageCharacter()
{
    return CQuestManager::instance().GetCurrentCharacterPtr();
}

DWORD ReadMarriageId(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (value >= -9223372036854775808.0 && value < 9223372036854775808.0)
        return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
    std::feraiseexcept(FE_INVALID);
    return 0;
}

marriage::TMarriage* FindMarriage(LPCHARACTER character, const char* function, int line)
{
    if (!character)
        return nullptr;
    auto* record = marriage::CManager::instance().Get(character->GetPlayerID());
    if (!record && function) {
        const char* name = character->GetName();
        const auto playerId = static_cast<std::int32_t>(character->GetPlayerID());
        CQuestManager::instance().QuestError(function, line, "pid[%d:%s] is not exist couple", playerId, name);
    }
    return record;
}

marriage::WeddingMap* FindWedding(marriage::TMarriage* record)
{
    return record && record->pWeddingInfo
        ? marriage::WeddingManager::instance().Find(record->pWeddingInfo->dwMapIndex) : nullptr;
}
}

int marriage_engage_to(lua_State* state)
{
    const DWORD vid = ReadMarriageId(state, 1);
    LPCHARACTER character = CurrentMarriageCharacter();
    LPCHARACTER partner = CHARACTER_MANAGER::instance().Find(vid);
    if (character && partner) {
        const char* partnerName = partner->GetName();
        const char* characterName = character->GetName();
        const DWORD partnerId = partner->GetPlayerID();
        auto& manager = marriage::CManager::instance();
        const DWORD characterId = character->GetPlayerID();
        manager.RequestAdd(characterId, partnerId, characterName, partnerName);
    }
    return 0;
}

int marriage_remove(lua_State*)
{
    LPCHARACTER character = CurrentMarriageCharacter();
    if (auto* record = FindMarriage(character, "marriage_remove", 0x23)) {
        const DWORD playerId = character->GetPlayerID();
        const DWORD partnerId = record->GetOther(playerId);
        marriage::CManager::instance().RequestRemove(playerId, partnerId);
    }
    return 0;
}

int marriage_set_to_marriage(lua_State*)
{
    if (auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_set_to_marriage", 0x30))
        record->SetMarried();
    return 0;
}

int marriage_find_married_vid(lua_State* state)
{
    LPCHARACTER character = CurrentMarriageCharacter();
    DWORD vid = 0;
    if (auto* record = FindMarriage(character, nullptr, 0)) {
        const DWORD partnerId = record->GetOther(character->GetPlayerID());
        LPCHARACTER partner = CHARACTER_MANAGER::instance().FindByPID(partnerId);
        if (partner)
            vid = partner->GetVID();
    }
    lua_pushnumber(state, vid);
    return 1;
}

struct FBuildLuaWeddingMapList
{
    lua_State* L;
    int m_count;

    void operator()(marriage::TMarriage* record)
    {
        if (!record->pWeddingInfo)
            return;

        lua_newtable(L);
        lua_pushnumber(L, record->pid1);
        lua_rawseti(L, -2, 1);
        lua_pushnumber(L, record->pid2);
        lua_rawseti(L, -2, 2);
        lua_pushstring(L, record->name1.c_str());
        lua_rawseti(L, -2, 3);
        lua_pushstring(L, record->name2.c_str());
        lua_rawseti(L, -2, 4);
        lua_rawseti(L, -2, m_count);
        ++m_count;
    }
};

int marriage_get_wedding_list(lua_State* state)
{
    FBuildLuaWeddingMapList build{state, 1};
    lua_newtable(state);
    marriage::CManager::instance().for_each_wedding(build);
    return 1;
}

int marriage_join_wedding(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2)) {
        quests.QuestError("marriage_join_wedding", 0x6d, "invalid player id for wedding map");
        return 0;
    }
    const DWORD first = ReadMarriageId(state, 1);
    const DWORD second = ReadMarriageId(state, 2);
    LPCHARACTER character = quests.GetCurrentCharacterPtr();
    if (!character)
        return 0;
    auto* record = marriage::CManager::instance().Get(first);
    if (!record) {
        quests.QuestError("marriage_join_wedding", 0x79, "pid[%d:%s] is not exist couple",
                          static_cast<int>(character->GetPlayerID()), character->GetName());
    } else if (record->GetOther(first) != second) {
        quests.QuestError("marriage_join_wedding", 0x7e, "not married %lu %lu", first, second);
    } else if (!character->IsHack(true, true, g_nPortalLimitTime)) {
        record->WarpToWeddingMap(character->GetPlayerID());
    }
    return 0;
}

int marriage_warp_to_my_marriage_map(lua_State*)
{
    LPCHARACTER character = CurrentMarriageCharacter();
    auto* record = FindMarriage(character, "marriage_warp_to_my_marriage_map", 0x90);
    if (record && !character->IsHack(true, true, g_nPortalLimitTime))
        record->WarpToWeddingMap(character->GetPlayerID());
    return 0;
}

int marriage_end_wedding(lua_State*)
{
    if (auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_end_wedding", 0xa4))
        record->RequestEndWedding();
    return 0;
}

int marriage_wedding_dark(lua_State* state)
{
    if (!lua_isboolean(state, 1)) {
        CQuestManager::instance().QuestError("marriage_wedding_dark", 0xb3, "invalid argument 1 : must be boolean");
        return 0;
    }
    auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_wedding_dark", 0xbb);
    if (record && record->pWeddingInfo) {
        auto* wedding = FindWedding(record);
        const bool enabled = lua_toboolean(state, 1) != 0;
        if (wedding)
            wedding->SetDark(enabled);
    }
    return 0;
}

int marriage_wedding_client_command(lua_State* state)
{
    if (!lua_isstring(state, 1)) {
        CQuestManager::instance().QuestError("marriage_wedding_client_command", 0xcb,
                                             "invalid argument 1 : must be string");
        return 0;
    }
    auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_wedding_client_command", 0xd3);
    if (record && record->pWeddingInfo) {
        auto* wedding = FindWedding(record);
        const char* command = lua_tostring(state, 1);
        if (wedding)
            wedding->ShoutInMap(5, command);
    }
    return 0;
}

int marriage_wedding_is_playing_music(lua_State* state)
{
    auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_wedding_is_playing_music", 0xe5);
    if (!record)
        return 0;
    if (record->pWeddingInfo) {
        auto* wedding = FindWedding(record);
        lua_pushboolean(state, wedding && wedding->IsPlayingMusic());
    }
    // The original returns the final false even when the map reports music playing.
    lua_pushboolean(state, false);
    return 1;
}

int marriage_wedding_music(lua_State* state)
{
    auto& quests = CQuestManager::instance();
    if (!lua_isboolean(state, 1)) {
        quests.QuestError("marriage_wedding_music", 0xf8, "invalid argument 1 : must be boolean");
        return 0;
    }
    if (!lua_isstring(state, 2)) {
        quests.QuestError("marriage_wedding_music", 0xfd, "invalid argument 2 : must be string");
        return 0;
    }
    auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_wedding_music", 0x105);
    if (record && record->pWeddingInfo) {
        auto* wedding = FindWedding(record);
        const char* filename = lua_tostring(state, 2);
        const bool enabled = lua_toboolean(state, 1) != 0;
        if (wedding)
            wedding->SetMusic(enabled, filename);
    }
    return 0;
}

int marriage_wedding_snow(lua_State* state)
{
    if (!lua_isboolean(state, 1)) {
        CQuestManager::instance().QuestError("marriage_wedding_snow", 0x116, "invalid argument 1 : must be boolean");
        return 0;
    }
    auto* record = FindMarriage(CurrentMarriageCharacter(), "marriage_wedding_snow", 0x11d);
    if (record && record->pWeddingInfo) {
        auto* wedding = FindWedding(record);
        const bool enabled = lua_toboolean(state, 1) != 0;
        if (wedding)
            wedding->SetSnow(enabled);
    }
    return 0;
}

int marriage_in_my_wedding(lua_State* state)
{
    LPCHARACTER character = CurrentMarriageCharacter();
    auto* record = FindMarriage(character, nullptr, 0);
    lua_pushboolean(state, record && record->pWeddingInfo &&
                    static_cast<std::uint32_t>(character->GetMapIndex()) == 
                         static_cast<std::uint32_t>(record->pWeddingInfo->dwMapIndex));
    return 1;
}

int marriage_get_married_time(lua_State* state)
{
    auto* record = FindMarriage(CurrentMarriageCharacter(), nullptr, 0);
    std::int32_t elapsed = 0;
    if (record) {
        const auto now = static_cast<std::uint32_t>(get_global_time());
        elapsed = static_cast<std::int32_t>(now - static_cast<std::uint32_t>(record->marry_time));
    } else {
        CQuestManager::instance().QuestError("marriage_get_married_time", 0x13e,
                                            "trying to get time for not married character");
    }
    lua_pushnumber(state, elapsed);
    return 1;
}

void RegisterMarriageFunctionTable()
{
    luaL_reg functions[] = {
        {"engage_to", marriage_engage_to},
        {"remove", marriage_remove},
        {"find_married_vid", marriage_find_married_vid},
        {"get_wedding_list", marriage_get_wedding_list},
        {"join_wedding", marriage_join_wedding},
        {"set_to_marriage", marriage_set_to_marriage},
        {"end_wedding", marriage_end_wedding},
        {"wedding_dark", marriage_wedding_dark},
        {"wedding_snow", marriage_wedding_snow},
        {"wedding_music", marriage_wedding_music},
        {"wedding_is_playing_music", marriage_wedding_is_playing_music},
        {"wedding_client_command", marriage_wedding_client_command},
        {"in_my_wedding", marriage_in_my_wedding},
        {"warp_to_my_marriage_map", marriage_warp_to_my_marriage_map},
        {"get_married_time", marriage_get_married_time},
        {nullptr, nullptr},
    };
    CQuestManager::instance().AddLuaFunctionTable("marriage", functions);
}
}
