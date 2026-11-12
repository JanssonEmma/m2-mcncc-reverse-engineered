#include "questmanager.h"
#include "char.h"
#include "desc_client.h"
#include "item.h"
#include "item_manager.h"
#include "../../libthecore/include/utils.h"

#include <cstdint>
#include <limits>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
#pragma pack(push, 1)
struct SafeboxSizeRequest {
    std::uint32_t account;
    BYTE size;
};
#pragma pack(pop)
static_assert(sizeof(SafeboxSizeRequest) == 5, "DB safebox size request");

int GameSignedNumber(lua_State* state, int argument)
{
    const lua_Number value = lua_tonumber(state, argument);
    return value > -2147483649.0 && value < 2147483648.0
        ? static_cast<int>(value) : std::numeric_limits<int>::min();
}
}

int game_set_event_flag(lua_State* state)
{
    if (lua_isstring(state, 1) && lua_isnumber(state, 2)) {
        const int value = GameSignedNumber(state, 2);
        CQuestManager::instance().RequestSetEventFlag(lua_tostring(state, 1), value);
    }
    return 0;
}

int game_get_event_flag(lua_State* state)
{
    const int value = lua_isstring(state, 1)
        ? CQuestManager::instance().GetEventFlag(lua_tostring(state, 1)) : 0;
    lua_pushnumber(state, value);
    return 1;
}

int game_request_make_guild(lua_State*)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    DESC* desc = character ? character->GetDesc() : nullptr;
    if (desc) {
        const BYTE header = 0x52;
        desc->Packet(&header, sizeof(header));
    }
    return 0;
}

int game_get_safebox_level(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    lua_pushnumber(state, character ? character->GetSafeboxSize() / 9 : 0);
    return 1;
}

int game_set_safebox_level(lua_State* state)
{
    CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr();
    DESC* desc = character ? character->GetDesc() : nullptr;
    const int level = GameSignedNumber(state, -1);
    if (desc && level >= 0 && level <= 255) {
        const SafeboxSizeRequest request = {
            static_cast<std::uint32_t>(desc->GetAccountTable().id), static_cast<BYTE>(level)
        };
        db_clientdesc->DBPacket(0x0e, desc->GetHandle(), &request, sizeof(request));
        character->SetSafeboxSize(level * 9);
    }
    return 0;
}

int game_open_safebox(lua_State*)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr()) {
        character->SetSafeboxOpenPosition();
        character->ChatPacket(5, "ShowMeSafeboxPassword");
    }
    return 0;
}

int game_open_mall(lua_State*)
{
    if (CHARACTER* character = CQuestManager::instance().GetCurrentCharacterPtr()) {
        character->SetSafeboxOpenPosition();
        character->ChatPacket(5, "ShowMeMallPassword");
    }
    return 0;
}

int game_drop_item(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CHARACTER* character = manager.GetCurrentCharacterPtr();
    const auto vnumArg = lua_tonumber(state, 1);
    const auto countArg = lua_tonumber(state, 2);
    if (!character)
        return 0;

    PIXEL_POSITION position = character->GetXYZ();
    const auto vnum = vnumArg >= -9223372036854775808.0 && vnumArg < 9223372036854775808.0
        ? static_cast<std::uint32_t>(static_cast<std::int64_t>(vnumArg)) : 0U;
    const auto count = countArg > -2147483649.0 && countArg < 2147483648.0
        ? static_cast<std::int32_t>(countArg) : INT32_MIN;
    ITEM_MANAGER& items = ITEM_MANAGER::instance();
    CItem* item = items.CreateItem(vnum, static_cast<std::uint32_t>(count), 0, false, -1);
    if (!item) {
        manager.QuestError("game_drop_item", 0x66, "cannot create item vnum %d count %d",
                           static_cast<std::int32_t>(vnum), count);
        return 0;
    }

    const int dx = number_ex(-200, 200, const_cast<char*>("questlua_game.cpp"), 0x6b);
    const int dy = number_ex(-200, 200, const_cast<char*>("questlua_game.cpp"), 0x6c);
    position.x = static_cast<std::int32_t>(static_cast<std::uint32_t>(position.x) + dx);
    position.y = static_cast<std::int32_t>(static_cast<std::uint32_t>(position.y) + dy);
    item->AddToGround(character->GetMapIndex(), position);
    item->StartDestroyEvent();
    return 0;
}

// 0x080b3010, table at 0x0820296c.
void RegisterGameFunctionTable()
{
    luaL_reg functions[] = {
        {"get_safebox_level", game_get_safebox_level},
        {"request_make_guild", game_request_make_guild},
        {"set_safebox_level", game_set_safebox_level},
        {"open_safebox", game_open_safebox},
        {"open_mall", game_open_mall},
        {"get_event_flag", game_get_event_flag},
        {"set_event_flag", game_set_event_flag},
        {"drop_item", game_drop_item},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("game", functions);
}
}
