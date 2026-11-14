#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "questmanager.h"

#include <cfenv>
#include <cstdint>

extern "C" {
#include "../../liblua/include/lua.h"
#include "../../liblua/include/lauxlib.h"
}

namespace quest {
namespace {
template <typename Value>
int PushItemValue(lua_State* state, Value (CItem::*getter)())
{
    CItem* item = CQuestManager::instance().GetCurrentItem();
    lua_pushnumber(state, item ? (item->*getter)() : 0);
    return 1;
}

std::int32_t ItemSignedNumber(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value > -2147483649.0 && value < 2147483648.0)) {
        std::feraiseexcept(FE_INVALID);
        return INT32_MIN;
    }
    return static_cast<std::int32_t>(value);
}

std::uint32_t ItemUnsignedNumber(lua_State* state, int index)
{
    const lua_Number value = lua_tonumber(state, index);
    if (!(value >= -9223372036854775808.0 && value < 9223372036854775808.0)) {
        std::feraiseexcept(FE_INVALID);
        return 0;
    }
    return static_cast<std::uint32_t>(static_cast<std::int64_t>(value));
}
}

int item_get_id(lua_State* state) { return PushItemValue(state, &CItem::GetID); }
int item_get_cell(lua_State* state) { return PushItemValue(state, &CItem::GetCell); }
int item_get_vnum(lua_State* state) { return PushItemValue(state, &CItem::GetVnum); }
int item_get_size(lua_State* state) { return PushItemValue(state, &CItem::GetSize); }
int item_get_count(lua_State* state) { return PushItemValue(state, &CItem::GetCount); }
int item_get_type(lua_State* state) { return PushItemValue(state, &CItem::GetType); }
int item_get_sub_type(lua_State* state) { return PushItemValue(state, &CItem::GetSubType); }
int item_get_level(lua_State* state) { return PushItemValue(state, &CItem::GetRefineLevel); }

int item_select(lua_State* state)
{
    lua_pushboolean(state, false);
    if (lua_isnumber(state, 1)) {
        const auto id = ItemUnsignedNumber(state, 1);
        if (CItem* item = ITEM_MANAGER::instance().Find(id)) {
            CQuestManager::instance().SetCurrentItem(item);
            lua_pushboolean(state, true);
        }
    }
    return 1;
}

int item_select_cell(lua_State* state)
{
    lua_pushboolean(state, false);
    if (lua_isnumber(state, 1)) {
        const BYTE cell = static_cast<BYTE>(ItemUnsignedNumber(state, 1));
        CQuestManager& manager = CQuestManager::instance();
        if (CHARACTER* character = manager.GetCurrentCharacterPtr()) {
            if (CItem* item = character->GetItem(cell)) {
                manager.SetCurrentItem(item);
                lua_pushboolean(state, true);
            }
        }
    }
    return 1;
}

int item_remove(lua_State*)
{
    CQuestManager& manager = CQuestManager::instance();
    if (CItem* item = manager.GetCurrentItem()) {
        ITEM_MANAGER::instance().RemoveItem(item, nullptr);
        manager.ClearCurrentItem();
    }
    return 0;
}

int item_get_socket(lua_State* state)
{
    auto& manager = CQuestManager::instance();
    const auto index = manager.GetCurrentItem() && lua_isnumber(state, 1) ? ItemSignedNumber(state, 1) : -1;
    std::int32_t value = 0;
    if (index >= 0 && index < 3) {
        if (CItem* item = manager.GetCurrentItem())
            value = static_cast<std::int32_t>(item->GetSockets()[index]);
    }
    lua_pushnumber(state, value);
    return 1;
}

int item_set_socket(lua_State* state)
{
    CItem* item = CQuestManager::instance().GetCurrentItem();
    if (item && lua_isnumber(state, 1) && lua_isnumber(state, 2)) {
        const auto index = ItemSignedNumber(state, 1);
        const auto value = ItemSignedNumber(state, 2);
        if (index >= 0 && index < 3)
            item->SetSocket(index, value);
    }
    return 0;
}

int item_has_flag(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    CItem* item = manager.GetCurrentItem();
    bool hasFlag = false;
    if (!lua_isnumber(state, 1))
        manager.QuestError("item_has_flag", 0x95, "flag is not a number.");
    else if (item) {
        const auto mask = ItemSignedNumber(state, 1);
        hasFlag = (static_cast<std::int32_t>(item->GetFlag()) & mask) != 0;
    }
    lua_pushboolean(state, hasFlag);
    return 1;
}

int item_get_value(lua_State* state)
{
    CQuestManager& manager = CQuestManager::instance();
    std::int32_t result = 0;
    if (CItem* item = manager.GetCurrentItem()) {
        if (!lua_isnumber(state, 1)) {
            manager.QuestError("item_get_value", 0xb3, "index is not a number");
        } else {
            const auto index = ItemSignedNumber(state, 1);
            if (index < 0 || index >= 6)
                manager.QuestError("item_get_value", 0xbc, "index(%d) is out of range (0..%d)", index, 6);
            else
                result = static_cast<std::int32_t>(item->GetValue(index));
        }
    }
    lua_pushnumber(state, result);
    return 1;
}

int item_get_name(lua_State* state)
{
    CItem* item = CQuestManager::instance().GetCurrentItem();
    lua_pushstring(state, item ? item->GetName() : "");
    return 1;
}

int item_get_refine_vnum(lua_State* state)
{
    CItem* item = CQuestManager::instance().GetCurrentItem();
    const TItemTable* proto = item ? item->GetProto() : nullptr;
    lua_pushnumber(state, proto ? proto->dwRefinedVnum : 0);
    return 1;
}

int item_next_refine_vnum(lua_State* state)
{
    const auto vnum = lua_isnumber(state, 1) ? ItemUnsignedNumber(state, 1) : 0U;
    const TItemTable* proto = ITEM_MANAGER::instance().GetTable(vnum);
    if (!proto) {
        CQuestManager::instance().QuestError("item_next_refine_vnum", 0x120,
            "Cannot find item table of vnum %lu", static_cast<unsigned long>(vnum));
    }
    lua_pushnumber(state, proto ? proto->dwRefinedVnum : 0);
    return 1;
}

// 0x080f10b0, table at 0x0821d734.
void RegisterITEMFunctionTable()
{
    luaL_reg functions[] = {
        {"get_id", item_get_id},
        {"get_cell", item_get_cell},
        {"select", item_select},
        {"select_cell", item_select_cell},
        {"remove", item_remove},
        {"get_socket", item_get_socket},
        {"set_socket", item_set_socket},
        {"get_vnum", item_get_vnum},
        {"has_flag", item_has_flag},
        {"get_value", item_get_value},
        {"get_name", item_get_name},
        {"get_size", item_get_size},
        {"get_count", item_get_count},
        {"get_type", item_get_type},
        {"get_sub_type", item_get_sub_type},
        {"get_refine_vnum", item_get_refine_vnum},
        {"get_level", item_get_level},
        {"next_refine_vnum", item_next_refine_vnum},
        {nullptr, nullptr}
    };
    CQuestManager::instance().AddLuaFunctionTable("item", functions);
}
}
