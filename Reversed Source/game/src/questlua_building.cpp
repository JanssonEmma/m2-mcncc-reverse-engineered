#include "building.h"
#include "char.h"
#include "config.h"
#include "db.h"
#include "guild.h"
#include "questmanager.h"
#include "../../libthecore/include/log.h"
#include <cstdint>
#include <cstdlib>
#include <memory>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
std::int32_t LandCoordinate(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value > -2147483649.0 && value < 2147483648.0))
        return INT32_MIN;
    return static_cast<std::int32_t>(value);
}

std::uint32_t LandID(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0))
        return 0;
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
}
}

int building_get_land_id(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2) || !lua_isnumber(state, 3)) {
        _sys_err("building_get_land_id", 0x15, "invalid argument");
        lua_pushnumber(state, 0);
        return 1;
    }

    const auto y = LandCoordinate(state, 3);
    const auto x = LandCoordinate(state, 2);
    const auto map = LandCoordinate(state, 1);
    building::CLand* land = building::CManager::instance().FindLand(map, x, y);
    lua_pushnumber(state, land ? land->GetData().dwID : 0);
    return 1;
}

int building_get_land_info(lua_State* state)
{
    int price = 1000000000;
    int owner = 1000000000;
    int levelLimit = 1000000000;
    if (!lua_isnumber(state, 1)) {
        _sys_err("building_get_land_info", 0x35, "invalid argument");
    } else {
        const std::uint32_t id = LandID(state, 1);
        if (building::CLand* land = building::CManager::instance().FindLand(id)) {
            const building::TLand& data = land->GetData();
            price = data.dwPrice;
            owner = data.dwGuildID;
            levelLimit = data.bGuildLevelLimit;
        }
    }

    lua_pushnumber(state, price);
    lua_pushnumber(state, owner);
    lua_pushnumber(state, levelLimit);
    return 3;
}

int building_set_land_owner(lua_State* state)
{
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2)) {
        _sys_err("building_set_land_owner", 0x41, "invalid argument");
        return 0;
    }

    const std::uint32_t id = LandID(state, 1);
    building::CLand* land = building::CManager::instance().FindLand(id);
    if (land && !land->GetData().dwGuildID) {
        const std::uint32_t guild = LandID(state, 2);
        land->SetOwner(guild);
    }
    return 0;
}

int building_has_land(lua_State* state)
{
    if (!lua_isnumber(state, 1)) {
        _sys_err("building_has_land", 0x58, "invalid argument");
        lua_pushboolean(state, true);
        return 1;
    }

    const std::uint32_t guild = LandID(state, 1);
    std::unique_ptr<SQLMsg> message(DBManager::instance().DirectQuery(
        "SELECT COUNT(*) FROM land%s WHERE guild_id = %d", get_table_postfix(),
        static_cast<int>(guild)));
    SQLResult* result = message ? message->Get() : 0;
    bool hasLand = true;
    if (result && result->iNumRows > 0 && result->pSQLResult) {
        MYSQL_ROW row = mysql_fetch_row(result->pSQLResult);
        if (row && row[0])
            hasLand = std::atoi(row[0]) != 0;
    }
    lua_pushboolean(state, hasLand);
    return 1;
}

int building_reconstruct(lua_State* state)
{
    const std::uint32_t vnum = LandID(state, 1);
    CHARACTER* npc = CQuestManager::instance().GetCurrentNPCCharacterPtr();
    if (npc && npc->GetGuild()) {
        if (building::CLand* land = building::CManager::instance().FindLandByGuild(npc->GetGuild()->GetID())) {
            if (building::CObject* object = land->FindObjectByNPC(npc))
                object->Reconstruct(vnum);
        }
    }
    return 0;
}

void RegisterBuildingFunctionTable()
{
    luaL_reg functions[] = {
        {"get_land_id", building_get_land_id},
        {"get_land_info", building_get_land_info},
        {"set_land_owner", building_set_land_owner},
        {"has_land", building_has_land},
        {"reconstruct", building_reconstruct},
        {0, 0},
    };
    CQuestManager::instance().AddLuaFunctionTable("building", functions);
}
}

